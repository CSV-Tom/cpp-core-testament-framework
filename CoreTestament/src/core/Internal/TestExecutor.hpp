#ifndef TESTAMENT_INTERNAL_TESTEXECUTOR_HPP
#define TESTAMENT_INTERNAL_TESTEXECUTOR_HPP

#include "Testament/TestEventHandler.hpp"
#include "utils/HookManager.hpp"

#include <chrono>
#include <exception>

namespace Testament {

class InternalTest;
class LifecycleSuite;
class TestManager;

namespace detail {

struct TestExecutionResult {
    TestEventHandler::TestResultStatus status;
    std::chrono::duration<double> duration{};
    std::exception_ptr exception;

    [[nodiscard]] bool lifecycleSucceeded() const noexcept {
        return status != TestEventHandler::TestResultStatus::LifecycleError;
    }
};

class TestExecutor {
public:
    TestExecutor(TestManager& testManager, HookManager& hookManager) noexcept;

    [[nodiscard]] TestExecutionResult executeFixtureless(InternalTest& test) const;
    [[nodiscard]] TestExecutionResult executeWithLifecycle(
        InternalTest& test, LifecycleSuite* fixture,
        HookManager::Callback& beforeEach, HookManager::Callback& afterEach
    ) const;

private:
    TestManager& testManager;
    HookManager& hookManager;
};

}
}

#endif
