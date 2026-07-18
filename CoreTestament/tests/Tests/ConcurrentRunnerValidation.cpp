#include "Testament/Testament.hpp"

#include <atomic>
#include <barrier>
#include <chrono>
#include <future>
#include <thread>

int main() {
    std::atomic<unsigned int> activeTests{};
    std::atomic<unsigned int> maximumActiveTests{};
    std::atomic<unsigned int> executions{};

    auto suite = Testament::Suite("concurrent runners", Testament::Test("serialized", [&] {
        const auto active = activeTests.fetch_add(1) + 1;
        auto maximum = maximumActiveTests.load();
        while (maximum < active
               && !maximumActiveTests.compare_exchange_weak(maximum, active)) {}
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
        ++executions;
        --activeTests;
    }));

    std::barrier start{2};
    const auto run = [&] {
        start.arrive_and_wait();
        Testament::Runner runner;
        return runner.run(0, nullptr);
    };

    auto first = std::async(std::launch::async, run);
    auto second = std::async(std::launch::async, run);

    return suite
        && first.get() == 0
        && second.get() == 0
        && executions == 2
        && maximumActiveTests == 1
        ? 0
        : 1;
}
