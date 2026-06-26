#include "Testament/Runner.hpp"

#include "Internal/InternalRegistry.hpp"
#include "EventHandlers/ConsoleTestEventHandler.hpp"
#include "Internal/utils/TestStatistics.hpp"

namespace Testament {

int Runner::run(int /*argc*/, char** /*argv*/) {
    ConsoleTestEventHandler handler;

    auto& registry = InternalRegistry::getInstance();
    auto suites = registry.getAllSuites();

    handler.onStartReport(static_cast<unsigned int>(suites.size()));

    TestStatistics<unsigned int> total;
    for (auto& suite : suites) {
        suite->setHandler(&handler);
        suite->run();
        total += suite->getStatistics();
    }

    handler.onFinalReport(
        static_cast<unsigned int>(suites.size()),
        total.getPassedTests(),
        total.getFailedTests(),
        total.getSkippedTests()
    );

    return total.getFailedTests() == 0 ? 0 : 1;
}

}
