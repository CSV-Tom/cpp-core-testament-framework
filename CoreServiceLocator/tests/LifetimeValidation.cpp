#include "CoreServices/ServiceLocator.hpp"

#include <memory>

namespace {

class Service final : public Core::Services::IService {};

}

int main() {
    Core::Services::ServiceLocator locator;
    auto service = std::make_shared<Service>();
    std::weak_ptr<Service> lifetime = service;
    locator.registerService(service);

    auto consumer = locator.getService<Service>();
    service.reset();
    locator.unregisterService<Service>();
    const bool consumerKeepsServiceAlive = !lifetime.expired() && consumer.use_count() == 1;

    consumer.reset();
    return consumerKeepsServiceAlive && lifetime.expired() ? 0 : 1;
}
