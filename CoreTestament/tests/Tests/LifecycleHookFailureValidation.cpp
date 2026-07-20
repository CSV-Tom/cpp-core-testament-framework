#include "Testament/Testament.hpp"
#include "Testament/Runner.hpp"

#include "runtime/SuiteRegistry.hpp"
#include "runtime/SuiteInstance.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

class RecordingHandler : public Testament::TestEventHandler {
public:
    void onSuiteAbort(const SuiteInfo& suite, std::string_view message) override {
        suiteName = suite.name;
        error = message;
        reportedErrors = suite.errors;
    }

    void onTestFailed(const SuiteInfo& suite, const TestInfo& test) override {
        ++failedTests;
        reportedFailures = suite.failed;
        reportedErrors = suite.errors;
        status = test.status;
    }

    void onTestSkipped(const SuiteInfo& suite, const TestInfo& test) override {
        ++skippedTests;
        reportedSkipped = suite.skipped;
        status = test.status;
    }

    std::string suiteName;
    std::string error;
    unsigned int failedTests{};
    unsigned int reportedFailures{};
    unsigned int skippedTests{};
    unsigned int reportedSkipped{};
    unsigned int reportedErrors{};
    TestResultStatus status{TestResultStatus::NotRun};
};

}

int main() {
    auto suite = Testament::detail::SuiteRegistry::instance().registerSuite(
        std::make_shared<Testament::detail::SuiteInstance>("failing lifecycle hook")
    );
    bool testExecuted = false;

    suite->setBeforeSuite([] {
        throw std::runtime_error("expected hook failure");
    });
    suite->addTest(Testament::detail::RuntimeBridge::makeTest("must not run", {}, [&testExecuted] {
        testExecuted = true;
    }));

    RecordingHandler handler;
    const bool suiteSucceeded = suite->run(&handler);

    Testament::detail::SuiteInstance beforeEachSuite("failing before each hook");
    bool beforeEachTestExecuted = false;
    beforeEachSuite.setBeforeEach([] {
        throw std::runtime_error("expected before each failure");
    });
    beforeEachSuite.addTest(Testament::detail::RuntimeBridge::makeTest("must also not run", {}, [&beforeEachTestExecuted] {
        beforeEachTestExecuted = true;
    }));
    RecordingHandler beforeEachHandler;
    const bool beforeEachSuiteSucceeded = beforeEachSuite.run(&beforeEachHandler);

    const int exitCode = Testament::run(0, nullptr);
    return !suiteSucceeded
        && !beforeEachSuiteSucceeded
        && exitCode == 1
        && !testExecuted
        && !beforeEachTestExecuted
        && suite->statistics().failedTests() == 0
        && suite->statistics().skippedTests() == 1
        && suite->statistics().errors() == 1
        && beforeEachSuite.statistics().failedTests() == 0
        && beforeEachSuite.statistics().errors() == 1
        && handler.suiteName == "failing lifecycle hook"
        && handler.error == "Error in beforeSuite: expected hook failure"
        && handler.failedTests == 0
        && handler.skippedTests == 1
        && handler.reportedSkipped == 1
        && handler.reportedErrors == 1
        && handler.status == Testament::TestEventHandler::TestResultStatus::Skipped
        && beforeEachHandler.failedTests == 1
        && beforeEachHandler.reportedFailures == 0
        && beforeEachHandler.reportedErrors == 1
        && beforeEachHandler.status == Testament::TestEventHandler::TestResultStatus::LifecycleError
        ? 0
        : 1;
}
