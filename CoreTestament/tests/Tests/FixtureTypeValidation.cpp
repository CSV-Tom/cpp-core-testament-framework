#include "Testament/Testament.hpp"

namespace {

class ExpectedFixture : public Testament::LifecycleSuite {};
class ActualFixture : public Testament::LifecycleSuite {};

}

int main() {
    bool testBodyEntered = false;
    auto mismatchedTest = Testament::makeTest<ExpectedFixture>(
        "mismatched fixture",
        [&testBodyEntered](ExpectedFixture&) {
            testBodyEntered = true;
        }
    );
    static_cast<void>(Testament::makeSuite<ActualFixture>(
        "fixture type validation",
        mismatchedTest
    ));

    const int runnerResult = Testament::Runner::run(0, nullptr);
    return runnerResult == 1 && !testBodyEntered ? 0 : 1;
}
