#include "Testament/Testament.hpp"

#include "runtime/SuiteRegistry.hpp"

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
    const auto& registry = Testament::detail::SuiteRegistry::instance();
    const auto errors = registry.configurationErrors();
    const bool locationsReported = std::ranges::all_of(errors, [](const auto& error) {
        return error.contains("ConfigurationErrorValidation.cpp:");
    });
    return !invalidSuite
        && validSuite
        && !duplicateSuite
        && registry.suites().size() == 1
        && !errors.empty()
        && locationsReported
        && Testament::run(0, nullptr) == 2
        && !executed
        ? 0
        : 1;
}
