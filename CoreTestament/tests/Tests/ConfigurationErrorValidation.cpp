#include "Testament/Testament.hpp"

#include "core/Internal/InternalRegistry.hpp"

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
    return !invalidSuite
        && validSuite
        && !duplicateSuite
        && registry.getAllSuites().size() == 1
        && !registry.getConfigurationErrors().empty()
        && Testament::run(0, nullptr) == 2
        && !executed
        ? 0
        : 1;
}
