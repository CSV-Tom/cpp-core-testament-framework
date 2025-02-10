
#include "CoreServices/ServiceLocator.hpp"

#include <unordered_map>
#include <memory>
#include <mutex>

/**
 * @brief Internal implementation class for ServiceLocator.
 */
class ServiceLocator::Impl {
private:
    std::unordered_map<std::type_index, std::shared_ptr<IService>> services;
    mutable std::mutex mutex;

public:
    void registerServiceImpl(std::type_index typeIndex, std::shared_ptr<IService> service) {
        std::scoped_lock lock(mutex);

        if (services.contains(typeIndex)) {
            throw std::runtime_error("Service already registered");
        }

        services[typeIndex] = std::move(service);
    }

    void unregisterServiceImpl(std::type_index typeIndex) {
        std::scoped_lock lock(mutex);

        if (!services.contains(typeIndex)) {
            throw std::runtime_error("Service not registered");
        }

        services.erase(typeIndex);
    }

    [[nodiscard]] std::shared_ptr<IService> getServiceImpl(std::type_index typeIndex) const {
        std::scoped_lock lock(mutex);

        auto it = services.find(typeIndex);
        if (it == services.end()) {
            throw std::runtime_error("Service not found");
        }

        return it->second;
    }
};

void ServiceLocator::registerServiceImpl(std::type_index typeIndex, std::shared_ptr<IService> service) {
    pImpl->registerServiceImpl(typeIndex, std::move(service));
}

void ServiceLocator::unregisterServiceImpl(std::type_index typeIndex) {
    pImpl->unregisterServiceImpl(typeIndex);
}

[[nodiscard]] std::shared_ptr<IService> ServiceLocator::getServiceImpl(std::type_index typeIndex) const {
    return pImpl->getServiceImpl(typeIndex);
}

ServiceLocator::ServiceLocator() : pImpl(std::make_unique<Impl>()) {}

ServiceLocator::~ServiceLocator() = default;

/*
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
}*/