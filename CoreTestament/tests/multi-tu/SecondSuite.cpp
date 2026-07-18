#include "Testament/Testament.hpp"

namespace {

inline const auto secondSuite = Testament::Suite(
    "second translation unit",
    Testament::Test("second test", [] {})
);

}
