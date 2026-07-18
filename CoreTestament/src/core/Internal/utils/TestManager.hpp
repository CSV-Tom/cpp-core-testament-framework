#ifndef TESTFRAMEWORK_TESTAMENT_TESTMANAGER_HPP
#define TESTFRAMEWORK_TESTAMENT_TESTMANAGER_HPP

#include "TestStatistics.hpp"
#include <chrono>
#include <expected>
#include <memory>
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
                     const std::unique_ptr<InternalTest>& test,
                     TestEventHandler* handler) const {
        if (handler) {
            handler->onTestStart(suiteInfo, {
                test->getName(), std::chrono::duration<double>::zero(), {}, test->getOptions(),
                TestEventHandler::TestResultStatus::NotRun
            });
        }
    }

    Result executeAttempt(LifecycleSuite* fixture, std::unique_ptr<InternalTest>& test) {
        return test->execute(fixture);
    }

    void reportResult(TestEventHandler::SuiteInfo suiteInfo,
                      const std::unique_ptr<InternalTest>& test,
                      TestEventHandler::TestResultStatus status,
                      std::chrono::duration<double> duration,
                      std::exception_ptr exception,
                      TestEventHandler* handler) {
        TestEventHandler::TestInfo testInfo{
            test->getName(), duration, std::move(exception), test->getOptions(), status
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
        case TestEventHandler::TestResultStatus::LifecycleError:
            statistic.incrementFailedTests();
            suiteInfo.failed = statistic.getFailedTests();
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
