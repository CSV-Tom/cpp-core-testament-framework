#include "Testament/Testament.hpp"

#include <memory>
#include <string>
#include <vector>

namespace {

inline int beforeAllCalls = 0;
inline int afterAllCalls = 0;
inline int beforeEachCalls = 0;
inline int afterEachCalls = 0;
inline int sum = 0;

class Fixture final : public Testament::LifecycleSuite {
    void beforeAll() override { ++beforeAllCalls; }
    void afterAll() override { ++afterAllCalls; }
    void beforeEach() override { ++beforeEachCalls; }
    void afterEach() override { ++afterEachCalls; }
};

class RecordingHandler final : public Testament::TestEventHandler {
public:
    void onTestPassed(const SuiteInfo&, const TestInfo& test) override {
        names.push_back(test.name);
    }

    std::vector<std::string> names;
};

inline const auto suite = Testament::Suite<Fixture>(
    "fixture definition validation",
    Testament::Test("normal", [](Fixture&) { ++sum; }),
    Testament::Test(
        "move-only callable",
        [value = std::make_unique<int>(5)](Fixture&) { sum += *value; }
    ),
    Testament::ParameterizedTest(
        "values",
        Testament::Cases(
            Testament::TestCase("one", 1),
            Testament::TestCase("two", 2),
            Testament::TestCase("three", 3)
        ),
        [](Fixture&, int value) { sum += value; }
    ),
    Testament::ParameterizedTest(
        "move-only",
        Testament::Cases(
            Testament::TestCase("owned", std::make_unique<int>(4))
        ),
        [](Fixture&, const std::unique_ptr<int>& value) { sum += *value; }
    ),
    Testament::ParameterizedTest(
        "move-only parameter callable",
        Testament::Cases(Testament::TestCase("value", 2)),
        [base = std::make_unique<int>(5)](Fixture&, int value) { sum += *base + value; }
    )
);

}

int main() {
    auto handler = std::make_unique<RecordingHandler>();
    auto* result = handler.get();
    Testament::Runner runner;
    runner.addHandler(std::move(handler));

    return suite
        && runner.run(0, nullptr) == 0
        && sum == 23
        && beforeAllCalls == 1
        && afterAllCalls == 1
        && beforeEachCalls == 7
        && afterEachCalls == 7
        && result->names == std::vector<std::string>{
            "normal", "move-only callable", "values / one", "values / two",
            "values / three", "move-only / owned", "move-only parameter callable / value"
        }
        ? 0
        : 1;
}
