#pragma once

#include <cstddef>
#include <memory>
#include <vector>

namespace Testament {
class GlobalEnvironment;
class TestEventHandler;
}

namespace Testament::detail {

class EnvironmentSession {
public:
    EnvironmentSession(const std::vector<std::unique_ptr<GlobalEnvironment>>& environments,
                       TestEventHandler& eventHandler) noexcept;
    ~EnvironmentSession();

    EnvironmentSession(const EnvironmentSession&) = delete;
    EnvironmentSession& operator=(const EnvironmentSession&) = delete;

    [[nodiscard]] bool start() noexcept;
    [[nodiscard]] bool finish() noexcept;

private:
    const std::vector<std::unique_ptr<GlobalEnvironment>>& mEnvironments;
    TestEventHandler& mHandler;
    std::size_t mActive{};
    bool mSucceeded{true};
};

}
