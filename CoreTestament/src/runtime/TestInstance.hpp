#pragma once

#include <exception>
#include <expected>
#include <optional>
#include <source_location>
#include <string>
#include <typeindex>

#include "Testament/TestOptions.hpp"
#include "TestFunction.hpp"
#include "Stopwatch.hpp"
#include "TestState.hpp"

namespace Testament::detail {

class TestInstance {
public:
    using Result = std::expected<void, std::exception_ptr>;

    TestInstance(std::string name, TestFunction mFunction);
    TestInstance(std::string name, std::source_location location, TestOptions options,
                 TestFunction mFunction,
                 std::optional<std::type_index> fixtureType = std::nullopt);
    TestInstance(TestInstance&&) noexcept = default;
    TestInstance& operator=(TestInstance&&) noexcept = default;
    ~TestInstance();

    [[nodiscard]] Result execute(LifecycleSuite* fixture = nullptr);

    const std::string& name() const;
    const TestOptions& options() const;
    [[nodiscard]] std::source_location location() const noexcept;
    [[nodiscard]] std::optional<std::type_index> fixtureType() const noexcept;

    const Stopwatch& executionTimer() const;
    const TestState& status() const;
    std::exception_ptr exception() const;

private:
    std::string mName;
    std::source_location mLocation;
    TestOptions mOptions;
    TestFunction mFunction;
    std::optional<std::type_index> mFixtureType;
    TestState mStatus{TestState::Status::NotRun};
    std::exception_ptr mException;
    Stopwatch mStopwatch;
};

}
