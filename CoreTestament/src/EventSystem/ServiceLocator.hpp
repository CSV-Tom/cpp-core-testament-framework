#ifndef SERVICE_LOCATOR_HPP
#define SERVICE_LOCATOR_HPP

#include <unordered_map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <typeindex>
#include <type_traits>

/// Base class for all services.
class IService {
public:
    virtual ~IService() = default;
    /// Returns the base type index of the service.
    virtual std::type_index getTypeIndex() const noexcept = 0;
};

/// Specific service base class.
template<typename T>
class ServiceBase : public T {
public:
    std::type_index getTypeIndex() const noexcept override {
        return std::type_index(typeid(T));
    }
};

/// Service Locator class.
class ServiceLocator {
private:
    std::unordered_map<std::type_index, std::shared_ptr<IService>> services;
    mutable std::mutex mutex;

public:
    /// Registers a service with the service locator.
    template<typename T>
    void registerService(std::shared_ptr<T> service) {
        static_assert(std::is_base_of_v<IService, T>, "T must inherit from IService");
        std::scoped_lock lock(mutex);
        std::type_index typeIndex = service->getTypeIndex();
        if (services.contains(typeIndex)) {
            throw std::runtime_error("Service already registered");
        }
        services[typeIndex] = std::move(service);
    }

    /// Unregisters a service from the service locator.
    template<typename T>
    void unregisterService() {
        std::scoped_lock lock(mutex);
        std::type_index typeIndex = std::type_index(typeid(T));
        if (!services.contains(typeIndex)) {
            throw std::runtime_error("Service not registered");
        }
        services.erase(typeIndex);
    }

    /// Gets a service from the service locator.
    template<typename T>
    [[nodiscard]] std::shared_ptr<T> getService() const {
        std::scoped_lock lock(mutex);
        std::type_index typeIndex = std::type_index(typeid(T));
        auto it = services.find(typeIndex);
        if (it == services.end()) {
            throw std::runtime_error("Service not found");
        }
        return std::static_pointer_cast<T>(it->second);
    }
};

#endif
