#include "Testament/Testament.hpp"

#include <memory>
#include <tuple>
#include <utility>
#include <vector>

namespace {

class ExpectedFixture : public Testament::LifecycleSuite {};
class ActualFixture : public Testament::LifecycleSuite {};
class NonDefaultFixture : public Testament::LifecycleSuite {
public:
    explicit NonDefaultFixture(int) {}
};

struct RvalueOnlyCallable {
    void operator()() && {}
};

template <typename Fixture>
concept SuiteFactoryAccepts = requires { Testament::makeSuite<Fixture>("suite"); };

template <typename Callable>
concept TestFactoryAccepts = requires(Callable&& callable) {
    Testament::makeTest("test", std::forward<Callable>(callable));
};

static_assert(!SuiteFactoryAccepts<NonDefaultFixture>);
static_assert(!TestFactoryAccepts<RvalueOnlyCallable>);

}

int main() {
    bool testBodyEntered = false;
    bool parameterizedBodyEntered = false;
    int parameterInvocationCount = 0;
    int parameterSum = 0;
    int moveOnlyParameterSum = 0;

    auto mismatchedTest = Testament::makeTest<ExpectedFixture>(
        "mismatched fixture",
        [&testBodyEntered](ExpectedFixture&) {
            testBodyEntered = true;
        }
    );
    auto mismatchedSuite = Testament::makeSuite<ActualFixture>(
        "fixture type validation",
        std::move(mismatchedTest)
    );

    auto matchingParameterizedTest = Testament::makeParameterizedTest<ExpectedFixture>(
        "matching parameterized fixture",
        [&parameterInvocationCount, &parameterSum](ExpectedFixture&, int value) {
            ++parameterInvocationCount;
            parameterSum += value;
        },
        std::vector<std::tuple<int>>{{1}, {2}, {3}},
        Testament::TestOptions{}.tag("fixture")
    );
    std::vector<std::tuple<std::unique_ptr<int>>> moveOnlyParameters;
    moveOnlyParameters.emplace_back(std::make_unique<int>(4));
    auto moveOnlyParameterizedTest = Testament::makeParameterizedTest<ExpectedFixture>(
        "move-only fixture parameter",
        [&moveOnlyParameterSum](ExpectedFixture&, const std::unique_ptr<int>& value) {
            moveOnlyParameterSum += *value;
        },
        std::move(moveOnlyParameters)
    );
    auto matchingSuite = Testament::makeSuite<ExpectedFixture>(
        "matching parameterized fixture validation",
        Testament::SuiteOptions{}.order(1),
        std::move(matchingParameterizedTest),
        std::move(moveOnlyParameterizedTest)
    );

    auto mismatchedParameterizedTest = Testament::makeParameterizedTest<ExpectedFixture>(
        "mismatched parameterized fixture",
        [&parameterizedBodyEntered](ExpectedFixture&, int) {
            parameterizedBodyEntered = true;
        },
        std::vector<std::tuple<int>>{{1}, {2}}
    );
    auto mismatchedParameterizedSuite = Testament::makeSuite<ActualFixture>(
        "mismatched parameterized fixture validation",
        std::move(mismatchedParameterizedTest)
    );
    static_cast<void>(mismatchedSuite);
    static_cast<void>(matchingSuite);
    static_cast<void>(mismatchedParameterizedSuite);

    const int runnerResult = Testament::run(0, nullptr);
    return runnerResult == 1
        && !testBodyEntered
        && !parameterizedBodyEntered
        && parameterInvocationCount == 3
        && parameterSum == 6
        && moveOnlyParameterSum == 4
        ? 0
        : 1;
}
