#include "Testament/Testament.hpp"

#include <memory>
#include <utility>

namespace {

class RecordingHandler final : public Testament::TestEventHandler {
public:
    void onTestStart(const SuiteInfo&, const TestInfo&) override {
        ++started;
    }

    void onTestPassed(const SuiteInfo&, const TestInfo& test) override {
        ++passed;
        retryMetadataReceived = test.options.retryCount() == 2;
    }

    void onTestFailed(const SuiteInfo&, const TestInfo& test) override {
        ++failed;
        retryMetadataReceived = retryMetadataReceived && test.options.retryCount() == 2;
    }

    void onTestSkipped(const SuiteInfo&, const TestInfo& test) override {
        ++skipped;
        disabledMetadataReceived = test.options.isDisabled();
    }

    int started{};
    int passed{};
    int failed{};
    int skipped{};
    bool retryMetadataReceived{};
    bool disabledMetadataReceived{};
};

}

int main() {
    int disabledExecutions = 0;
    int flakyExecutions = 0;
    int failingExecutions = 0;

    auto suite = Testament::makeSuite(
        "test option execution",
        Testament::makeTest(
            "disabled",
            [&disabledExecutions] { ++disabledExecutions; },
            Testament::TestOptions{}.disabled()
        ),
        Testament::makeTest(
            "flaky",
            [&flakyExecutions] {
                if (++flakyExecutions < 3) {
                    throw 1;
                }
            },
            Testament::TestOptions{}.retries(2)
        ),
        Testament::makeTest(
            "always failing",
            [&failingExecutions] {
                ++failingExecutions;
                throw 1;
            },
            Testament::TestOptions{}.retries(2)
        )
    );

    auto handler = std::make_unique<RecordingHandler>();
    auto* result = handler.get();
    Testament::Runner runner;
    runner.addHandler(std::move(handler));
    const int exitCode = runner.run(0, nullptr);

    return suite
        && exitCode == 1
        && disabledExecutions == 0
        && flakyExecutions == 3
        && failingExecutions == 3
        && result->started == 2
        && result->passed == 1
        && result->failed == 1
        && result->skipped == 1
        && result->retryMetadataReceived
        && result->disabledMetadataReceived
        ? 0
        : 1;
}
