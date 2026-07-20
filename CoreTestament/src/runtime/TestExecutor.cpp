#include "TestExecutor.hpp"

#include "TestInstance.hpp"
#include <stdexcept>
#include <string>

namespace Testament::detail {

namespace {

bool isTerminal(TestEventHandler::TestResultStatus status) noexcept {
    return status == TestEventHandler::TestResultStatus::Passed
        || status == TestEventHandler::TestResultStatus::Skipped;
}

}

TestExecutor::TestExecutor(HookExecutor& hooks) noexcept : mHookExecutor(hooks) {}

TestExecutionResult TestExecutor::executeFixtureless(TestInstance& test) const {
    if (test.options().isDisabled()) {
        return {TestEventHandler::TestResultStatus::Skipped, {}, {}};
    }

    auto remainingAttempts = test.options().maxAttempts();
    TestExecutionResult execution{TestEventHandler::TestResultStatus::Failed, {}, {}};
    while (true) {
        const auto result = test.execute();
        execution.duration += test.executionTimer().duration();
        if (test.status().isSkipped()) {
            execution.status = TestEventHandler::TestResultStatus::Skipped;
            execution.exception = test.exception();
        } else if (result) {
            execution.status = TestEventHandler::TestResultStatus::Passed;
            execution.exception = {};
        } else {
            execution.status = TestEventHandler::TestResultStatus::Failed;
            execution.exception = result.error();
        }
        if (isTerminal(execution.status) || remainingAttempts <= 1) return execution;
        --remainingAttempts;
    }
}

TestExecutionResult TestExecutor::executeWithLifecycle(
    TestInstance& test, LifecycleSuite* fixture,
    HookExecutor::Callback& beforeEach, HookExecutor::Callback& afterEach
) const {
    if (test.options().isDisabled()) {
        return {TestEventHandler::TestResultStatus::Skipped, {}, {}};
    }

    auto remainingAttempts = test.options().maxAttempts();
    TestExecutionResult execution{TestEventHandler::TestResultStatus::Failed, {}, {}};
    while (true) {
        std::string lifecycleError;
        const bool beforeEachSucceeded = mHookExecutor.invokeBeforeEachHook()
            && mHookExecutor.invoke(beforeEach, "beforeEach");
        if (!beforeEachSucceeded) lifecycleError = mHookExecutor.errors().back();

        TestInstance::Result result{};
        if (beforeEachSucceeded) {
            result = test.execute(fixture);
            execution.duration += test.executionTimer().duration();
        }
        if (!mHookExecutor.invoke(afterEach, "afterEach")
            || !mHookExecutor.invokeAfterEachHook()) {
            if (!lifecycleError.empty()) lifecycleError += "; ";
            lifecycleError += mHookExecutor.errors().back();
        }

        if (!lifecycleError.empty()) {
            execution.status = TestEventHandler::TestResultStatus::LifecycleError;
            execution.exception = std::make_exception_ptr(std::runtime_error(lifecycleError));
        } else if (test.status().isSkipped()) {
            execution.status = TestEventHandler::TestResultStatus::Skipped;
            execution.exception = test.exception();
        } else if (!result) {
            execution.status = TestEventHandler::TestResultStatus::Failed;
            execution.exception = result.error();
        } else {
            execution.status = TestEventHandler::TestResultStatus::Passed;
            execution.exception = {};
        }

        if (isTerminal(execution.status) || remainingAttempts <= 1) return execution;
        --remainingAttempts;
    }
}

}
