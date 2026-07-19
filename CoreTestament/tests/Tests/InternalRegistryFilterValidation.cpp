#include "core/Internal/InternalRegistry.hpp"
#include "core/Internal/InternalSuite.hpp"
#include "core/Internal/FilterPattern.hpp"

#include <memory>
#include <string>
#include <vector>

int main() {
    auto& registry = Testament::InternalRegistry::getInstance();
    registry.registerSuite(std::make_shared<Testament::InternalSuite>("selected first"));
    registry.registerSuite(std::make_shared<Testament::InternalSuite>("ignored"));

    const auto selectedSuites = registry.getSuitesByFilter([](const auto& suite) {
        return suite->getName().starts_with("selected");
    });

    registry.registerSuite(std::make_shared<Testament::InternalSuite>("selected later"));

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
        && selectedSuites.front()->getName() == "selected first"
        && registry.getAllSuites().size() == 3
        && patternsWork
        ? 0
        : 1;
}
