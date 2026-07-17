#ifndef TESTFRAMEWORK_TESTAMENT_TESTMANAGER_HPP
#define TESTFRAMEWORK_TESTAMENT_TESTMANAGER_HPP

#include "ExecutionTimer.hpp"
#include "TestStatistics.hpp"
#include <chrono>
#include <memory>
#include <variant>
#include <exception>

#include "../InternalTest.hpp"
#include "Testament/TestEventHandler.hpp"

namespace Testament {

class LifecycleSuite;

class TestManager {
public:
    TestManager(ExecutionTimer& timer, TestStatistics<unsigned int>& statistic_)
        : testTimer(timer), statistic(statistic_) {}

    void executeTest(LifecycleSuite* fixture, std::unique_ptr<InternalTest>& test,
                     const TestEventHandler::SuiteInfo& suiteInfo,
                     TestEventHandler* handler = nullptr) {
        if (test->getOptions().isDisabled()) {
            const auto result = test->execute(fixture);
            processResult(suiteInfo, test, result, std::chrono::duration<double>::zero(), handler);
            return;
        }

        if (handler) {
            handler->onTestStart(suiteInfo, {
                test->getName(), std::chrono::duration<double>::zero(), {}, test->getOptions()
            });
        }
        testTimer.start();
        auto result = test->execute(fixture);
        auto duration = test->getExecutionTimer().getDuration();
        auto remainingRetries = test->getOptions().retryCount();
        while (std::holds_alternative<std::exception_ptr>(result) && remainingRetries > 0) {
            --remainingRetries;
            result = test->execute(fixture);
            duration += test->getExecutionTimer().getDuration();
        }
        testTimer.stop();
        processResult(suiteInfo, test, result, duration, handler);
    }

private:
    ExecutionTimer& testTimer;
    TestStatistics<unsigned int>& statistic;

    void processResult(const TestEventHandler::SuiteInfo& suiteInfo,
                       std::unique_ptr<InternalTest>& test,
                       const std::variant<std::monostate, std::exception_ptr>& result,
                       std::chrono::duration<double> duration,
                       TestEventHandler* handler) {
        TestEventHandler::TestInfo testInfo{
            test->getName(), duration, {}, test->getOptions()
        };

        if (std::holds_alternative<std::monostate>(result)) {
            if (test->getStatus().isPassed()) {
                statistic.incrementPassedTests();
                if (handler) handler->onTestPassed(suiteInfo, testInfo);
            } else if (test->getStatus().isSkipped()) {
                statistic.incrementSkippedTests();
                if (handler) handler->onTestSkipped(suiteInfo, testInfo);
            }
        } else {
            testInfo.exception = std::get<std::exception_ptr>(result);
            statistic.incrementFailedTests();
            if (handler) handler->onTestFailed(suiteInfo, testInfo);
        }
    }
};

}

#endif
