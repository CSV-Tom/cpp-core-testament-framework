#include "Testament/Runner.hpp"

#include "EventHandlers/ConsoleTestEventHandler.hpp"
#include "EventHandlers/JUnitTestEventHandler.hpp"
#include "EventHandlers/TestEventHandlerChain.hpp"
#include "Internal/InternalRegistry.hpp"
#include "Internal/utils/TestStatistics.hpp"

#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <string_view>

namespace Testament {

int Runner::run(int argc, char** argv) {
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

    ConsoleTestEventHandler consoleHandler;
    std::unique_ptr<JUnitTestEventHandler> junitHandler;

    TestEventHandlerChain chain;
    chain.add(&consoleHandler);
    if (junitOutput) {
        junitHandler = std::make_unique<JUnitTestEventHandler>(*junitOutput);
        chain.add(junitHandler.get());
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

    if (junitHandler && !junitHandler->writeSucceeded()) {
        std::cerr << junitHandler->errorMessage() << '\n';
        return 1;
    }

    return total.getFailedTests() == 0 && hooksSucceeded ? 0 : 1;
}

}
