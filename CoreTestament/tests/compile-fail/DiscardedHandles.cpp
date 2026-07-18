#include <Testament/Testament.hpp>

void discardTest() {
    Testament::Test("discarded test", [] {});
}

void discardSuite() {
    Testament::Suite("discarded suite");
}

void discardParameterizedTest() {
    Testament::ParameterizedTest(
        "discarded parameterized test",
        Testament::Cases(Testament::TestCase("case", 1)),
        [](int) {}
    );
}

void discardConfiguredRunnerResult() {
    Testament::Runner runner;
    runner.run(0, nullptr);
}

void discardDefaultRunnerResult() {
    Testament::run(0, nullptr);
}
