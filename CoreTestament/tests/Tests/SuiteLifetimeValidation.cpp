#include "Testament/Testament.hpp"

#include "core/Internal/InternalRegistry.hpp"

#include <utility>

int main() {
    auto& registry = Testament::InternalRegistry::instance();
    const auto initialSuiteCount = registry.suites().size();

    bool registeredWhileHandleLives = false;
    {
        auto suite = Testament::Suite(
            "lifetime suite",
            Testament::Test("lifetime test", [] {})
        );
        registeredWhileHandleLives = static_cast<bool>(suite)
            && registry.suites().size() == initialSuiteCount + 1;
    }

    return registeredWhileHandleLives
        && registry.suites().size() == initialSuiteCount
        ? 0
        : 1;
}
