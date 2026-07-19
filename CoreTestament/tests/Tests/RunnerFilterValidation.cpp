#include "Testament/Testament.hpp"

#include <memory>
#include <string>
#include <vector>

namespace {

class RecordingHandler final : public Testament::TestEventHandler {
public:
    void onSuiteStart(const SuiteInfo& suite) override {
        suites.push_back(suite.name);
    }

    void onTestStart(const SuiteInfo&, const TestInfo& test) override {
        tests.push_back(test.name);
    }

    std::vector<std::string> suites;
    std::vector<std::string> tests;
};

}

int main() {
    unsigned int selectedRuns{};
    unsigned int excludedRuns{};
    auto selectedSuite = Testament::Suite(
        "selected suite",
        Testament::Test("selected test", [&selectedRuns] { ++selectedRuns; }),
        Testament::Test("excluded test", [&excludedRuns] { ++excludedRuns; })
    );
    auto excludedSuite = Testament::Suite(
        "excluded suite",
        Testament::Test("selected test", [&excludedRuns] { ++excludedRuns; })
    );

    auto handler = std::make_unique<RecordingHandler>();
    auto* recording = handler.get();
    Testament::Runner runner;
    runner.addHandler(std::move(handler))
        .filterSuite("selected suite")
        .filterTest("selected test");

    return selectedSuite && excludedSuite
        && runner.run(0, nullptr) == 0
        && selectedRuns == 1
        && excludedRuns == 0
        && recording->suites == std::vector<std::string>{"selected suite"}
        && recording->tests == std::vector<std::string>{"selected test"}
        ? 0
        : 1;
}
