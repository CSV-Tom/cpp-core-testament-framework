#include "Testament/Runner.hpp"

#include "Internal/InternalRegistry.hpp"
#include "EventHandlers/ConsoleTestEventHandler.hpp"
#include "EventHandlers/TestEventHandlerChain.hpp"
#include "Internal/utils/TestStatistics.hpp"

namespace Testament {

int Runner::run(int /*argc*/, char** /*argv*/) {
    ConsoleTestEventHandler consoleHandler;

    TestEventHandlerChain chain;
    chain.add(&consoleHandler);

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

    return total.getFailedTests() == 0 && hooksSucceeded ? 0 : 1;
}

}
