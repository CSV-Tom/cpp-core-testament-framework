#include "CoreServices/ServiceLocator.hpp"
#include <iostream>
#include <stdexcept>
#include <utility>


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
    bool duplicateServiceRejected = false;
    bool missingServiceRejected = false;
    bool unknownUnregistrationRejected = false;
    bool nullServiceRejected = false;

    // Register services
    locator.registerService(std::make_shared<StandardOutService>());
    locator.registerService(std::make_shared<StandardErrorService>());

    // Retrieve and use StandardOutService
    auto outService = locator.getService<StandardOutService>();
    outService->logMessage("This is a test message.");

    // Retrieve and use StandardErrorService
    auto errService = locator.getService<StandardErrorService>();
    errService->logError("This is a test error.");

    try {
        locator.registerService(std::make_shared<StandardErrorService>());
    } catch (const std::runtime_error& e) {
        duplicateServiceRejected = true;
        std::cerr << "[Exception Caught] " << e.what() << std::endl;
    }

    // Unregister StandardOutService
    locator.unregisterService<StandardOutService>();

    try {
        // Attempt to retrieve a non-registered service
        auto missingService = locator.getService<StandardOutService>();
        missingService->logMessage("This should not print.");
    } catch (const std::runtime_error& e) {
        missingServiceRejected = true;
        std::cerr << "[Exception Caught] " << e.what() << std::endl;
    }

    try {
        locator.unregisterService<StandardOutService>();
    } catch (const std::runtime_error& e) {
        unknownUnregistrationRejected = true;
        std::cerr << "[Exception Caught] " << e.what() << std::endl;
    }

    Core::Services::ServiceLocator movedLocator(std::move(locator));
    const bool movePreservedService =
        movedLocator.getService<StandardErrorService>() == errService;

    try {
        movedLocator.registerService(std::shared_ptr<StandardOutService>{});
    } catch (const std::invalid_argument& e) {
        nullServiceRejected = true;
        std::cerr << "[Exception Caught] " << e.what() << std::endl;
    }

    return duplicateServiceRejected
        && missingServiceRejected
        && unknownUnregistrationRejected
        && movePreservedService
        && nullServiceRejected
        ? 0
        : 1;
}
