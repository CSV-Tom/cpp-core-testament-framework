#include "Testament/Testament.hpp"

#include "core/Internal/InternalSuite.hpp"

#include <regex>
#include <string>

int main() {
    Testament::InternalSuite suite("filter validation");
    int alphaRuns = 0;
    int betaOneRuns = 0;
    int betaTwoRuns = 0;

    suite.addTest(Testament::makeTest("alpha", [&alphaRuns] { ++alphaRuns; }));
    suite.addTest(Testament::makeTest("beta-one", [&betaOneRuns] { ++betaOneRuns; }));
    suite.addTest(Testament::makeTest("beta-two", [&betaTwoRuns] { ++betaTwoRuns; }));

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

    return exactRunSucceeded && exactFilterMatched
        && regexRunSucceeded && regexFilterMatched
        ? 0
        : 1;
}
