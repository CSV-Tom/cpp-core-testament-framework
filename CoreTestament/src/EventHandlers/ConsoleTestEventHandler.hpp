#pragma once
#include <iostream>
#include <sstream>
#include <iomanip>
#include "TestEventHandler.hpp"

class ConsoleTestEventHandler : public TestEventHandler {
public:
    static std::string formatDuration(std::chrono::duration<double> d) {
        std::ostringstream os;
        os << " (🕒 " << std::fixed << std::setprecision(2) << d.count() * 1e9 << " ns) ";
        return os.str();
    }

    void onStartReport(unsigned int suiteCount) override {
        std::cout << "[BEGIN] Starting test run for " << suiteCount << " suite(s).\n" << std::endl;
    }

    void onSuiteStart(const SuiteInfo& suite) override {
        std::cout << "⏩ ===============================================================================\n"
                  << "⏩ [STARTING SUITE] " << suite.name << "\n"
                  << "⏩ ===============================================================================" << std::endl;
    }

    void onSuiteEnd(const SuiteInfo& suite) override {
        std::cout << "\n📊 [RESULTS] Suite Summary: "
                  << "Total: " << (suite.passed + suite.failed + suite.skipped) << ", "
                  << "Passed: " << suite.passed << ", "
                  << "Failed: " << suite.failed << ", "
                  << "Skipped: " << suite.skipped << "\n" << std::endl;
    }

    void onSuiteAbort(const std::string& message) override {
        std::cout << "[ABORT] Suite aborted: " << message << std::endl;
    }

    void onTestPassed(const SuiteInfo&, const TestInfo& test) override {
        std::cout << "✅ PASSED: " << test.name << formatDuration(test.duration) << std::endl;
    }

    void onTestFailed(const SuiteInfo&, const TestInfo& test) override {
        std::cout << "❌ FAILED: " << test.name << formatDuration(test.duration);
        if (test.exception) {
            try {
                std::rethrow_exception(test.exception);
            } catch (const std::exception& e) {
                std::cout << ".\n\n[ERROR]\n" << e.what();
            }
        }
        std::cout << "\n" << std::endl;
    }

    void onTestSkipped(const SuiteInfo&, const TestInfo& test) override {
        std::cout << "⏩ SKIPPED: " << test.name << formatDuration(test.duration) << std::endl;
    }

    void onLogEvent(const std::string& message) override {
        std::cout << "--> " << message << std::endl;
    }

    void onFinalReport(unsigned int suites, unsigned int passed, unsigned int failed, unsigned int skipped) override {
        std::cout << "\n📊 [FINAL REPORT]\n"
                  << "    Suites Executed: " << suites << ", "
                  << "Total Tests: " << (passed + failed + skipped) << ", "
                  << "Passed: " << passed << ", "
                  << "Failed: " << failed << ", "
                  << "Skipped: " << skipped << std::endl;
    }
};
