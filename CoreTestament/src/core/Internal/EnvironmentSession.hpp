#ifndef TESTAMENT_INTERNAL_ENVIRONMENTSESSION_HPP
#define TESTAMENT_INTERNAL_ENVIRONMENTSESSION_HPP

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
                       TestEventHandler& handler) noexcept;
    ~EnvironmentSession();

    EnvironmentSession(const EnvironmentSession&) = delete;
    EnvironmentSession& operator=(const EnvironmentSession&) = delete;

    [[nodiscard]] bool start() noexcept;
    [[nodiscard]] bool finish() noexcept;

private:
    const std::vector<std::unique_ptr<GlobalEnvironment>>& environments;
    TestEventHandler& handler;
    std::size_t active{};
    bool succeeded{true};
};

}

#endif
