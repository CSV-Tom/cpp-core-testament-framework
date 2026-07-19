#include "Testament/Runner.hpp"

#include "Testament/Reporters.hpp"
#include "Testament/TestEventHandler.hpp"

#include "EventHandlers/ConsoleTestEventHandler.hpp"
#include "EventHandlers/BufferedTestEventHandler.hpp"
#include "EventHandlers/JUnitTestEventHandler.hpp"
#include "EventHandlers/TestEventHandlerChain.hpp"
#include "Internal/InternalRegistry.hpp"
#include "Internal/InternalSuite.hpp"
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

int Runner::run(int argc, char** argv) {
    const auto arguments = commandLineArguments(argc, argv);
    if (!arguments) {
        std::cerr << "Invalid command-line arguments\n";
        return 2;
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
            return suite->getName() != *impl->suiteFilter;
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
    const auto executeSuite = [&testFilter](const auto& suite) {
        SuiteResult result;
        result.hooksSucceeded = suite->run(&result.events, testFilter);
        result.statistics = suite->getStatistics();
        return result;
    };

    std::vector<std::future<SuiteResult>> running;
    running.reserve(suites.size());
    const auto parallelism = std::min(impl->maxParallelSuites, suites.size());
    for (std::size_t index = 0; index < parallelism; ++index) {
        running.push_back(std::async(std::launch::async, executeSuite, suites[index]));
    }

    TestStatistics<unsigned int> total;
    bool hooksSucceeded = true;
    for (std::size_t index = 0; index < suites.size(); ++index) {
        auto result = running[index].get();
        result.events.replay(chain);
        hooksSucceeded = result.hooksSucceeded && hooksSucceeded;
        total += result.statistics;

        const auto next = index + parallelism;
        if (next < suites.size()) {
            running.push_back(std::async(std::launch::async, executeSuite, suites[next]));
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
