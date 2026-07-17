#include <Testament/Testament.hpp>

void discardTest() {
    Testament::makeTest("discarded test", [] {});
}

void discardSuite() {
    Testament::makeSuite("discarded suite");
}
