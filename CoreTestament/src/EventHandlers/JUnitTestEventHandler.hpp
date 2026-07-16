#pragma once

#include "Testament/TestEventHandler.hpp"

#include <filesystem>
#include <string>
#include <vector>

class JUnitTestEventHandler final : public Testament::TestEventHandler {
public:
    explicit JUnitTestEventHandler(std::filesystem::path outputPath);

    void onStartReport(unsigned int suiteCount) override;
    void onSuiteStart(const SuiteInfo& suite) override;
    void onSuiteAbort(const SuiteInfo& suite, const std::string& message) override;
    void onTestPassed(const SuiteInfo& suite, const TestInfo& test) override;
    void onTestFailed(const SuiteInfo& suite, const TestInfo& test) override;
    void onTestSkipped(const SuiteInfo& suite, const TestInfo& test) override;
    void onFinalReport(unsigned int suites, unsigned int passed,
                       unsigned int failed, unsigned int skipped) override;

    [[nodiscard]] bool writeSucceeded() const noexcept;
    [[nodiscard]] std::string errorMessage() const override;

private:
    enum class Status {
        Passed,
        Failed,
        Skipped
    };

    struct TestResult {
        TestInfo info;
        Status status;
        std::string failureMessage;
    };

    struct SuiteResult {
        std::string name;
        std::vector<TestResult> tests;
        std::vector<std::string> lifecycleErrors;
    };

    SuiteResult& suiteResult(const SuiteInfo& suite);
    void writeReport();

    std::filesystem::path outputPath;
    std::vector<SuiteResult> suiteResults;
    bool reportWritten{false};
    std::string writeError;
};
