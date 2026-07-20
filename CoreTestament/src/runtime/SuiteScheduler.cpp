#include "SuiteScheduler.hpp"

#include "SuiteInstance.hpp"
#include "Testament/Execution.hpp"

#include <algorithm>
#include <future>

namespace Testament::detail {

bool SuiteScheduler::Result::succeeded() const noexcept {
    return hooksSucceeded && statistics.failedTests() == 0 && statistics.errors() == 0;
}

SuiteScheduler::Result SuiteScheduler::run(
    const std::vector<std::shared_ptr<SuiteInstance>>& suites,
    TestEventHandler& handler, Configuration configuration
) {
    struct SuiteResult {
        bool hooksSucceeded;
        TestCounts<unsigned int> statistics;
    };
    const auto executeSuite = [&handler, configuration](const auto& suite) {
        SuiteResult result;
        result.hooksSucceeded = suite->run(
            &handler,
            SuiteInstance::RunConfiguration{
                configuration.testFilter, configuration.expression,
                configuration.maxParallelTests, configuration.shuffleSeed
            }
        );
        result.statistics = suite->statistics();
        return result;
    };

    Result result;
    const auto consume = [&result](SuiteResult suiteResult) {
        result.hooksSucceeded = suiteResult.hooksSucceeded && result.hooksSucceeded;
        result.statistics += suiteResult.statistics;
    };

    std::size_t index{};
    while (index < suites.size()) {
        if (suites[index]->options().execution() == Execution::Serial) {
            consume(executeSuite(suites[index++]));
            continue;
        }

        const auto concurrentEnd = std::ranges::find_if(
            suites.begin() + static_cast<std::ptrdiff_t>(index), suites.end(),
            [](const auto& suite) {
                return suite->options().execution() == Execution::Serial;
            }
        );
        const auto endIndex = static_cast<std::size_t>(concurrentEnd - suites.begin());
        while (index < endIndex) {
            const auto count = std::min(configuration.maxParallelSuites, endIndex - index);
            std::vector<std::future<SuiteResult>> running;
            running.reserve(count);
            for (std::size_t offset = 0; offset < count; ++offset) {
                running.push_back(std::async(
                    std::launch::async, executeSuite, suites[index + offset]
                ));
            }
            for (auto& future : running) consume(future.get());
            index += count;
        }
    }
    return result;
}

}
