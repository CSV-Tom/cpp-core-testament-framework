#ifndef CORE_SERVICELOCATOR_HPP
#define CORE_SERVICELOCATOR_HPP

#include <memory>
#include <typeindex>
/**
 * @brief Base interface for all services.
 */
class IService {
public:
    virtual ~IService() = default;

    /**
     * @brief Retrieves the type index of the service.
     * @return The std::type_index of the derived service.
     */
    virtual std::type_index getTypeIndex() const noexcept = 0;
};

/**
 * @brief A helper template that provides type-safe service registration.
 * 
 * @tparam T The derived service type.
 */
template<typename T>
class ServiceBase : public T {
public:
    std::type_index getTypeIndex() const noexcept override {
        return std::type_index(typeid(T));
    }
};

/**
 * @brief A thread-safe Service Locator implementation.
 */
class ServiceLocator {
private:
    class Impl; 
    std::unique_ptr<Impl> pImpl;

    void registerServiceImpl(std::type_index typeIndex, std::shared_ptr<IService> service);
    void unregisterServiceImpl(std::type_index typeIndex);
   [[nodiscard]] std::shared_ptr<IService> getServiceImpl(std::type_index typeIndex) const;

public:
    ServiceLocator();
    ~ServiceLocator();

    /**
     * @brief Registers a service instance.
     * 
     * @tparam T The service type.
     * @param service The shared pointer to the service instance.
     * @throws std::runtime_error If the service is already registered.
     */
    template<typename T>
    void registerService(std::shared_ptr<T> service);

    /**
     * @brief Unregisters a service instance.
     * 
     * @tparam T The service type.
     * @throws std::runtime_error If the service is not found.
     */
    template<typename T>
    void unregisterService();

    /**
     * @brief Retrieves a service instance.
     * 
     * @tparam T The service type.
     * @return A shared pointer to the requested service.
     * @throws std::runtime_error If the service is not found.
     */
    template<typename T>
    [[nodiscard]] std::shared_ptr<T> getService() const;


};

#include "ServiceLocator.tpp"

#endif
