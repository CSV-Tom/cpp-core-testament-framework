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
        Testament::SuiteOptions{}.tag("integration"),
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
        .filterSuite("selected*")
        .filterTest("selected ?est");

    const auto filteredRun = runner.run(0, nullptr);

    Testament::Runner cliRunner;
    std::string executable{"RunnerFilterValidation"};
    std::string filter{"--filter=tag:integ*"};
    char* arguments[]{executable.data(), filter.data()};
    const auto cliRun = cliRunner.run(2, arguments);

    return selectedSuite && excludedSuite
        && filteredRun == 0
        && cliRun == 0
        && selectedRuns == 2
        && excludedRuns == 1
        && recording->suites == std::vector<std::string>{"selected suite"}
        && recording->tests == std::vector<std::string>{"selected test"}
        ? 0
        : 1;
}
