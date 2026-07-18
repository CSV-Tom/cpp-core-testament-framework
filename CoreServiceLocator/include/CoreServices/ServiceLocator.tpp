#ifndef CORE_SERVICELOCATOR_TPP
#define CORE_SERVICELOCATOR_TPP

#include "CoreServices/ServiceLocator.hpp"

#include <utility>

namespace Core::Services {

template<typename T>
requires std::derived_from<T, IService>
void ServiceLocator::registerService(std::shared_ptr<T> service) {
    registerServiceImpl(std::type_index(typeid(T)), std::move(service));
}

template<typename T>
requires std::derived_from<T, IService>
void ServiceLocator::unregisterService() {
    unregisterServiceImpl(std::type_index(typeid(T)));
}

template<typename T>
requires std::derived_from<T, IService>
[[nodiscard]] std::shared_ptr<T> ServiceLocator::getService() const {
    return std::static_pointer_cast<T>(getServiceImpl(std::type_index(typeid(T))));
}

}

#endif
