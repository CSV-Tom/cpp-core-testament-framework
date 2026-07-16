#ifndef TESTFRAMEWORK_TESTAMENT_TESTMANAGER_HPP
#define TESTFRAMEWORK_TESTAMENT_TESTMANAGER_HPP

#include "ExecutionTimer.hpp"
#include "TestStatistics.hpp"
#include <memory>
#include <variant>
#include <exception>

#include "Testament/Suite.hpp"
#include "../InternalTest.hpp"
#include "Testament/TestEventHandler.hpp"

namespace Testament {

class TestManager {
public:
    TestManager(ExecutionTimer& timer, TestStatistics<unsigned int>& statistic_)
        : testTimer(timer), statistic(statistic_) {}

    void executeTest(Suite& suite, std::shared_ptr<InternalTest>& test,
                     const std::string& suiteName, TestEventHandler* handler = nullptr) {
        testTimer.start();
        auto result = test->execute(&suite);
        testTimer.stop();
        processResult(suiteName, test, result, handler);
    }

private:
    ExecutionTimer& testTimer;
    TestStatistics<unsigned int>& statistic;

    void processResult(const std::string& suiteName, std::shared_ptr<InternalTest>& test,
                       const std::variant<std::monostate, std::exception_ptr>& result,
                       TestEventHandler* handler) {
        TestEventHandler::SuiteInfo suiteInfo{suiteName};
        TestEventHandler::TestInfo testInfo{test->getName(), test->getExecutionTimer().getDuration(), {}};

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
