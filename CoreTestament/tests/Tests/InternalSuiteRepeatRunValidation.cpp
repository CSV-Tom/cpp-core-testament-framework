#include "Testament/Testament.hpp"

#include "core/Internal/InternalSuite.hpp"

int main() {
    Testament::InternalSuite suite("repeat-run validation");
    suite.addTest(Testament::detail::RuntimeBridge::makeTest("passing test", {}, [] {}));

    const bool firstRunSucceeded = suite.run();
    const bool secondRunSucceeded = suite.run();

    const auto& statistics = suite.statistics();
    return firstRunSucceeded && secondRunSucceeded
        && statistics.totalTests() == 1 && statistics.passedTests() == 1
        ? 0
        : 1;
}
