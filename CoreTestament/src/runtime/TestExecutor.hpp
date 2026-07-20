#pragma once

#include "Testament/TestEventHandler.hpp"
#include "HookExecutor.hpp"

#include <chrono>
#include <exception>

namespace Testament {

class LifecycleSuite;
namespace detail {

class TestInstance;

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
    explicit TestExecutor(HookExecutor& mHookExecutor) noexcept;

    [[nodiscard]] TestExecutionResult executeFixtureless(TestInstance& test) const;
    [[nodiscard]] TestExecutionResult executeWithLifecycle(
        TestInstance& test, LifecycleSuite* fixture,
        HookExecutor::Callback& beforeEach, HookExecutor::Callback& afterEach
    ) const;

private:
    HookExecutor& mHookExecutor;
};

}
}
