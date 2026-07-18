#include "Testament/Testament.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

class RecordingHandler final : public Testament::TestEventHandler {
public:
    void onSuiteStart(const SuiteInfo& suite) override {
        suites.push_back(suite.name);
        if (suite.name == "alpha") {
            suiteMetadataReceived = suite.options.tags().size() == 1
                && suite.options.tags().front() == "integration"
                && suite.options.attribute("component") == "database";
        }
    }

    void onTestStart(const SuiteInfo& suite, const TestInfo& test) override {
        tests.push_back(suite.name + "/" + test.name);
        if (test.name == "early") {
            testMetadataReceived = test.options.tags().size() == 1
                && test.options.tags().front() == "trace"
                && test.options.attribute("operation") == "insert";
        }
    }

    std::vector<std::string> suites;
    std::vector<std::string> tests;
    bool suiteMetadataReceived{};
    bool testMetadataReceived{};
};

}

int main() {
    Testament::SuiteOptions reusableSuiteOptions;
    reusableSuiteOptions.tag("integration").attribute("component", "database");
    auto copiedSuiteOptions = reusableSuiteOptions;
    copiedSuiteOptions.tag("copy-only");
    const bool optionsAreIndependent = reusableSuiteOptions.tags().size() == 1
        && copiedSuiteOptions.tags().size() == 2;

    auto movedSuiteOptions = std::move(copiedSuiteOptions);
    movedSuiteOptions.tag("moved-only");
    Testament::TestOptions originalTestOptions;
    originalTestOptions.disabled().maxAttempts(2);
    auto movedTestOptions = std::move(originalTestOptions);
    movedTestOptions.maxAttempts(3);
    const bool movedOptionsRemainValid = copiedSuiteOptions.tags().size() == 2
        && movedSuiteOptions.tags().size() == 3
        && originalTestOptions.isDisabled()
        && originalTestOptions.maxAttempts() == 2
        && movedTestOptions.maxAttempts() == 3;

    auto zeta = Testament::Suite(
        "zeta",
        Testament::Test("only", [] {})
    );
    auto alpha = Testament::Suite(
        "alpha",
        reusableSuiteOptions,
        Testament::Test("late", Testament::TestOptions{}.order(20), [] {}),
        Testament::Test("first default", [] {}),
        Testament::Test(
            "early",
            Testament::TestOptions{}
                .order(-5)
                .tag("trace")
                .attribute("operation", "insert"),
            [] {}
        ),
        Testament::Test("second default", [] {})
    );
    auto early = Testament::Suite(
        "early suite",
        Testament::SuiteOptions{}.order(-10),
        Testament::Test("only", [] {})
    );

    std::string suiteNameStorage = "[view suite]";
    std::string testNameStorage = "[view test]";
    auto viewed = Testament::Suite(
        std::string_view{suiteNameStorage}.substr(1, 10),
        Testament::Test(std::string_view{testNameStorage}.substr(1, 9), [] {})
    );
    suiteNameStorage.clear();
    testNameStorage.clear();

    auto handler = std::make_unique<RecordingHandler>();
    auto* result = handler.get();
    Testament::Runner runner;
    runner.addHandler(std::move(handler));
    const int exitCode = runner.run(0, nullptr);

    return zeta && alpha && early && viewed
        && exitCode == 0
        && optionsAreIndependent
        && movedOptionsRemainValid
        && result->suiteMetadataReceived
        && result->testMetadataReceived
        && result->suites == std::vector<std::string>{
            "early suite", "alpha", "view suite", "zeta"
        }
        && result->tests == std::vector<std::string>{
            "early suite/only",
            "alpha/early",
            "alpha/first default",
            "alpha/second default",
            "alpha/late",
            "view suite/view test",
            "zeta/only"
        }
        ? 0
        : 1;
}
