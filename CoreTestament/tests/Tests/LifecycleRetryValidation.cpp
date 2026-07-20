#include "Testament/Testament.hpp"

#include "runtime/SuiteInstance.hpp"

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
    Testament::detail::SuiteInstance suite("lifecycle retry");
    unsigned int beforeEachCalls = 0;
    unsigned int afterEachCalls = 0;
    unsigned int attempts = 0;

    suite.setBeforeEach([&beforeEachCalls] { ++beforeEachCalls; });
    suite.setAfterEach([&afterEachCalls] { ++afterEachCalls; });
    suite.addTest(Testament::detail::RuntimeBridge::makeTest(
        "flaky",
        Testament::TestOptions{}.maxAttempts(3),
        [&attempts] {
            if (++attempts < 3) throw std::runtime_error("retry");
        }
    ));

    RecordingHandler handler;
    const bool succeeded = suite.run(&handler);

    return succeeded
        && attempts == 3
        && beforeEachCalls == 3
        && afterEachCalls == 3
        && suite.statistics().passedTests() == 1
        && handler.passed == 1
        && handler.reportedPassed == 1
        && handler.status == Testament::TestEventHandler::TestResultStatus::Passed
        ? 0
        : 1;
}
