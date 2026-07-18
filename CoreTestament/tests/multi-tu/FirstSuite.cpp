#include "Testament/Testament.hpp"

namespace {

inline const auto firstSuite = Testament::Suite(
    "first translation unit",
    Testament::Test("first test", [] {})
);

}
