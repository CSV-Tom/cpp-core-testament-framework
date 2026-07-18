#include "Testament/Testament.hpp"

#include "core/Internal/InternalRegistry.hpp"

namespace {

class EmptyNameLifecycleSuite : public Testament::LifecycleSuite {};

inline const auto invalidSuite = Testament::Suite<EmptyNameLifecycleSuite>(
    "",
    Testament::Test("must not register", [](EmptyNameLifecycleSuite&) {})
);

}

int main() {
    return !invalidSuite
        && Testament::InternalRegistry::getInstance().getAllSuites().empty()
        && Testament::run(0, nullptr) == 2
        ? 0
        : 1;
}
