#include "Testament/Testament.hpp"

#include <tuple>
#include <utility>
#include <vector>

namespace {

class ExpectedFixture : public Testament::LifecycleSuite {};
class ActualFixture : public Testament::LifecycleSuite {};

}

int main() {
    bool testBodyEntered = false;
    bool parameterizedBodyEntered = false;
    int parameterInvocationCount = 0;
    int parameterSum = 0;

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
        std::vector<std::tuple<int>>{{1}, {2}, {3}}
    );
    auto matchingSuite = Testament::makeSuite<ExpectedFixture>(
        "matching parameterized fixture validation",
        std::move(matchingParameterizedTest)
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
        ? 0
        : 1;
}
