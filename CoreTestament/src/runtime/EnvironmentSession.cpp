#include "EnvironmentSession.hpp"

#include "Testament/GlobalEnvironment.hpp"
#include "Testament/TestEventHandler.hpp"

#include <exception>

namespace Testament::detail {

EnvironmentSession::EnvironmentSession(
    const std::vector<std::unique_ptr<GlobalEnvironment>>& registeredEnvironments,
    TestEventHandler& eventHandler
) noexcept : mEnvironments(registeredEnvironments), mHandler(eventHandler) {}

EnvironmentSession::~EnvironmentSession() { (void)finish(); }

bool EnvironmentSession::start() noexcept {
    try {
        for (const auto& environment : mEnvironments) {
            environment->setUp();
            ++mActive;
        }
        return true;
    } catch (const std::exception& error) {
        mHandler.onEnvironmentError("setup", error.what());
    } catch (...) {
        mHandler.onEnvironmentError("setup", "unknown non-standard exception");
    }
    (void)finish();
    return false;
}

bool EnvironmentSession::finish() noexcept {
    while (mActive > 0) {
        try {
            mEnvironments[--mActive]->tearDown();
        } catch (const std::exception& error) {
            mHandler.onEnvironmentError("teardown", error.what());
            mSucceeded = false;
        } catch (...) {
            mHandler.onEnvironmentError("teardown", "unknown non-standard exception");
            mSucceeded = false;
        }
    }
    return mSucceeded;
}

}
