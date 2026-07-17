#include "InternalRegistry.hpp"

#include <algorithm>
#include <mutex>
#include <stdexcept>
#include <string>
#include <utility>

#include "InternalSuite.hpp"


namespace Testament {

InternalRegistry& InternalRegistry::getInstance() {
    static InternalRegistry instance;
    return instance;
}

void InternalRegistry::unregisterSuite(const std::shared_ptr<InternalSuite>& suite) {
    std::unique_lock lock(registryMutex);
    std::erase(registeredSuites, suite);
}

std::shared_ptr<InternalSuite> InternalRegistry::registerSuite(std::shared_ptr<InternalSuite> suite) {
    if (!suite) {
        throw std::invalid_argument("Cannot register a null Suite");
    }

    std::unique_lock lock(registryMutex);  // Schreibzugriff
    if (std::ranges::any_of(registeredSuites, [&suite](const auto& registered) {
        return registered->getName() == suite->getName();
    })) {
        throw std::logic_error("Suite name must be unique: " + suite->getName());
    }
    registeredSuites.push_back(std::move(suite));

    return registeredSuites.back();
}

std::vector<std::shared_ptr<InternalSuite>> InternalRegistry::getAllSuites() const {
    std::shared_lock lock(registryMutex);  // **Lesender Zugriff**
    return registeredSuites;
}

std::function<bool(const std::shared_ptr<InternalSuite>&)> InternalRegistry::createFilter(
    std::string_view type, std::string_view value
) {
    if (type == "name") {
        return [value = std::string{value}](const std::shared_ptr<InternalSuite>& suite) {
            return suite->getName() == value;
        };
    } else {
        throw std::invalid_argument("Unknown filter type");
    }
}

}
