#include <Testament/Testament.hpp>

#include <string>

inline const auto invalidSuite = Testament::Suite(
    "invalid parameters",
    Testament::ParameterizedTest(
        "values",
        Testament::Cases(Testament::TestCase("integer", 1)),
        [](const std::string&) {}
    )
);
