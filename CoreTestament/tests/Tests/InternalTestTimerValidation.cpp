#include "core/Internal/FunctionVariant.hpp"
#include "core/Internal/InternalTest.hpp"

#include <chrono>
#include <stdexcept>
#include <thread>

int main() {
    bool firstExecution = true;
    Testament::InternalTest test(
        "throwing test",
        Testament::FunctionVariant{std::function<void()>([&firstExecution] {
            if (firstExecution) {
                firstExecution = false;
                throw std::runtime_error("expected failure");
            }
        })}
    );

    const auto firstResult = test.execute();
    const auto durationAfterExecution = test.getExecutionTimer().getDuration();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    const auto durationAfterWaiting = test.getExecutionTimer().getDuration();
    const auto secondResult = test.execute();

    const bool firstRunFailed = std::holds_alternative<std::exception_ptr>(firstResult);
    const bool secondRunPassed = std::holds_alternative<std::monostate>(secondResult)
        && test.getStatus().isPassed() && !test.getException();
    const bool failedTimerStopped = durationAfterExecution == durationAfterWaiting;

    return firstRunFailed && secondRunPassed && failedTimerStopped ? 0 : 1;
}
