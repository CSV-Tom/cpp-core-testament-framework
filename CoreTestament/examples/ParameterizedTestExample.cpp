// ParameterizedTest with multiple TestCase value types. See docs/guide.md for the full guide.
#include <Testament/Testament.hpp>

using namespace Testament;

namespace {

inline const auto parameterizedSuite = Suite(
    "Parameterized Test Example",
    SuiteOptions{}.tag("example"),

    ParameterizedTest(
        "addition",
        Cases(
            TestCase("1 + 1 = 2", 1, 1, 2),
            TestCase("2 + 3 = 5", 2, 3, 5),
            TestCase("-1 + 1 = 0", -1, 1, 0)
        ),
        [](int left, int right, int expected) {
            Asserts::assertEquals(expected, left + right);
        }
    ),

    ParameterizedTest(
        "reject invalid labels",
        Cases(
            TestCase("empty label", std::string{}),
            TestCase("label with spaces", std::string{"has space"})
        ),
        [](const std::string& label) {
            Asserts::assertTrue(label.empty() || label.contains(' '));
        }
    )
);

}
