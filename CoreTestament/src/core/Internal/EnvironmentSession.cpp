#include "EnvironmentSession.hpp"

#include "Testament/GlobalEnvironment.hpp"
#include "Testament/TestEventHandler.hpp"

#include <exception>

namespace Testament::detail {

EnvironmentSession::EnvironmentSession(
    const std::vector<std::unique_ptr<GlobalEnvironment>>& registeredEnvironments,
    TestEventHandler& eventHandler
) noexcept : environments(registeredEnvironments), handler(eventHandler) {}

EnvironmentSession::~EnvironmentSession() { (void)finish(); }

bool EnvironmentSession::start() noexcept {
    try {
        for (const auto& environment : environments) {
            environment->setUp();
            ++active;
        }
        return true;
    } catch (const std::exception& error) {
        handler.onEnvironmentError("setup", error.what());
    } catch (...) {
        handler.onEnvironmentError("setup", "unknown non-standard exception");
    }
    (void)finish();
    return false;
}

bool EnvironmentSession::finish() noexcept {
    while (active > 0) {
        try {
            environments[--active]->tearDown();
        } catch (const std::exception& error) {
            handler.onEnvironmentError("teardown", error.what());
            succeeded = false;
        } catch (...) {
            handler.onEnvironmentError("teardown", "unknown non-standard exception");
            succeeded = false;
        }
    }
    return succeeded;
}

}
