#include <sstream>
#include <iomanip>

#include "TestEventHandler.hpp"
#include "TestCase.hpp"
#include "TestSuite.hpp"
#include "ExecutionTimer.hpp"
#include "TestStatistics.hpp"



class ConsoleTestEventHandler : public TestEventHandler {
public:
    static auto convert(const ExecutionTimer& executionTimer) {
        std::ostringstream os;
        os << " (🕒 " << std::fixed << std::setprecision(2) << executionTimer.getDuration().count() * 1e9 << " ns" << " ) ";
        return os;
    }


    void onTestPassed(const TestSuite& suite, const TestCase& testCase) override {
        std::cout << "✅ PASSED: " << testCase.getName() << convert(testCase.getExecutionTimer()).str()  << std::endl;
    }

    void onTestFailed(const TestSuite& suite, const TestCase& testCase, std::exception_ptr exception) override {
        try {
            if (exception) {
                std::rethrow_exception(exception);
            }
        } catch (const std::exception& e) {
            std::cout << "❌ FAILED: "  << testCase.getName() << convert(testCase.getExecutionTimer()).str() << ".\n\n[ERROR]\n" << e.what() << "\n" << std::endl;
        }
    }

    void onTestSkipped(const TestSuite& suite, const TestCase& testCase) override {
        std::cout << "⏩ SKIPPED: " << testCase.getName() << convert(testCase.getExecutionTimer()).str() << std::endl;
    }

    void onSuiteSummary(const TestSuite& suite, const std::vector<std::shared_ptr<TestCase>>& testCases) override {
        std::cout << "\n"
                  << "📊 [RESULTS] Suite Summary: "
                  << "Total: " <<  testCases.size() << ", "
                  << "Passed: " << suite.getTestStatistics().getPassedTests() << ", "
                  << "Failed: " << suite.getTestStatistics().getFailedTests() << ", "
                  << "Skipped: " << suite.getTestStatistics().getSkippedTests() <<  "\n"
                  << std::endl;
    }

    void onSuiteStart(const TestSuite& suite) override {
        std::cout << "⏩ ===============================================================================" << std::endl;
        std::cout << "⏩ [STARTING SUITE] " << suite.getName() << std::endl;
        std::cout << "⏩ ===============================================================================" << std::endl;
    }

    void onSuiteEnd(const TestSuite& suite) override {
        std::cout << std::endl;
    }

    void onLogEvent(const std::string message) override {
        std::cout << "-->  " << message << std::endl;
    }

    void onSuiteAbort(const std::string& message) override {
        std::cout << "[ABORT] Suite aborted: " << message << std::endl;
    }

    void onStartReport(const std::vector<std::shared_ptr<TestSuite>>& suites) override {
        std::cout << "[BEGIN] Starting test run for " << suites.size() << " suite(s)." << "\n" << std::endl;
    }

    void onFinalReport(const std::vector<std::shared_ptr<TestSuite>>& suites) override {
        TestStatistics<unsigned int> stat;
        // TODO: OHne Schleife, globales Objekt mit Konfiguration, Stats etc.
        for (const auto& suite : suites) {
            stat += suite->getTestStatistics();
        }
        std::cout << "📊 [FINAL REPORT]" << std::endl;
        std::cout << "    ";
        std::cout << "Suites Executed: " << suites.size() << ", ";
        std::cout << "Total Tests: " << stat.getTotalTests() << ", ";
        std::cout << "Failed: " << stat.getFailedTests() <<  ", ";
        std::cout << "Skipped: " << stat.getSkippedTests() << std::endl;

    }
};
