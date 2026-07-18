#include "Testament/Testament.hpp"

namespace Testament {

/// Suite for validating fundamental test logic.
inline const auto suite = Suite(
    "Fundamental Test Suite",
    Test("Equality Test", [] {
        Asserts::assertEquals(42, 42);
    }),
    Test("Boolean Condition Test", [] {
        Asserts::assertTrue(true, "condition should be true");
    })
);

}
