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
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

namespace Testament {

namespace {

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

    bool handlersConfigured = true;
    for (const auto& handler : impl->handlers) {
        if (const auto result = handler->configure(*arguments); !result) {
            std::cerr << result.error() << '\n';
            handlersConfigured = false;
        }
    }
    if (!handlersConfigured) {
        return 2;
    }

    TestEventHandlerChain chain;
    for (const auto& handler : impl->handlers) {
        chain.add(handler.get());
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

    bool handlersSucceeded = true;
    for (const auto& handler : impl->handlers) {
        if (const auto error = handler->errorMessage(); !error.empty()) {
            std::cerr << error << '\n';
            handlersSucceeded = false;
        }
    }

    return total.getFailedTests() == 0 && hooksSucceeded && handlersSucceeded ? 0 : 1;
}

std::unique_ptr<TestEventHandler> makeConsoleHandler() {
    return std::make_unique<ConsoleTestEventHandler>();
}

std::unique_ptr<TestEventHandler> makeJUnitHandler(std::filesystem::path outputPath) {
    return std::make_unique<JUnitTestEventHandler>(std::move(outputPath));
}

int run(int argc, char** argv) {
    const auto arguments = commandLineArguments(argc, argv);
    if (!arguments) {
        std::cerr << "Invalid command-line arguments\n";
        return 2;
    }

    std::optional<std::filesystem::path> junitOutput;
    for (std::size_t index = 0; index < arguments->size(); ++index) {
        const auto argument = (*arguments)[index];
        if (argument == "--junit") {
            if (++index >= arguments->size() || (*arguments)[index].empty()) {
                std::cerr << "--junit requires an output path\n";
                return 2;
            }
            junitOutput = (*arguments)[index];
        } else if (argument.starts_with("--junit=")) {
            const auto path = argument.substr(std::string_view{"--junit="}.size());
            if (path.empty()) {
                std::cerr << "--junit requires an output path\n";
                return 2;
            }
            junitOutput = path;
        }
    }

    Runner runner;
    runner.addHandler(makeConsoleHandler());
    if (junitOutput) {
        runner.addHandler(makeJUnitHandler(*junitOutput));
    }
    return runner.run(argc, argv);
}

}
