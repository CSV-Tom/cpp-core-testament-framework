#ifndef TESTAMENT_TESTEVENTHANDLER_HPP
#define TESTAMENT_TESTEVENTHANDLER_HPP

#include <chrono>
#include <exception>
#include <expected>
#include <span>
#include <source_location>
#include <string>
#include <string_view>

#include "Testament/SuiteOptions.hpp"
#include "Testament/TestOptions.hpp"
#include "Testament/RunSummary.hpp"

namespace Testament {

class TestEventHandler {
public:
    // Argument views remain valid for the duration of Runner::run.
    using Arguments = std::span<const std::string_view>;

    enum class TestResultStatus {
        NotRun,
        Passed,
        Failed,
        Skipped,
        LifecycleError
    };

    struct TestInfo {
        std::string name;
        std::source_location location;
        std::chrono::duration<double> duration;
        std::exception_ptr exception;
        TestOptions options;
        TestResultStatus status{TestResultStatus::NotRun};
    };

    struct SuiteInfo {
        std::string name;
        std::source_location location;
        unsigned int passed{};
        unsigned int failed{};
        unsigned int skipped{};
        unsigned int errors{};
        SuiteOptions options;
    };

    virtual ~TestEventHandler() = default;

    // Runner contains exceptions thrown by handlers, continues dispatching to the remaining
    // handlers and reports a failed test run.
    // Return an error message to reject the configuration before tests execute.
    [[nodiscard]] virtual std::expected<void, std::string> configure(Arguments) { return {}; }
    virtual void onStartReport(unsigned int) {}
    virtual void onSuiteStart(const SuiteInfo&) {}
    virtual void onSuiteEnd(const SuiteInfo&) {}
    virtual void onSuiteAbort(const SuiteInfo&, std::string_view) {}
    virtual void onTestStart(const SuiteInfo&, const TestInfo&) {}
    virtual void onTestPassed(const SuiteInfo&, const TestInfo&) {}
    virtual void onTestFailed(const SuiteInfo&, const TestInfo&) {}
    virtual void onTestSkipped(const SuiteInfo&, const TestInfo&) {}
    virtual void onEnvironmentError(std::string_view, std::string_view) {}
    virtual void onFinalReport(const RunSummary&) {}

    [[nodiscard]] virtual std::string errorMessage() const { return {}; }
};

}

#endif
