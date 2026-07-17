#include "Testament/Testament.hpp"

#include "core/Internal/InternalSuite.hpp"

int main() {
    Testament::InternalSuite suite("repeat-run validation");
    suite.addTest(Testament::makeTest("passing test", [] {}));

    const bool firstRunSucceeded = suite.run();
    const bool secondRunSucceeded = suite.run();

    const auto& statistics = suite.getStatistics();
    return firstRunSucceeded && secondRunSucceeded
        && statistics.getTotalTests() == 1 && statistics.getPassedTests() == 1
        ? 0
        : 1;
}
