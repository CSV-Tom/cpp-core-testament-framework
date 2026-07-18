#include "Testament/Testament.hpp"

namespace Testament {

inline const auto suite = Suite(
    "Parameterized Suite",
    ParameterizedTest(
        "Division Tests",
        TestOptions{}.tag("parameterized"),
        Cases(
            TestCase("10 divided by 2", 10, 2, 5),
            TestCase("20 divided by 4", 20, 4, 5),
            TestCase("30 divided by 6", 30, 6, 5)
        ),
        [](int dividend, int divisor, int expected) {
            Asserts::assertEquals(expected, dividend / divisor);
        }
    )
);

}
