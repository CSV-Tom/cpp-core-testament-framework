#include "Testament/Testament.hpp"

#include <memory>

namespace {

class RecordingHandler final : public Testament::TestEventHandler {
public:
    void onTestPassed(const SuiteInfo&, const TestInfo&) override {
        ++passed;
    }

    void onTestFailed(const SuiteInfo&, const TestInfo&) override {
        ++failed;
    }

    void onFinalReport(unsigned int, unsigned int, unsigned int, unsigned int) override {
        finalReportReceived = true;
    }

    unsigned int passed{};
    unsigned int failed{};
    bool finalReportReceived{};
};

}

int main() {
    auto suite = Testament::Suite(
        "non-standard exceptions",
        Testament::Test("throws integer", [] { throw 42; }),
        Testament::Test("continues afterwards", [] {})
    );

    auto recordingHandler = std::make_unique<RecordingHandler>();
    auto* result = recordingHandler.get();

    Testament::Runner runner;
    runner.addHandler(Testament::makeConsoleHandler());
    runner.addHandler(std::move(recordingHandler));

    const int exitCode = runner.run(0, nullptr);
    return suite
        && exitCode == 1
        && result->failed == 1
        && result->passed == 1
        && result->finalReportReceived
        ? 0
        : 1;
}
