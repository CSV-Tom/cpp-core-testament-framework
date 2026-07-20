#pragma once

#include "TestCounts.hpp"
#include <chrono>
#include <utility>
#include <exception>

#include "TestInstance.hpp"
#include "Testament/TestEventHandler.hpp"

namespace Testament::detail {

class TestResultRecorder {
public:
    explicit TestResultRecorder(TestCounts<unsigned int>& testStatistics)
        : mCounts(testStatistics) {}

    void reportStart(const TestEventHandler::SuiteInfo& suiteInfo,
                     const TestInstance& test,
                     TestEventHandler* handler) const {
        if (handler) {
            handler->onTestStart(suiteInfo, {
                test.name(), test.location(), std::chrono::duration<double>::zero(),
                {}, test.options(),
                TestEventHandler::TestResultStatus::NotRun
            });
        }
    }

    void reportResult(TestEventHandler::SuiteInfo suiteInfo,
                      const TestInstance& test,
                      TestEventHandler::TestResultStatus status,
                      std::chrono::duration<double> duration,
                      std::exception_ptr exception,
                      TestEventHandler* handler) {
        TestEventHandler::TestInfo testInfo{
            test.name(), test.location(), duration, std::move(exception),
            test.options(), status
        };

        switch (status) {
        case TestEventHandler::TestResultStatus::Passed:
            mCounts.incrementPassedTests();
            suiteInfo.passed = mCounts.passedTests();
            if (handler) handler->onTestPassed(suiteInfo, testInfo);
            break;
        case TestEventHandler::TestResultStatus::Skipped:
            mCounts.incrementSkippedTests();
            suiteInfo.skipped = mCounts.skippedTests();
            if (handler) handler->onTestSkipped(suiteInfo, testInfo);
            break;
        case TestEventHandler::TestResultStatus::Failed:
            mCounts.incrementFailedTests();
            suiteInfo.failed = mCounts.failedTests();
            if (handler) handler->onTestFailed(suiteInfo, testInfo);
            break;
        case TestEventHandler::TestResultStatus::LifecycleError:
            mCounts.incrementErrors();
            suiteInfo.errors = mCounts.errors();
            if (handler) handler->onTestFailed(suiteInfo, testInfo);
            break;
        case TestEventHandler::TestResultStatus::NotRun:
            break;
        }
    }

private:
    TestCounts<unsigned int>& mCounts;
};

}
