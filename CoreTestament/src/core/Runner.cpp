#include "Testament/Runner.hpp"

#include "Testament/Reporters.hpp"

#include "EventHandlers/ConsoleTestEventHandler.hpp"
#include "EventHandlers/JUnitTestEventHandler.hpp"
#include "EventHandlers/TestEventHandlerChain.hpp"
#include "Internal/InternalRegistry.hpp"
#include "Internal/utils/TestStatistics.hpp"

#include <filesystem>
#include <iostream>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

namespace Testament {

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

int Runner::run(int, char**) {
    TestEventHandlerChain chain;
    for (const auto& handler : impl->handlers) {
        chain.add(handler.get());
    }

    auto& registry = InternalRegistry::getInstance();
    auto suites = registry.getAllSuites();

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
    std::optional<std::filesystem::path> junitOutput;
    for (int index = 1; index < argc; ++index) {
        const std::string_view argument{argv[index]};
        if (argument == "--junit") {
            if (++index >= argc || std::string_view{argv[index]}.empty()) {
                std::cerr << "--junit requires an output path\n";
                return 2;
            }
            junitOutput = argv[index];
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
