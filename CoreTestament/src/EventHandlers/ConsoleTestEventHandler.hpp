#pragma once
#include <chrono>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include "Testament/TestEventHandler.hpp"

namespace Testament::detail {

class ConsoleTestEventHandler final : public TestEventHandler {
public:
    static std::string formatDuration(std::chrono::duration<double> d) {
        std::ostringstream os;
        os << " (🕒 " << std::fixed << std::setprecision(2) << d.count() * 1e9 << " ns) ";
        return os.str();
    }

    static std::string formatLocation(std::source_location location) {
        if (std::string_view{location.file_name()}.empty()) return {};
        return std::string{location.file_name()} + ':' + std::to_string(location.line());
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

    void onSuiteAbort(const SuiteInfo& suite, std::string_view message) override {
        std::cout << "[ABORT] Suite '" << suite.name << "' aborted: " << message;
        if (const auto location = formatLocation(suite.location); !location.empty()) {
            std::cout << "\n[DEFINED AT] " << location;
        }
        std::cout << std::endl;
    }

    void onTestPassed(const SuiteInfo&, const TestInfo& test) override {
        std::cout << "✅ PASSED: " << test.name << formatDuration(test.duration) << std::endl;
    }

    void onTestFailed(const SuiteInfo&, const TestInfo& test) override {
        const bool lifecycleError = test.status == TestResultStatus::LifecycleError;
        std::cout << (lifecycleError ? "⚠️ LIFECYCLE ERROR: " : "❌ FAILED: ")
                  << test.name << formatDuration(test.duration);
        if (test.exception) {
            try {
                std::rethrow_exception(test.exception);
            } catch (const std::exception& e) {
                std::cout << ".\n\n[ERROR]\n" << e.what();
            } catch (...) {
                std::cout << ".\n\n[ERROR]\nUnknown non-standard exception";
            }
        }
        if (const auto location = formatLocation(test.location); !location.empty()) {
            std::cout << "\n[DEFINED AT] " << location;
        }
        std::cout << "\n" << std::endl;
    }

    void onTestSkipped(const SuiteInfo&, const TestInfo& test) override {
        std::cout << "⏩ SKIPPED: " << test.name << formatDuration(test.duration) << std::endl;
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

}
