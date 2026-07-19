#include <CoreServices/ServiceLocator.hpp>

#include <expected>
#include <memory>

namespace {

class Service final : public Core::Services::IService {
public:
    explicit Service(int value_) : value(value_) {}
    int value;
};

}

int main() {
    const std::expected<int, int> cxx23Value{23};
    Core::Services::ServiceLocator locator;
    auto service = std::make_shared<Service>(42);
    locator.registerService(service);

    return cxx23Value.value() == 23
        && locator.getService<Service>() == service
        && locator.getService<Service>()->value == 42
        ? 0
        : 1;
}
