#include "runtime/TestFunction.hpp"
#include "runtime/TestInstance.hpp"

#include <chrono>
#include <stdexcept>
#include <thread>

int main() {
    bool firstExecution = true;
    Testament::detail::TestInstance test(
        "throwing test",
        Testament::detail::TestFunction{std::move_only_function<void()>([&firstExecution] {
            if (firstExecution) {
                firstExecution = false;
                throw std::runtime_error("expected failure");
            }
        })}
    );

    const auto firstResult = test.execute();
    const auto durationAfterExecution = test.executionTimer().duration();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    const auto durationAfterWaiting = test.executionTimer().duration();
    const auto secondResult = test.execute();

    const bool firstRunFailed = !firstResult;
    const bool secondRunPassed = secondResult
        && test.status().isPassed() && !test.exception();
    const bool failedTimerStopped = durationAfterExecution == durationAfterWaiting;

    return firstRunFailed && secondRunPassed && failedTimerStopped ? 0 : 1;
}
