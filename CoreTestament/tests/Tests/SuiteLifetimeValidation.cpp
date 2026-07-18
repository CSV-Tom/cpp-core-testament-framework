#include "Testament/Testament.hpp"

#include "core/Internal/InternalRegistry.hpp"

#include <utility>

int main() {
    auto& registry = Testament::InternalRegistry::getInstance();
    const auto initialSuiteCount = registry.getAllSuites().size();

    bool registeredWhileHandleLives = false;
    {
        auto suite = Testament::Suite(
            "lifetime suite",
            Testament::Test("lifetime test", [] {})
        );
        registeredWhileHandleLives = static_cast<bool>(suite)
            && registry.getAllSuites().size() == initialSuiteCount + 1;
    }

    return registeredWhileHandleLives
        && registry.getAllSuites().size() == initialSuiteCount
        ? 0
        : 1;
}
