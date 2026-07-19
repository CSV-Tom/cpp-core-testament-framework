#include "Testament/Testament.hpp"

#include "core/Internal/InternalRegistry.hpp"

int main() {
    auto& registry = Testament::InternalRegistry::getInstance();
    if (!registry.getConfigurationErrors().empty()) return 1;

    bool errorLivesWithInvalidRegistration = false;
    {
        auto first = Testament::Suite("temporary duplicate", Testament::Test("first", [] {}));
        {
            auto duplicate = Testament::Suite(
                "temporary duplicate", Testament::Test("second", [] {})
            );
            errorLivesWithInvalidRegistration = first
                && !duplicate
                && registry.getConfigurationErrors().size() == 1;
        }
        if (!registry.getConfigurationErrors().empty()) return 1;
    }

    auto valid = Testament::Suite("valid after error", Testament::Test("passes", [] {}));
    Testament::Runner runner;
    return errorLivesWithInvalidRegistration
        && valid
        && registry.getConfigurationErrors().empty()
        && runner.run(0, nullptr) == 0
        ? 0
        : 1;
}
