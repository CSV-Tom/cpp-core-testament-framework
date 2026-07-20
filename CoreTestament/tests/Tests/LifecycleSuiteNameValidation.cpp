#include "Testament/Testament.hpp"

#include "runtime/SuiteRegistry.hpp"

namespace {

class EmptyNameLifecycleSuite : public Testament::LifecycleSuite {};

inline const auto invalidSuite = Testament::Suite<EmptyNameLifecycleSuite>(
    "",
    Testament::Test("must not register", [](EmptyNameLifecycleSuite&) {})
);

}

int main() {
    return !invalidSuite
        && Testament::detail::SuiteRegistry::instance().suites().empty()
        && Testament::run(0, nullptr) == 2
        ? 0
        : 1;
}
