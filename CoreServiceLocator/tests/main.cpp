#include "CoreServices/ServiceLocator.hpp"
#include <iostream>

class TestService : public IService {
public:
    void sayHello() {
        std::cout << "Hello from TestService!" << std::endl;
    }

    std::type_index getTypeIndex() const noexcept override {
        return std::type_index(typeid(TestService));
    }
};

int main() {
    ServiceLocator locator;

    auto service = std::make_shared<TestService>();
    locator.registerService(service);

    auto retrievedService = locator.getService<TestService>();
    retrievedService->sayHello();

    locator.unregisterService<TestService>();

    return 0;
}
