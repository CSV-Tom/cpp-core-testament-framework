#include <Testament/Testament.hpp>

void discardTest() {
    Testament::makeTest("discarded test", [] {});
}

void discardSuite() {
    Testament::makeSuite("discarded suite");
}

void discardConfiguredRunnerResult() {
    Testament::Runner runner;
    runner.run(0, nullptr);
}

void discardDefaultRunnerResult() {
    Testament::run(0, nullptr);
}
