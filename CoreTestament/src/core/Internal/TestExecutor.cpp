#include "TestExecutor.hpp"

#include "InternalTest.hpp"
#include "utils/TestManager.hpp"

#include <stdexcept>
#include <string>

namespace Testament::detail {

namespace {

bool isTerminal(TestEventHandler::TestResultStatus status) noexcept {
    return status == TestEventHandler::TestResultStatus::Passed
        || status == TestEventHandler::TestResultStatus::Skipped;
}

}

TestExecutor::TestExecutor(TestManager& manager, HookManager& hooks) noexcept
    : testManager(manager), hookManager(hooks) {}

TestExecutionResult TestExecutor::executeFixtureless(InternalTest& test) const {
    if (test.options().isDisabled()) {
        return {TestEventHandler::TestResultStatus::Skipped, {}, {}};
    }

    auto remainingAttempts = test.options().maxAttempts();
    TestExecutionResult execution{TestEventHandler::TestResultStatus::Failed, {}, {}};
    while (true) {
        const auto result = testManager.executeAttempt(nullptr, test);
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
    InternalTest& test, LifecycleSuite* fixture,
    HookManager::Callback& beforeEach, HookManager::Callback& afterEach
) const {
    if (test.options().isDisabled()) {
        return {TestEventHandler::TestResultStatus::Skipped, {}, {}};
    }

    auto remainingAttempts = test.options().maxAttempts();
    TestExecutionResult execution{TestEventHandler::TestResultStatus::Failed, {}, {}};
    while (true) {
        std::string lifecycleError;
        const bool beforeEachSucceeded = hookManager.invokeBeforeEachHook()
            && hookManager.invoke(beforeEach, "beforeEach");
        if (!beforeEachSucceeded) lifecycleError = hookManager.errors().back();

        TestManager::Result result{};
        if (beforeEachSucceeded) {
            result = testManager.executeAttempt(fixture, test);
            execution.duration += test.executionTimer().duration();
        }
        if (!hookManager.invoke(afterEach, "afterEach")
            || !hookManager.invokeAfterEachHook()) {
            if (!lifecycleError.empty()) lifecycleError += "; ";
            lifecycleError += hookManager.errors().back();
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
