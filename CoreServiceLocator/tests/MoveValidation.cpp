#include "CoreServices/ServiceLocator.hpp"

#include <memory>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace {

class Service final : public Core::Services::IService {};

bool isMovedFromError(const std::logic_error& error) {
    return std::string_view{error.what()} == "ServiceLocator has been moved from";
}

}

int main() {
    Core::Services::ServiceLocator source;
    auto service = std::make_shared<Service>();
    source.registerService(service);

    Core::Services::ServiceLocator target(std::move(source));
    const bool serviceMoved = target.getService<Service>() == service;

    bool lookupRejected = false;
    try {
        static_cast<void>(source.getService<Service>());
    } catch (const std::logic_error& error) {
        lookupRejected = isMovedFromError(error);
    }

    bool registrationRejected = false;
    try {
        source.registerService(std::make_shared<Service>());
    } catch (const std::logic_error& error) {
        registrationRejected = isMovedFromError(error);
    }

    Core::Services::ServiceLocator assigned;
    assigned = std::move(target);
    return serviceMoved
        && lookupRejected
        && registrationRejected
        && assigned.getService<Service>() == service
        ? 0
        : 1;
}
