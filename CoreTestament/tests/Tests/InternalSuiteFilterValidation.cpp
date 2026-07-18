#include "Testament/Testament.hpp"

#include "core/Internal/InternalSuite.hpp"

#include <regex>
#include <string>

int main() {
    Testament::InternalSuite suite("filter validation");
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

    suite.setTestFilter(std::string{"alpha"});
    const bool exactRunSucceeded = suite.run();
    const bool exactFilterMatched = alphaRuns == 1
        && betaOneRuns == 0
        && betaTwoRuns == 0
        && suite.getStatistics().getPassedTests() == 1;

    suite.setTestFilter(std::regex{"beta-.*"});
    const bool regexRunSucceeded = suite.run();
    const bool regexFilterMatched = alphaRuns == 1
        && betaOneRuns == 1
        && betaTwoRuns == 1
        && suite.getStatistics().getPassedTests() == 2;

    suite.setTestFilter(std::string{"values / two"});
    const bool parameterFilterSucceeded = suite.run();
    const bool parameterFilterMatched = betaTwoRuns == 3
        && suite.getStatistics().getPassedTests() == 1;

    return exactRunSucceeded && exactFilterMatched
        && regexRunSucceeded && regexFilterMatched
        && parameterFilterSucceeded && parameterFilterMatched
        ? 0
        : 1;
}
