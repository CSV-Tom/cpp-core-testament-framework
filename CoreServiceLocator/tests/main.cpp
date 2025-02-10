#include "CoreServices/ServiceLocator.hpp"
#include <iostream>


// StandardOutService: Writes messages to std::cout
class StandardOutService : public Core::Services::IService {
public:
    void logMessage(const std::string& message) {
        std::cout << "[StandardOut] " << message << std::endl;
    }

    std::type_index getTypeIndex() const noexcept override {
        return std::type_index(typeid(StandardOutService));
    }
};

// StandardErrorService: Writes messages to std::cerr
class StandardErrorService : public Core::Services::IService {
public:
    void logError(const std::string& errorMessage) {
        std::cerr << "[StandardError] " << errorMessage << std::endl;
    }

    std::type_index getTypeIndex() const noexcept override {
        return std::type_index(typeid(StandardErrorService));
    }
};


int main() {
    Core::Services::ServiceLocator locator;

    // Register services
    locator.registerService(std::make_shared<StandardOutService>());
    locator.registerService(std::make_shared<StandardErrorService>());

    // Retrieve and use StandardOutService
    auto outService = locator.getService<StandardOutService>();
    outService->logMessage("This is a test message.");

    // Retrieve and use StandardErrorService
    auto errService = locator.getService<StandardErrorService>();
    errService->logError("This is a test error.");

    // Unregister StandardOutService
    locator.unregisterService<StandardOutService>();

    try {
        // Attempt to retrieve a non-registered service
        auto missingService = locator.getService<StandardOutService>();
        missingService->logMessage("This should not print.");
    } catch (const std::runtime_error& e) {
        std::cerr << "[Exception Caught] " << e.what() << std::endl;
    }

    return 0;
}
