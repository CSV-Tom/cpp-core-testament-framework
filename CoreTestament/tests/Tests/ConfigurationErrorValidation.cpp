#include "Testament/Testament.hpp"

#include "core/Internal/InternalRegistry.hpp"

#include <algorithm>
#include <ranges>
#include <string>

namespace {

inline bool executed = false;

inline const auto invalidSuite = Testament::Suite(
    "invalid atomic suite",
    Testament::Test("duplicate", [] { executed = true; }),
    Testament::Test("duplicate", [] { executed = true; })
);

inline const auto validSuite = Testament::Suite(
    "duplicate suite name",
    Testament::Test("would run without configuration errors", [] { executed = true; })
);

inline const auto duplicateSuite = Testament::Suite(
    "duplicate suite name",
    Testament::Test("duplicate", [] { executed = true; })
);

}

int main() {
    const auto& registry = Testament::InternalRegistry::getInstance();
    const auto errors = registry.getConfigurationErrors();
    const bool locationsReported = std::ranges::all_of(errors, [](const auto& error) {
        return error.contains("ConfigurationErrorValidation.cpp:");
    });
    return !invalidSuite
        && validSuite
        && !duplicateSuite
        && registry.getAllSuites().size() == 1
        && !errors.empty()
        && locationsReported
        && Testament::run(0, nullptr) == 2
        && !executed
        ? 0
        : 1;
}
