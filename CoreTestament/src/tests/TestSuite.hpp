#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <chrono>
#include <variant>
#include <optional>
#include <exception>


class TestSuite {
public:
    explicit TestSuite(const std::string& name) : name(name) {
        if (name.empty()) {
            throw std::logic_error("TestSuite name cannot be empty!");
        }
    }

    virtual ~TestSuite() =default;

    void addTest(const std::shared_ptr<TestCase>& testCase) {
        if (!testCase) {
            throw std::logic_error("Attempted to add an invalid TestCase to the TestSuite");
        }
        testCases.push_back(testCase);
    }

    void run() {
        executionTimer.start();
        //TODO: Zeitmessung, TODO: Layout Log Ausgabe Verbessern, Event Hander Junit
        beforeAll();

        for (auto& testCase : testCases) {
            beforeEach();
            auto result = testCase->execute(this);

            if (std::holds_alternative<std::monostate>(result)) {
                if (testCase->getStatus().isPassed()) {
                    statistic.incrementPassedTests();
                    afterEach();
                    executionTimer.stop();
                    for (const auto& handler : eventHandlers) {
                        handler->onTestPassed(*this, *testCase);
                    }
                } else if (testCase->getStatus().isSkipped()) {
                    statistic.incrementSkippedTests();
                    afterEach();
                    executionTimer.stop();
                    for (const auto& handler : eventHandlers) {
                        handler->onTestSkipped(*this, *testCase);
                    }
                }
            } else if (std::holds_alternative<std::exception_ptr>(result)) {
                std::exception_ptr exception = std::get<std::exception_ptr>(result);
                try {
                    std::rethrow_exception(exception);
                } catch (...) {
                    statistic.incrementFailedTests();
                    afterEach();
                    executionTimer.stop();
                    for (const auto& handler : eventHandlers) {
                        handler->onTestFailed(*this, *testCase, exception);
                    }
                }
            }
        }
        executionTimer.getDuration();
        afterAll();

        for (const auto& handler : eventHandlers) {
            handler->onSuiteSummary(*this, testCases);
        }
    }

    void addTestEventHandler(const std::shared_ptr<TestEventHandler>& eventHandler) {
        eventHandlers.push_back(eventHandler);
    }

    virtual void beforeAll() {}
    virtual void beforeEach() {}
    virtual void afterEach() {}
    virtual void afterAll() {}

    const std::string& getName() const {
        return name;
    }

    const TestStatistics<unsigned int>& getTestStatistics() const {
        return statistic;
    }

    const ExecutionTimer& getExecutionTimer() const {
        return executionTimer;
    }

private:
    std::string name;
    TestStatistics<unsigned int> statistic;
    ExecutionTimer executionTimer;
    std::vector<std::shared_ptr<TestCase>> testCases;
    std::vector<std::shared_ptr<TestEventHandler>> eventHandlers;
};