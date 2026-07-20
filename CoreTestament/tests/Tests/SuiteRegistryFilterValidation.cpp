#include "runtime/SuiteRegistry.hpp"
#include "runtime/SuiteInstance.hpp"
#include "configuration/FilterMatcher.hpp"

#include <memory>
#include <string>
#include <vector>

int main() {
    auto& registry = Testament::detail::SuiteRegistry::instance();
    registry.registerSuite(std::make_shared<Testament::detail::SuiteInstance>("selected first"));
    registry.registerSuite(std::make_shared<Testament::detail::SuiteInstance>("ignored"));

    const auto selectedSuites = registry.suitesBy([](const auto& suite) {
        return suite->name().starts_with("selected");
    });

    registry.registerSuite(std::make_shared<Testament::detail::SuiteInstance>("selected later"));

    const std::vector<std::string> tags{"integration", "slow-network"};
    const bool patternsWork = Testament::detail::globMatches("selected first", "selected*")
        && Testament::detail::globMatches("test-7", "test-?")
        && Testament::detail::matchesNameFilter("kept", "-ignored*")
        && !Testament::detail::matchesNameFilter("ignored suite", "-ignored*")
        && Testament::detail::matchesTestFilter(
            "suite", tags, "test", {}, "tag:integ*"
        )
        && !Testament::detail::matchesTestFilter(
            "suite", tags, "test", {}, "-tag:integ*"
        );

    return selectedSuites.size() == 1
        && selectedSuites.front()->name() == "selected first"
        && registry.suites().size() == 3
        && patternsWork
        ? 0
        : 1;
}
