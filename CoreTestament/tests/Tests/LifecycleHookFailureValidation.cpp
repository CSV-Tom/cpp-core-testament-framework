#include "Testament/Testament.hpp"
#include "Testament/Runner.hpp"

#include "core/Internal/InternalRegistry.hpp"
#include "core/Internal/InternalSuite.hpp"

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
    }

    void onTestFailed(const SuiteInfo& suite, const TestInfo& test) override {
        ++failedTests;
        reportedFailures = suite.failed;
        status = test.status;
    }

    std::string suiteName;
    std::string error;
    unsigned int failedTests{};
    unsigned int reportedFailures{};
    TestResultStatus status{TestResultStatus::NotRun};
};

}

int main() {
    auto suite = Testament::InternalRegistry::getInstance().registerSuite(
        std::make_shared<Testament::InternalSuite>("failing lifecycle hook")
    );
    bool testExecuted = false;

    suite->setBeforeSuite([] {
        throw std::runtime_error("expected hook failure");
    });
    suite->addTest(Testament::detail::makeRuntimeTest("must not run", {}, [&testExecuted] {
        testExecuted = true;
    }));

    RecordingHandler handler;
    suite->setHandler(&handler);
    const bool suiteSucceeded = suite->run();

    Testament::InternalSuite beforeEachSuite("failing before each hook");
    bool beforeEachTestExecuted = false;
    beforeEachSuite.setBeforeEach([] {
        throw std::runtime_error("expected before each failure");
    });
    beforeEachSuite.addTest(Testament::detail::makeRuntimeTest("must also not run", {}, [&beforeEachTestExecuted] {
        beforeEachTestExecuted = true;
    }));
    RecordingHandler beforeEachHandler;
    beforeEachSuite.setHandler(&beforeEachHandler);
    const bool beforeEachSuiteSucceeded = beforeEachSuite.run();

    const int exitCode = Testament::run(0, nullptr);
    return !suiteSucceeded
        && !beforeEachSuiteSucceeded
        && exitCode == 1
        && !testExecuted
        && !beforeEachTestExecuted
        && suite->getStatistics().getFailedTests() == 1
        && beforeEachSuite.getStatistics().getFailedTests() == 1
        && handler.suiteName == "failing lifecycle hook"
        && handler.error == "Error in beforeSuite: expected hook failure"
        && handler.failedTests == 1
        && handler.reportedFailures == 1
        && handler.status == Testament::TestEventHandler::TestResultStatus::LifecycleError
        && beforeEachHandler.failedTests == 1
        && beforeEachHandler.reportedFailures == 1
        && beforeEachHandler.status == Testament::TestEventHandler::TestResultStatus::LifecycleError
        ? 0
        : 1;
}
