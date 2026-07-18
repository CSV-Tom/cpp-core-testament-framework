#include "Testament/Runner.hpp"

#include "Testament/Reporters.hpp"
#include "Testament/TestEventHandler.hpp"

#include "EventHandlers/ConsoleTestEventHandler.hpp"
#include "EventHandlers/JUnitTestEventHandler.hpp"
#include "EventHandlers/TestEventHandlerChain.hpp"
#include "Internal/InternalRegistry.hpp"
#include "Internal/InternalSuite.hpp"
#include "Internal/utils/TestStatistics.hpp"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <optional>
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
    std::ranges::sort(suites, [](const auto& left, const auto& right) {
        const auto leftOrder = left->getOptions().order().value_or(0);
        const auto rightOrder = right->getOptions().order().value_or(0);
        if (leftOrder != rightOrder) {
            return leftOrder < rightOrder;
        }
        return left->getName() < right->getName();
    });

    chain.onStartReport(static_cast<unsigned int>(suites.size()));

    TestStatistics<unsigned int> total;
    bool hooksSucceeded = true;
    for (auto& suite : suites) {
        suite->setHandler(&chain);
        hooksSucceeded = suite->run() && hooksSucceeded;
        total += suite->getStatistics();
    }

    chain.onFinalReport(
        static_cast<unsigned int>(suites.size()),
        total.getPassedTests(),
        total.getFailedTests(),
        total.getSkippedTests()
    );

    const auto handlerError = chain.errorMessage();
    if (!handlerError.empty()) {
        std::cerr << handlerError << '\n';
    }

    return total.getFailedTests() == 0 && hooksSucceeded && handlerError.empty() ? 0 : 1;
}

std::unique_ptr<TestEventHandler> makeConsoleHandler() {
    return std::make_unique<detail::ConsoleTestEventHandler>();
}

std::unique_ptr<TestEventHandler> makeJUnitHandler(std::filesystem::path outputPath) {
    return std::make_unique<detail::JUnitTestEventHandler>(std::move(outputPath));
}

int run(int argc, char** argv) {
    Runner runner;
    runner.addHandler(makeConsoleHandler());
    runner.addHandler(std::make_unique<detail::JUnitTestEventHandler>());
    return runner.run(argc, argv);
}

}
