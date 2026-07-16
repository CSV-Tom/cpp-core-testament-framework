#include "core/Internal/InternalRegistry.hpp"
#include "core/Internal/InternalSuite.hpp"

#include <memory>
#include <string>

int main() {
    auto& registry = Testament::InternalRegistry::getInstance();
    registry.registerSuite(std::make_shared<Testament::InternalSuite>("selected first"));
    registry.registerSuite(std::make_shared<Testament::InternalSuite>("ignored"));

    const auto selectedSuites = registry.getSuitesByFilter([](const auto& suite) {
        return suite->getName().starts_with("selected");
    });

    registry.registerSuite(std::make_shared<Testament::InternalSuite>("selected later"));

    return selectedSuites.size() == 1
        && selectedSuites.front()->getName() == "selected first"
        && registry.getAllSuites().size() == 3
        ? 0
        : 1;
}
