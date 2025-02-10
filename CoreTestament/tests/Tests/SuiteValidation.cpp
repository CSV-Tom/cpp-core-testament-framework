#include "Testament/Testament.hpp"

//#include <memory>
#include <cassert>
#include <vector>
#include <string>

namespace Testament {

/// Utility class to track and validate test results.
class TestResultValidator {
public:
    /// Records the outcome of a test.
    static void recordResult(const std::string& testName, bool passed) {
        testResults.emplace_back(testName, passed);
    }

    /// Validates all recorded test results and asserts failures.
    static void verifyResults() {
        for (const auto& [testName, passed] : testResults) {
            assert(passed && (testName + " failed").c_str());
        }
    }

private:
    static inline std::vector<std::pair<std::string, bool>> testResults;
};


/// Suite for validating fundamental test logic.
static auto suite = makeSuite("Fundamental Test Suite",
    makeTest("Equality Test", []() {
        int expectedValue = 42;
        int computedValue = 42;
        TestResultValidator::recordResult("Equality Test", computedValue == expectedValue);
    }),
    makeTest("Boolean Condition Test", []() {
        bool conditionMet = true;
        TestResultValidator::recordResult("Boolean Condition Test", conditionMet);
    })
);

/// Test case to validate the recorded test results.
static auto testResultValidation = makeTest("Test Result Validation", []() {
    TestResultValidator::verifyResults();
});

}
