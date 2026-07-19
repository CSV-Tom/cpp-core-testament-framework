#include "Testament/Testament.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <stdexcept>
#include <thread>

namespace {

class Fixture final : public Testament::LifecycleSuite {};

void updateMaximum(std::atomic<unsigned int>& maximum, unsigned int value) {
    auto observed = maximum.load();
    while (observed < value && !maximum.compare_exchange_weak(observed, value)) {}
}

}

int main() {
    std::atomic<unsigned int> started{};
    std::atomic<unsigned int> active{};
    const auto parallelTest = [&] {
        ++active;
        ++started;
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{1};
        while (started.load() != 2) {
            if (std::chrono::steady_clock::now() >= deadline) {
                throw std::runtime_error("Tests did not execute concurrently");
            }
            std::this_thread::yield();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{30});
        --active;
    };

    auto plainSuite = Testament::Suite(
        "parallel tests",
        Testament::Test(
            "exclusive test",
            Testament::TestOptions{}
                .order(-10)
                .execution(Testament::Execution::Serial),
            [&active] {
                std::this_thread::sleep_for(std::chrono::milliseconds{10});
                if (active != 0) throw std::runtime_error("Serial test overlapped another test");
            }
        ),
        Testament::Test("alpha test", parallelTest),
        Testament::Test("beta test", parallelTest)
    );

    std::atomic<unsigned int> fixtureActive{};
    std::atomic<unsigned int> fixtureMaximum{};
    const auto fixtureTest = [&](Fixture&) {
        const auto current = ++fixtureActive;
        updateMaximum(fixtureMaximum, current);
        std::this_thread::sleep_for(std::chrono::milliseconds{20});
        --fixtureActive;
    };
    auto fixtureSuite = Testament::Suite<Fixture>(
        "serial fixture tests",
        Testament::Test("first fixture test", fixtureTest),
        Testament::Test("second fixture test", fixtureTest)
    );

    Testament::Runner runner;
    runner.maxParallelTests(2);

    bool rejectedZero = false;
    try {
        runner.maxParallelTests(0);
    } catch (const std::invalid_argument&) {
        rejectedZero = true;
    }

    return plainSuite && fixtureSuite
        && rejectedZero
        && runner.run(0, nullptr) == 0
        && fixtureMaximum == 1
        ? 0
        : 1;
}
