#include "Testament/Testament.hpp"

#include <exception>
#include <memory>
#include <string>
#include <string_view>

namespace {

class RecordingHandler final : public Testament::TestEventHandler {
public:
    void onTestFailed(const SuiteInfo&, const TestInfo& test) override {
        ++failed;
        try {
            std::rethrow_exception(test.exception);
        } catch (const Testament::AssertionFailure& failure) {
            message = failure.what();
        }
    }

    void onTestPassed(const SuiteInfo&, const TestInfo&) override {
        ++passed;
    }

    unsigned int failed{};
    unsigned int passed{};
    std::string message;
};

}

int main() {
    bool continuedAfterFirstFailure = false;
    bool continuedAfterSecondFailure = false;
    auto suite = Testament::Suite(
        "non-fatal assertions",
        Testament::Test("collect failures", [&] {
            Testament::Asserts::expect([] {
                Testament::Asserts::assertEquals(1, 2, "first expectation");
            });
            continuedAfterFirstFailure = true;
            Testament::Asserts::expect([] {
                Testament::Asserts::assertTrue(false, "second expectation");
            });
            continuedAfterSecondFailure = true;
        }),
        Testament::Test("subsequent test", [] {})
    );

    auto handler = std::make_unique<RecordingHandler>();
    auto* result = handler.get();
    Testament::Runner runner;
    runner.addHandler(std::move(handler));
    const auto exitCode = runner.run(0, nullptr);

    return suite
        && exitCode == 1
        && continuedAfterFirstFailure
        && continuedAfterSecondFailure
        && result->failed == 1
        && result->passed == 1
        && std::string_view{result->message}.contains("2 non-fatal assertions failed")
        && std::string_view{result->message}.contains("first expectation")
        && std::string_view{result->message}.contains("second expectation")
        ? 0
        : 1;
}
