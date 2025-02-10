#ifndef CORE_SERVICELOCATOR_HPP
#define CORE_SERVICELOCATOR_HPP

#include <memory>
#include <typeindex>

namespace Core::Services {


class IService {
public:
    virtual ~IService() = default;
    virtual std::type_index getTypeIndex() const noexcept = 0;
};

template<typename T>
class ServiceBase : public T {
public:
    std::type_index getTypeIndex() const noexcept override {
        return std::type_index(typeid(T));
    }
};

class ServiceLocator {
public:
    template<typename T>
    void registerService(std::shared_ptr<T> service);

    template<typename T>
    void unregisterService();

    template<typename T>
    [[nodiscard]] std::shared_ptr<T> getService() const;

    ServiceLocator();
    ~ServiceLocator();

    ServiceLocator(ServiceLocator&& other) noexcept;
    ServiceLocator& operator=(ServiceLocator&& other) noexcept;    

    ServiceLocator(const ServiceLocator&) = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;

private:
    void registerServiceImpl(std::type_index typeIndex, std::shared_ptr<IService> service);
    void unregisterServiceImpl(std::type_index typeIndex);
    [[nodiscard]] std::shared_ptr<IService> getServiceImpl(std::type_index typeIndex) const;    

    class Impl;
    std::unique_ptr<Impl> pImpl;
};

}

#include "ServiceLocator.tpp"

#endif
