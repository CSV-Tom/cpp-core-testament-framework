#include <Testament/Testament.hpp>

inline const auto invalidCases = Testament::Cases(
    Testament::TestCase("integer", 1),
    Testament::TestCase("text", "value")
);
