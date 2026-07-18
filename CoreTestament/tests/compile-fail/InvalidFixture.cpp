#include <Testament/Testament.hpp>

class ExpectedFixture : public Testament::LifecycleSuite {};
class WrongFixture : public Testament::LifecycleSuite {};

inline const auto invalidSuite = Testament::Suite<ExpectedFixture>(
    "invalid fixture",
    Testament::Test("wrong", [](WrongFixture&) {})
);
