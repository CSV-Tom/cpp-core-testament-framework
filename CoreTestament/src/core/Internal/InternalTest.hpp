#ifndef TESTFRAMEWORK_TESTAMENT_INTERNALTEST_HPP
#define TESTFRAMEWORK_TESTAMENT_INTERNALTEST_HPP

#include <exception>
#include <expected>
#include <optional>
#include <source_location>
#include <string>
#include <typeindex>

#include "Testament/TestOptions.hpp"
#include "FunctionVariant.hpp"
#include "utils/ExecutionTimer.hpp"
#include "utils/TestStatus.hpp"

namespace Testament {

class InternalTest {
public:
    InternalTest(std::string name, FunctionVariant function);
    InternalTest(std::string name, std::source_location location, TestOptions options,
                 FunctionVariant function,
                 std::optional<std::type_index> fixtureType = std::nullopt);
    InternalTest(InternalTest&&) noexcept = default;
    InternalTest& operator=(InternalTest&&) noexcept = default;
    ~InternalTest();

    [[nodiscard]] std::expected<void, std::exception_ptr> execute(
        LifecycleSuite* fixture = nullptr
    );

    const std::string& name() const;
    const TestOptions& options() const;
    [[nodiscard]] std::source_location location() const noexcept;
    [[nodiscard]] std::optional<std::type_index> fixtureType() const noexcept;

    const ExecutionTimer& executionTimer() const;
    const TestStatus& status() const;
    std::exception_ptr exception() const;

private:
    std::string name_;
    std::source_location location_;
    TestOptions options_;
    FunctionVariant function;
    std::optional<std::type_index> fixtureType_;
    TestStatus status_{TestStatus::Status::NotRun};
    std::exception_ptr exception_;
    ExecutionTimer executionTimer_;
};

}

#endif
