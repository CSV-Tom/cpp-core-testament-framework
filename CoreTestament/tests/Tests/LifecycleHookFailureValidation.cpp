#include "Testament/Test.hpp"
#include "Testament/Runner.hpp"

#include "core/Internal/InternalRegistry.hpp"
#include "core/Internal/InternalSuite.hpp"

#include <memory>
#include <stdexcept>
#include <string>

namespace {

class RecordingHandler : public Testament::TestEventHandler {
public:
    void onSuiteAbort(const SuiteInfo& suite, const std::string& message) override {
        suiteName = suite.name;
        error = message;
    }

    std::string suiteName;
    std::string error;
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
    suite->addTest(Testament::Test::create("must not run", [&testExecuted] {
        testExecuted = true;
    }));

    RecordingHandler handler;
    suite->setHandler(&handler);
    const bool suiteSucceeded = suite->run();
    const int exitCode = Testament::run(0, nullptr);
    return !suiteSucceeded
        && exitCode == 1
        && !testExecuted
        && suite->getStatistics().getTotalTests() == 0
        && handler.suiteName == "failing lifecycle hook"
        && handler.error == "Error in beforeSuite: expected hook failure"
        ? 0
        : 1;
}
