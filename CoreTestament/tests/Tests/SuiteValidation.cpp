#include "Testament/Testament.hpp"

namespace Testament {

/// Suite for validating fundamental test logic.
static auto suite = makeSuite("Fundamental Test Suite",
makeTest("Equality Test", []() {
    const int expectedValue = 42;
    const int computedValue = 42;
    Asserts::assertEquals(expectedValue, computedValue);
}),
makeTest("Boolean Condition Test", []() {
    const bool conditionMet = true;
    Asserts::assertTrue(conditionMet, "condition should be true");
})
                             );

}
