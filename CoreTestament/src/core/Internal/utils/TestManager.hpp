#ifndef TESTFRAMEWORK_TESTAMENT_TESTMANAGER_HPP
#define TESTFRAMEWORK_TESTAMENT_TESTMANAGER_HPP

#include "TestStatistics.hpp"
#include <chrono>
#include <expected>
#include <utility>
#include <exception>

#include "../InternalTest.hpp"
#include "Testament/TestEventHandler.hpp"

namespace Testament {

class LifecycleSuite;

class TestManager {
public:
    using Result = std::expected<void, std::exception_ptr>;

    explicit TestManager(TestStatistics<unsigned int>& statistic_) : statistic(statistic_) {}

    void reportStart(const TestEventHandler::SuiteInfo& suiteInfo,
                     const InternalTest& test,
                     TestEventHandler* handler) const {
        if (handler) {
            handler->onTestStart(suiteInfo, {
                test.getName(), test.getLocation(), std::chrono::duration<double>::zero(),
                {}, test.getOptions(),
                TestEventHandler::TestResultStatus::NotRun
            });
        }
    }

    Result executeAttempt(LifecycleSuite* fixture, InternalTest& test) {
        return test.execute(fixture);
    }

    void reportResult(TestEventHandler::SuiteInfo suiteInfo,
                      const InternalTest& test,
                      TestEventHandler::TestResultStatus status,
                      std::chrono::duration<double> duration,
                      std::exception_ptr exception,
                      TestEventHandler* handler) {
        TestEventHandler::TestInfo testInfo{
            test.getName(), test.getLocation(), duration, std::move(exception),
            test.getOptions(), status
        };

        switch (status) {
        case TestEventHandler::TestResultStatus::Passed:
            statistic.incrementPassedTests();
            suiteInfo.passed = statistic.getPassedTests();
            if (handler) handler->onTestPassed(suiteInfo, testInfo);
            break;
        case TestEventHandler::TestResultStatus::Skipped:
            statistic.incrementSkippedTests();
            suiteInfo.skipped = statistic.getSkippedTests();
            if (handler) handler->onTestSkipped(suiteInfo, testInfo);
            break;
        case TestEventHandler::TestResultStatus::Failed:
            statistic.incrementFailedTests();
            suiteInfo.failed = statistic.getFailedTests();
            if (handler) handler->onTestFailed(suiteInfo, testInfo);
            break;
        case TestEventHandler::TestResultStatus::LifecycleError:
            statistic.incrementErrors();
            suiteInfo.errors = statistic.getErrors();
            if (handler) handler->onTestFailed(suiteInfo, testInfo);
            break;
        case TestEventHandler::TestResultStatus::NotRun:
            break;
        }
    }

private:
    TestStatistics<unsigned int>& statistic;
};

}

#endif
