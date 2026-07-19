#ifndef CORE_SERVICELOCATOR_HPP
#define CORE_SERVICELOCATOR_HPP

#include <concepts>
#include <memory>
#include <typeindex>

namespace Core::Services {

/**
 * @brief Base interface for all services that can be registered with ServiceLocator.
 *
 * Any service that should be managed by ServiceLocator must inherit from this interface.
 */
class IService {
public:
    virtual ~IService() = default;
};

/**
 * @brief A thread-safe Service Locator pattern implementation.
 *
 * ServiceLocator registers, retrieves, and manages shared services within one locator instance.
 * It ensures **thread-safe** access using `std::shared_mutex`.
 */
class ServiceLocator {
public:
    /**
     * @brief Registers a new service instance.
     *
     * Specify an interface explicitly to register an implementation under that interface:
     * `registerService<ILogger>(std::make_shared<Logger>())`.
     *
     * @tparam T The lookup type, which must inherit from `IService`.
     * @param service A shared pointer to the service instance.
     * @throws std::invalid_argument If the service pointer is null.
     * @throws std::runtime_error If the service is already registered.
     */
    template<typename T>
    requires std::derived_from<T, IService>
    void registerService(std::shared_ptr<T> service);

    /**
     * @brief Unregisters a service instance.
     *
     * @tparam T The service type.
     * @throws std::runtime_error If the service is not found.
     */
    template<typename T>
    requires std::derived_from<T, IService>
    void unregisterService();

    /**
     * @brief Retrieves a registered service instance.
     *
     * @tparam T The service type.
     * @return A shared pointer to the requested service.
     * @throws std::runtime_error If the service is not found.
     */
    template<typename T>
    requires std::derived_from<T, IService>
    [[nodiscard]] std::shared_ptr<T> getService() const;

    /**
     * @brief Constructs an empty ServiceLocator.
     */
    ServiceLocator();

    /**
     * @brief Destroys the ServiceLocator and releases all registered services.
     */
    ~ServiceLocator();

    /**
     * @brief Move constructor.
     * @param other The ServiceLocator instance to move from.
     */
    ServiceLocator(ServiceLocator&& other) noexcept;

    /**
     * @brief Move assignment operator.
     * @param other The ServiceLocator instance to move from.
     * @return Reference to this instance.
     */
    ServiceLocator& operator=(ServiceLocator&& other) noexcept;

    /// @brief Copy constructor is deleted to prevent copying.
    ServiceLocator(const ServiceLocator&) = delete;

    /// @brief Copy assignment operator is deleted to prevent copying.
    ServiceLocator& operator=(const ServiceLocator&) = delete;

private:
    class Impl;

    /**
     * @brief Registers a service by its type index.
     * @param typeIndex A unique identifier for the service type.
     * @param service The shared pointer to the service instance.
     */
    void registerServiceImpl(std::type_index typeIndex, std::shared_ptr<IService> service);

    /**
     * @brief Unregisters a service by its type index.
     * @param typeIndex A unique identifier for the service type.
     */
    void unregisterServiceImpl(std::type_index typeIndex);

    /**
     * @brief Retrieves a service by its type index.
     * @param typeIndex A unique identifier for the service type.
     * @return A shared pointer to the requested service.
     */
    [[nodiscard]] std::shared_ptr<IService> getServiceImpl(std::type_index typeIndex) const;

    [[nodiscard]] Impl& implementation();
    [[nodiscard]] const Impl& implementation() const;

    /// @brief Internal implementation details, hidden via PImpl pattern.
    std::unique_ptr<Impl> pImpl;
};

}

#include "ServiceLocator.tpp"

#endif
