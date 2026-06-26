#include "CoreTestamentFramework/TestRegistry.hpp"

#include <memory>
#include <vector>

#include "TestSuite.hpp"
#include "TestStatistics.hpp"
#include "TestEventHandler.hpp"

class TestRegistry::Impl {
public:
    void addSuite(const std::shared_ptr<TestSuite>& suite) {
        suites.push_back(suite);
    }

    void addGlobalEventHandler(const std::shared_ptr<TestEventHandler>& handler) {
        globalEventHandlers.push_back(handler);
    }

    int TestRegistry::run(int argc, char** argv) {
        addGlobalEventHandler(std::make_shared<ConsoleTestEventHandler>());

        dispatcher.registerHandler<LogEvent>([](const LogEvent& event) {
            std::cout << "Log message: " << event.message << std::endl;
        });

        statistics.reset();

        for (const auto& handler : globalEventHandlers) {
            handler->onStartReport(suites);
        }

        for (const auto& suite : suites) {
            try {
                for (const auto& handler : globalEventHandlers) {
                    suite->addTestEventHandler(handler);
                }
                for (const auto& handler : globalEventHandlers) {
                    handler->onSuiteStart(*suite);
                }

                suite->run();

                statistics += suite->getTestStatistics();

                for (const auto& handler : globalEventHandlers) {
                    handler->onSuiteEnd(*suite);
                }
            } catch (const std::exception& e) {
                statistics.incrementFailedTests();
                for (const auto& handler : globalEventHandlers) {
                    handler->onSuiteAbort(e.what());
                }
            } catch (...) {
                statistics.incrementFailedTests();
                for (const auto& handler : globalEventHandlers) {
                    handler->onSuiteAbort("Unknown exception occurred.");
                }
            }
        }

        for (const auto& handler : globalEventHandlers) {
            handler->onFinalReport(suites);
        }


        dispatcher.dispatch(LogEvent("All systems operational")); // TODO: Besser als EventHandler

        return statistics.getFailedTests() == 0 ? 0 : 1;
    }
private:
    std::vector<std::shared_ptr<TestSuite>> suites;
    std::vector<std::shared_ptr<TestEventHandler>> globalEventHandlers;
    TestStatistics<unsigned int> statistics;
    //EventDispatcher dispatcher;
    FilterEventDispatcher dispatcher;
};


TestRegistry::TestRegistry() : pImpl(std::make_unique<Impl>()) {}
TestRegistry::~TestRegistry() = default;

TestRegistry::TestRegistry& instance() {
    static TestRegistry registry;
    return registry;
}

void TestRegistry::addSuite(const std::shared_ptr<TestSuite>& suite) {
    pImpl->addSuite(suite);
}

void TestRegistry::addGlobalEventHandler(const std::shared_ptr<TestEventHandler>& handler) {
    pImpl->addGlobalEventHandler(handler);
}

void TestRegistry::run(int argc, char** argv) {
    pImpl->run(argc, argv);
}