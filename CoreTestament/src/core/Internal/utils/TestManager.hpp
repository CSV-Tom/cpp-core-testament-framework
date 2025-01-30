#ifndef TESTFRAMEWORK_TESTAMENT_TESTMANAGER_HPP
#define TESTFRAMEWORK_TESTAMENT_TESTMANAGER_HPP

#include "ExecutionTimer.hpp"
#include "TestStatistics.hpp"
#include <vector>
#include <memory>
#include <variant>
#include <exception>

#include "Testament/Suite.hpp"

#include "../InternalTest.hpp"

namespace Testament {


class TestManager {
public:

    TestManager(ExecutionTimer& timer) : testTimer(timer) {}

    void executeTest(Suite& suite, std::shared_ptr<InternalTest>& test) {
        testTimer.start();
        auto result = test->execute(&suite);
        testTimer.stop();

        processResult(test, result);
    }

private:
    ExecutionTimer& testTimer;
    TestStatistics<unsigned int> statistic;

    void processResult(std::shared_ptr<InternalTest>& test, const std::variant<std::monostate, std::exception_ptr>& result) {
        
        if (std::holds_alternative<std::monostate>(result)) {
            if (test->getStatus().isPassed()) {
                statistic.incrementPassedTests();
            } else if (test->getStatus().isSkipped()) {
                statistic.incrementSkippedTests();
            }
        } else if (std::holds_alternative<std::exception_ptr>(result)) {
            std::exception_ptr exception = std::get<std::exception_ptr>(result);
            try {
                std::rethrow_exception(exception);
            } catch (...) {
                statistic.incrementFailedTests();
            }
        }
    }

};

}

#endif
