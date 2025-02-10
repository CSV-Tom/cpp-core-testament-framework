#include "InternalRegistry.hpp"

#include <mutex>

#include "InternalSuite.hpp"


namespace Testament {

InternalRegistry& InternalRegistry::getInstance() {
    static InternalRegistry instance;
    return instance;
}

std::shared_ptr<InternalSuite> InternalRegistry::registerSuite(std::shared_ptr<InternalSuite> suite) {
    if (!suite) {
        throw std::invalid_argument("Cannot register a null Suite");
    }

    std::unique_lock lock(registryMutex);  // Schreibzugriff
    registeredSuites.push_back(std::move(suite));

    return registeredSuites.back();
}

std::vector<std::shared_ptr<InternalSuite>> InternalRegistry::getAllSuites() const {
    std::shared_lock lock(registryMutex);  // **Lesender Zugriff**
    return registeredSuites;
}

std::function<bool(const std::shared_ptr<InternalSuite>&)> InternalRegistry::createFilter(const std::string& type, const std::string& value) {
   /* if (type == "test_count") {
        return [](const std::shared_ptr<Suite>& suite) {
            return suite->getTestCount() > 5;
        };
    } else*/ if (type == "name") {
        return [value](const std::shared_ptr<InternalSuite>& suite) {
            return suite->getName() == value;
        };
    } else {
        throw std::invalid_argument("Unknown filter type");
    }
}

}
