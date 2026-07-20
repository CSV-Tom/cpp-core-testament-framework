// A plain Test() without a fixture. See docs/guide.md for the full guide.
#include <Testament/Testament.hpp>

using namespace Testament;

namespace {

inline const auto normalTestSuite = Suite(
    "Normal Test Example",
    SuiteOptions{}.tag("example"),

    Test("addition", [] {
        Asserts::assertEquals(4, 2 + 2);
    }),

    Test("string concatenation", [] {
        Asserts::assertEquals(std::string{"foobar"}, std::string{"foo"} + "bar");
    })
);

}
