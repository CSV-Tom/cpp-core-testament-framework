#include "Testament/Testament.hpp"

#include <atomic>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace {

class RecordingHandler final : public Testament::TestEventHandler {
public:
    void onSuiteStart(const SuiteInfo& suite) override {
        suites.push_back(suite.name);
    }

    std::vector<std::string> suites;
};

}

int main() {
    std::atomic<unsigned int> started{};
    std::atomic<unsigned int> active{};
    const auto runConcurrently = [&started, &active] {
        ++active;
        ++started;
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{1};
        while (started.load() != 2) {
            if (std::chrono::steady_clock::now() >= deadline) {
                throw std::runtime_error("Suites did not execute concurrently");
            }
            std::this_thread::yield();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{50});
        --active;
    };

    auto beta = Testament::Suite("beta suite", Testament::Test("beta test", runConcurrently));
    auto alpha = Testament::Suite("alpha suite", Testament::Test("alpha test", runConcurrently));
    auto exclusive = Testament::Suite(
        "exclusive suite",
        Testament::SuiteOptions{}
            .order(-10)
            .execution(Testament::Execution::Serial),
        Testament::Test("exclusive test", [&active] {
            std::this_thread::sleep_for(std::chrono::milliseconds{20});
            if (active != 0) throw std::runtime_error("Serial suite overlapped another suite");
        })
    );
    auto handler = std::make_unique<RecordingHandler>();
    auto* recording = handler.get();

    Testament::Runner runner;
    runner.addHandler(std::move(handler)).maxParallelSuites(2);

    bool rejectedZero = false;
    try {
        runner.maxParallelSuites(0);
    } catch (const std::invalid_argument&) {
        rejectedZero = true;
    }

    return alpha && beta && exclusive
        && rejectedZero
        && runner.run(0, nullptr) == 0
        && recording->suites == std::vector<std::string>{
            "exclusive suite", "alpha suite", "beta suite"
        }
        ? 0
        : 1;
}
