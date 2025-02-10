#ifndef CORE_SERVICELOCATOR_TPP
#define CORE_SERVICELOCATOR_TPP

#include "CoreServices/ServiceLocator.hpp"

template<typename T>
void ServiceLocator::registerService(std::shared_ptr<T> service) {
    static_assert(std::is_base_of_v<IService, T>, "T must inherit from IService");
    registerServiceImpl(service->getTypeIndex(), std::move(service));
}

template<typename T>
void ServiceLocator::unregisterService() {
    unregisterServiceImpl(std::type_index(typeid(T)));
}

template<typename T>
[[nodiscard]] std::shared_ptr<T> ServiceLocator::getService() const {
    return std::static_pointer_cast<T>(getServiceImpl(std::type_index(typeid(T))));
}

#endif 
