#include "Testament/Testament.hpp"

#include "runtime/SuiteInstance.hpp"

#include <string>

int main() {
    Testament::detail::SuiteInstance suite("filter validation");
    int alphaRuns = 0;
    int betaOneRuns = 0;
    int betaTwoRuns = 0;

    suite.addTest(Testament::detail::RuntimeBridge::makeTest("alpha", {}, [&alphaRuns] { ++alphaRuns; }));
    suite.addTest(Testament::detail::RuntimeBridge::makeTest("beta-one", {}, [&betaOneRuns] { ++betaOneRuns; }));
    suite.addTest(Testament::detail::RuntimeBridge::makeTest("beta-two", {}, [&betaTwoRuns] { ++betaTwoRuns; }));
    auto parameterized = Testament::ParameterizedTest(
        "values",
        Testament::Cases(
            Testament::TestCase("one", 1),
            Testament::TestCase("two", 2)
        ),
        [&betaTwoRuns](int value) { betaTwoRuns += value; }
    );
    for (auto& test : std::move(parameterized).materialize<void>()) {
        suite.addTest(std::move(test));
    }

    const bool exactRunSucceeded = suite.run(nullptr, {"alpha", {}, 1, {}});
    const bool exactFilterMatched = alphaRuns == 1
        && betaOneRuns == 0
        && betaTwoRuns == 0
        && suite.statistics().passedTests() == 1;

    const bool globRunSucceeded = suite.run(nullptr, {"beta-*", {}, 1, {}});
    const bool regexFilterMatched = alphaRuns == 1
        && betaOneRuns == 1
        && betaTwoRuns == 1
        && suite.statistics().passedTests() == 2;

    const bool parameterFilterSucceeded = suite.run(nullptr, {"values / two", {}, 1, {}});
    const bool parameterFilterMatched = betaTwoRuns == 3
        && suite.statistics().passedTests() == 1;

    return exactRunSucceeded && exactFilterMatched
        && globRunSucceeded && regexFilterMatched
        && parameterFilterSucceeded && parameterFilterMatched
        ? 0
        : 1;
}
