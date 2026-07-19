#include "Testament/Testament.hpp"

#include "../TestCheck.hpp"

#include <algorithm>
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
    const auto exitCode = runner.run(0, nullptr);
    auto names = result->names;
    std::ranges::sort(names);

    Testament::TestSupport::Checks checks;
    checks.expect(static_cast<bool>(suite), "suite registration remains active");
    checks.expect(exitCode == 0, "runner succeeds");
    checks.expect(sum == 23, "all fixture test bodies contribute to the sum");
    checks.expect(beforeAllCalls == 1, "beforeAll runs once");
    checks.expect(afterAllCalls == 1, "afterAll runs once");
    checks.expect(beforeEachCalls == 7, "beforeEach runs for every test case");
    checks.expect(afterEachCalls == 7, "afterEach runs for every test case");
    checks.expect(
        names == std::vector<std::string>{
            "move-only / owned", "move-only callable", "move-only parameter callable / value",
            "normal", "values / one", "values / three", "values / two"
        },
        "all expected fixture tests are reported"
    );
    return checks.result();
}
