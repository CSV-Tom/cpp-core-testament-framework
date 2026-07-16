#include "Testament/FunctionVariant.hpp"

#include "core/Internal/InternalTest.hpp"

#include <chrono>
#include <stdexcept>
#include <thread>

int main() {
    Testament::InternalTest test(
        "throwing test",
        Testament::FunctionVariant{std::function<void()>([] {
            throw std::runtime_error("expected failure");
        })}
    );

    test.execute();
    const auto durationAfterExecution = test.getExecutionTimer().getDuration();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    const auto durationAfterWaiting = test.getExecutionTimer().getDuration();

    return durationAfterExecution == durationAfterWaiting ? 0 : 1;
}
