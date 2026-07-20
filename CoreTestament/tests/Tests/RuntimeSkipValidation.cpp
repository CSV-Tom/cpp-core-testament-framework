#include "Testament/Testament.hpp"

#include <memory>
#include <string>

namespace {

class Fixture final : public Testament::LifecycleSuite {};

class RecordingHandler final : public Testament::TestEventHandler {
public:
    void onTestFailed(const SuiteInfo&, const TestInfo&) override {
        ++failed;
    }

    void onTestSkipped(const SuiteInfo&, const TestInfo& test) override {
        ++skipped;
        status = test.status;
        try {
            std::rethrow_exception(test.exception);
        } catch (const Testament::SkipRequest& request) {
            reason = request.reason();
        }
    }

    unsigned int failed{};
    unsigned int skipped{};
    std::string reason;
    TestResultStatus status{TestResultStatus::NotRun};
};

}

int main() {
    const Testament::SkipRequest original{"copy and move"};
    const auto copied = original;
    auto copyAssigned = Testament::SkipRequest{"temporary"};
    copyAssigned = copied;
    auto moved = Testament::SkipRequest{copied};
    auto moveAssigned = Testament::SkipRequest{"temporary"};
    moveAssigned = std::move(moved);

    unsigned int attempts{};
    unsigned int fixtureAttempts{};
    auto suite = Testament::Suite(
        "runtime skip",
        Testament::Test(
            "conditionally skipped",
            Testament::TestOptions{}.maxAttempts(3),
            [&attempts] {
                ++attempts;
                Testament::skip("optional dependency unavailable");
            }
        ),
        Testament::Test("passing test", [] {})
    );
    auto fixtureSuite = Testament::Suite<Fixture>(
        "fixture runtime skip",
        Testament::Test(
            "fixture skip",
            Testament::TestOptions{}.maxAttempts(3),
            [&fixtureAttempts](Fixture&) {
                ++fixtureAttempts;
                Testament::skip("fixture unavailable");
            }
        )
    );

    auto handler = std::make_unique<RecordingHandler>();
    auto* result = handler.get();
    Testament::Runner runner;
    runner.addHandler(std::move(handler)).maxParallelTests(2);

    return copied.reason() == "copy and move"
        && std::string_view{copied.what()} == "test skipped: copy and move"
        && copyAssigned.reason() == "copy and move"
        && moveAssigned.reason() == "copy and move"
        && suite && fixtureSuite
        && runner.run(0, nullptr) == 0
        && attempts == 1
        && fixtureAttempts == 1
        && result->failed == 0
        && result->skipped == 2
        && result->reason == "optional dependency unavailable"
        && result->status == Testament::TestEventHandler::TestResultStatus::Skipped
        ? 0
        : 1;
}
