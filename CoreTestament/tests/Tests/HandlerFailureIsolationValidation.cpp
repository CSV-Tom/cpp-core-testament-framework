#include "Testament/Testament.hpp"

#include <memory>
#include <stdexcept>
#include <string>

namespace {

class ThrowingEventHandler final : public Testament::TestEventHandler {
public:
    void onTestPassed(const SuiteInfo&, const TestInfo&) override {
        throw std::runtime_error("event callback failure");
    }

    std::string errorMessage() const override {
        throw 42;
    }
};

class ThrowingConfigurationHandler final : public Testament::TestEventHandler {
public:
    std::expected<void, std::string> configure(Arguments) override {
        throw 42;
    }
};

class RecordingHandler final : public Testament::TestEventHandler {
public:
    std::expected<void, std::string> configure(Arguments) override {
        configured = true;
        return {};
    }

    void onTestPassed(const SuiteInfo&, const TestInfo&) override {
        testPassed = true;
    }

    void onFinalReport(const Testament::RunSummary&) override {
        finalReportReceived = true;
    }

    bool configured{};
    bool testPassed{};
    bool finalReportReceived{};
};

bool validatesEventIsolation() {
    auto suite = Testament::Suite("handler event isolation", Testament::Test("passes", [] {}));
    auto recordingHandler = std::make_unique<RecordingHandler>();
    auto* recording = recordingHandler.get();

    Testament::Runner runner;
    runner.addHandler(std::make_unique<ThrowingEventHandler>());
    runner.addHandler(std::move(recordingHandler));

    const auto exitCode = runner.run(0, nullptr);
    return suite && exitCode == 1 && recording->testPassed && recording->finalReportReceived;
}

bool validatesConfigurationIsolation() {
    auto recordingHandler = std::make_unique<RecordingHandler>();
    auto* recording = recordingHandler.get();

    Testament::Runner runner;
    runner.addHandler(std::make_unique<ThrowingConfigurationHandler>());
    runner.addHandler(std::move(recordingHandler));

    return runner.run(0, nullptr) == 2 && recording->configured;
}

}

int main() {
    return validatesEventIsolation() && validatesConfigurationIsolation() ? 0 : 1;
}
