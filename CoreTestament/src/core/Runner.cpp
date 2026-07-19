#include "Testament/Runner.hpp"

#include "Testament/Reporters.hpp"
#include "Testament/TestEventHandler.hpp"

#include "EventHandlers/ConsoleTestEventHandler.hpp"
#include "EventHandlers/BufferedTestEventHandler.hpp"
#include "EventHandlers/JUnitTestEventHandler.hpp"
#include "EventHandlers/TestEventHandlerChain.hpp"
#include "Internal/InternalRegistry.hpp"
#include "Internal/InternalSuite.hpp"
#include "Internal/FilterPattern.hpp"
#include "Internal/utils/TestStatistics.hpp"

#include <algorithm>
#include <filesystem>
#include <future>
#include <iostream>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Testament {

namespace {

std::mutex testRunMutex;

std::optional<std::vector<std::string_view>> commandLineArguments(int argc, char** argv) {
    if (argc < 0 || (argc > 0 && !argv)) {
        return std::nullopt;
    }

    std::vector<std::string_view> arguments;
    arguments.reserve(argc > 1 ? static_cast<std::size_t>(argc - 1) : 0U);
    for (int index = 1; index < argc; ++index) {
        if (!argv[index]) {
            return std::nullopt;
        }
        arguments.emplace_back(argv[index]);
    }
    return arguments;
}

}

class Runner::Impl {
public:
    std::vector<std::unique_ptr<TestEventHandler>> handlers;
    std::optional<std::string> suiteFilter;
    std::optional<std::string> testFilter;
    std::size_t maxParallelSuites{1};
    std::size_t maxParallelTests{1};
};

Runner::Runner() : impl(std::make_unique<Impl>()) {}

Runner::~Runner() = default;

Runner::Runner(Runner&&) noexcept = default;

Runner& Runner::operator=(Runner&&) noexcept = default;

Runner& Runner::addHandler(std::unique_ptr<TestEventHandler> handler) {
    if (handler) {
        if (!impl) impl = std::make_unique<Impl>();
        impl->handlers.push_back(std::move(handler));
    }
    return *this;
}

Runner& Runner::filterSuite(std::string_view name) {
    if (name.empty()) throw std::invalid_argument("Suite filter cannot be empty");
    if (!impl) impl = std::make_unique<Impl>();
    impl->suiteFilter = name;
    return *this;
}

Runner& Runner::filterTest(std::string_view name) {
    if (name.empty()) throw std::invalid_argument("Test filter cannot be empty");
    if (!impl) impl = std::make_unique<Impl>();
    impl->testFilter = name;
    return *this;
}

Runner& Runner::clearFilters() noexcept {
    if (impl) {
        impl->suiteFilter.reset();
        impl->testFilter.reset();
    }
    return *this;
}

Runner& Runner::maxParallelSuites(std::size_t count) {
    if (count == 0) throw std::invalid_argument("Maximum parallel suites must be greater than zero");
    if (!impl) impl = std::make_unique<Impl>();
    impl->maxParallelSuites = count;
    return *this;
}

Runner& Runner::maxParallelTests(std::size_t count) {
    if (count == 0) throw std::invalid_argument("Maximum parallel tests must be greater than zero");
    if (!impl) impl = std::make_unique<Impl>();
    impl->maxParallelTests = count;
    return *this;
}

int Runner::run(int argc, char** argv) {
    const auto arguments = commandLineArguments(argc, argv);
    if (!arguments) {
        std::cerr << "Invalid command-line arguments\n";
        return 2;
    }
    std::optional<std::string> commandLineFilter;
    for (const auto argument : *arguments) {
        if (!argument.starts_with("--filter=")) continue;
        if (commandLineFilter) {
            std::cerr << "--filter may only be specified once\n";
            return 2;
        }
        const auto value = argument.substr(std::string_view{"--filter="}.size());
        if (value.empty() || value == "-" || value == "tag:" || value == "-tag:") {
            std::cerr << "--filter requires a non-empty pattern\n";
            return 2;
        }
        commandLineFilter = value;
    }

    const std::scoped_lock runLock(testRunMutex);
    if (!impl) impl = std::make_unique<Impl>();

    detail::TestEventHandlerChain chain;
    for (const auto& handler : impl->handlers) {
        chain.add(handler.get());
    }
    if (const auto result = chain.configure(*arguments); !result) {
        std::cerr << result.error() << '\n';
        return 2;
    }

    auto& registry = InternalRegistry::getInstance();
    if (const auto errors = registry.getConfigurationErrors(); !errors.empty()) {
        for (const auto& error : errors) std::cerr << "Test configuration error: " << error << '\n';
        return 2;
    }
    auto suites = registry.getAllSuites();
    if (impl->suiteFilter) {
        std::erase_if(suites, [this](const auto& suite) {
            return !detail::matchesNameFilter(suite->getName(), *impl->suiteFilter);
        });
    }
    std::ranges::sort(suites, [](const auto& left, const auto& right) {
        const auto leftOrder = left->getOptions().order().value_or(0);
        const auto rightOrder = right->getOptions().order().value_or(0);
        if (leftOrder != rightOrder) {
            return leftOrder < rightOrder;
        }
        return left->getName() < right->getName();
    });

    chain.onStartReport(static_cast<unsigned int>(suites.size()));

    struct SuiteResult {
        bool hooksSucceeded;
        TestStatistics<unsigned int> statistics;
        detail::BufferedTestEventHandler events;
    };
    const std::string testFilter = impl->testFilter.value_or("");
    const std::string cliFilter = commandLineFilter.value_or("");
    const auto executeSuite = [this, &testFilter, &cliFilter](const auto& suite) {
        SuiteResult result;
        result.hooksSucceeded = suite->run(
            &result.events,
            InternalSuite::RunConfiguration{
                testFilter, cliFilter, impl->maxParallelTests
            }
        );
        result.statistics = suite->getStatistics();
        return result;
    };

    TestStatistics<unsigned int> total;
    bool hooksSucceeded = true;
    const auto consume = [&chain, &hooksSucceeded, &total](SuiteResult result) {
        result.events.replay(chain);
        hooksSucceeded = result.hooksSucceeded && hooksSucceeded;
        total += result.statistics;
    };

    std::size_t index{};
    while (index < suites.size()) {
        if (suites[index]->getOptions().execution() == Execution::Serial) {
            consume(executeSuite(suites[index++]));
            continue;
        }

        const auto concurrentEnd = std::ranges::find_if(
            suites.begin() + static_cast<std::ptrdiff_t>(index), suites.end(),
            [](const auto& suite) {
                return suite->getOptions().execution() == Execution::Serial;
            }
        );
        const auto endIndex = static_cast<std::size_t>(concurrentEnd - suites.begin());
        while (index < endIndex) {
            const auto count = std::min(impl->maxParallelSuites, endIndex - index);
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

    chain.onFinalReport(
        static_cast<unsigned int>(suites.size()),
        total.getPassedTests(),
        total.getFailedTests(),
        total.getSkippedTests(),
        total.getErrors()
    );

    const auto handlerError = chain.errorMessage();
    if (!handlerError.empty()) {
        std::cerr << handlerError << '\n';
    }

    return total.getFailedTests() == 0 && total.getErrors() == 0
        && hooksSucceeded && handlerError.empty() ? 0 : 1;
}

std::unique_ptr<TestEventHandler> makeConsoleHandler() {
    return std::make_unique<detail::ConsoleTestEventHandler>();
}

std::unique_ptr<TestEventHandler> makeJUnitHandler() {
    return std::make_unique<detail::JUnitTestEventHandler>();
}

std::unique_ptr<TestEventHandler> makeJUnitHandler(std::filesystem::path outputPath) {
    return std::make_unique<detail::JUnitTestEventHandler>(std::move(outputPath));
}

int run(int argc, char** argv) {
    Runner runner;
    runner.addHandler(makeConsoleHandler());
    runner.addHandler(makeJUnitHandler());
    return runner.run(argc, argv);
}

}
