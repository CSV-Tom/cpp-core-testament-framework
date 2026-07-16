#include "Testament/Test.hpp"

#include "core/Internal/InternalSuite.hpp"

int main() {
    Testament::InternalSuite suite("repeat-run validation");
    suite.addTest(Testament::Test::create("passing test", [] {}));

    suite.run();
    suite.run();

    const auto& statistics = suite.getStatistics();
    return statistics.getTotalTests() == 1 && statistics.getPassedTests() == 1
        ? 0
        : 1;
}
