#include "Testament/Testament.hpp"

#include "core/Internal/InternalSuite.hpp"

#include <stdexcept>

namespace {

class RecordingHandler final : public Testament::TestEventHandler {
public:
    void onTestPassed(const SuiteInfo& suite, const TestInfo& test) override {
        ++passed;
        reportedPassed = suite.passed;
        status = test.status;
    }

    unsigned int passed{};
    unsigned int reportedPassed{};
    TestResultStatus status{TestResultStatus::NotRun};
};

}

int main() {
    Testament::InternalSuite suite("lifecycle retry");
    unsigned int beforeEachCalls = 0;
    unsigned int afterEachCalls = 0;
    unsigned int attempts = 0;

    suite.setBeforeEach([&beforeEachCalls] { ++beforeEachCalls; });
    suite.setAfterEach([&afterEachCalls] { ++afterEachCalls; });
    suite.addTest(Testament::makeTest(
        "flaky",
        [&attempts] {
            if (++attempts < 3) throw std::runtime_error("retry");
        },
        Testament::TestOptions{}.retries(2)
    ));

    RecordingHandler handler;
    suite.setHandler(&handler);
    const bool succeeded = suite.run();

    return succeeded
        && attempts == 3
        && beforeEachCalls == 3
        && afterEachCalls == 3
        && suite.getStatistics().getPassedTests() == 1
        && handler.passed == 1
        && handler.reportedPassed == 1
        && handler.status == Testament::TestEventHandler::TestResultStatus::Passed
        ? 0
        : 1;
}
