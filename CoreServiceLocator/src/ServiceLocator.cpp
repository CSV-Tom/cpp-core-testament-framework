
#include "CoreServices/ServiceLocator.hpp"

ServiceLocator::ServiceLocator() = default;

ServiceLocator::~ServiceLocator() = default;


void ServiceLocator::registerServiceImpl(std::type_index typeIndex, std::shared_ptr<IService> service) {
    std::scoped_lock lock(mutex);

    if (services.contains(typeIndex)) {
        throw std::runtime_error("Service already registered");
    }

    services[typeIndex] = std::move(service);
}

void ServiceLocator::unregisterServiceImpl(std::type_index typeIndex) {
    std::scoped_lock lock(mutex);

    if (!services.contains(typeIndex)) {
        throw std::runtime_error("Service not registered");
    }

    services.erase(typeIndex);
}

std::shared_ptr<IService> ServiceLocator::getServiceImpl(std::type_index typeIndex) const {
    std::scoped_lock lock(mutex);

    auto it = services.find(typeIndex);
    if (it == services.end()) {
        throw std::runtime_error("Service not found");
    }

    return it->second;
}