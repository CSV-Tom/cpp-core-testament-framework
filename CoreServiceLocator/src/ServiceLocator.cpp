#include "CoreServices/ServiceLocator.hpp"

#include <unordered_map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>
#include <utility>

namespace Core::Services {

class ServiceLocator::Impl {
private:
    std::unordered_map<std::type_index, std::shared_ptr<IService>> services;
    mutable std::shared_mutex mutex; // Ermöglicht paralleles Lesen

public:
    void registerServiceImpl(std::type_index typeIndex, std::shared_ptr<IService> service) {
        if (!service) {
            throw std::invalid_argument("Cannot register a null service");
        }

        std::unique_lock lock(mutex); // Exklusive Sperre für Schreibzugriff
        auto [it, inserted] = services.try_emplace(typeIndex, service);
        if (!inserted) {
            throw std::runtime_error("Service already registered");
        }
    }

    void unregisterServiceImpl(std::type_index typeIndex) {
        std::unique_lock lock(mutex); // Exklusive Sperre für Schreibzugriff
        if (services.erase(typeIndex) == 0) {
            throw std::runtime_error("Service not registered");
        }
    }

    [[nodiscard]] std::shared_ptr<IService> getServiceImpl(std::type_index typeIndex) const {
        std::shared_lock lock(mutex); // Shared Lock: Ermöglicht paralleles Lesen
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


ServiceLocator::ServiceLocator(ServiceLocator&& other) noexcept = default;
ServiceLocator& ServiceLocator::operator=(ServiceLocator&& other) noexcept = default;
ServiceLocator::ServiceLocator() : pImpl(std::make_unique<Impl>()) {}
ServiceLocator::~ServiceLocator() = default;

}
