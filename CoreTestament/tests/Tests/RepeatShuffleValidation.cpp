#include "Testament/Testament.hpp"

#include <iostream>
#include <memory>
#include <sstream>
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
    unsigned int executions{};
    auto unordered = Testament::Suite(
        "unordered suite",
        Testament::Test("unordered test", [&executions] { ++executions; })
    );
    auto ordered = Testament::Suite(
        "ordered suite",
        Testament::SuiteOptions{}.order(-10),
        Testament::Test("late test", [&executions] { ++executions; }),
        Testament::Test(
            "ordered test", Testament::TestOptions{}.order(-10),
            [&executions] { ++executions; }
        )
    );

    auto handler = std::make_unique<RecordingHandler>();
    auto* recording = handler.get();
    Testament::Runner runner;
    runner.addHandler(std::move(handler));

    std::string executable{"RepeatShuffleValidation"};
    std::string repeat{"--repeat=3"};
    std::string shuffle{"--shuffle"};
    std::string seed{"--seed=42"};
    char* arguments[]{executable.data(), repeat.data(), shuffle.data(), seed.data()};
    std::ostringstream output;
    auto* previous = std::cout.rdbuf(output.rdbuf());
    const auto exitCode = runner.run(4, arguments);
    std::cout.rdbuf(previous);

    return unordered && ordered
        && exitCode == 0
        && executions == 9
        && recording->suites.size() == 6
        && recording->suites[0] == "ordered suite"
        && recording->suites[2] == "ordered suite"
        && recording->suites[4] == "ordered suite"
        && recording->tests[0] == "ordered test"
        && recording->tests[3] == "ordered test"
        && recording->tests[6] == "ordered test"
        && output.str().contains("[SEED] 42")
        && output.str().contains("[SEED] 43")
        && output.str().contains("[SEED] 44")
        ? 0
        : 1;
}
