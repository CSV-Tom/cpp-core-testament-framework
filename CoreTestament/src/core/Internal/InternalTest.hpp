
#ifndef TESTFRAMEWORK_TESTAMENT_INTERNALTEST_HPP
#define TESTFRAMEWORK_TESTAMENT_INTERNALTEST_HPP

#include <exception>
#include <optional>
#include <string>
#include <typeindex>
#include <variant>

#include "Testament/Options.hpp"
#include "FunctionVariant.hpp"


#include "utils/TestStatus.hpp"
#include "utils/ExecutionTimer.hpp"


namespace Testament {

class InternalTest {
public:
    InternalTest(std::string name, FunctionVariant function);
    InternalTest(std::string name, TestOptions options, FunctionVariant function,
                 std::optional<std::type_index> fixtureType = std::nullopt);
    InternalTest(InternalTest&&) noexcept = default;
    InternalTest& operator=(InternalTest&&) noexcept = default;
    ~InternalTest();

    std::variant<std::monostate, std::exception_ptr> execute(LifecycleSuite* fixture = nullptr);

    const std::string& getName() const;
    const TestOptions& getOptions() const;
    [[nodiscard]] std::optional<std::type_index> getFixtureType() const noexcept;

    const ExecutionTimer& getExecutionTimer() const;
    const TestStatus& getStatus() const;
    std::exception_ptr getException() const;

private:
    std::string name;
    TestOptions options;
    FunctionVariant function;
    std::optional<std::type_index> fixtureType;
    TestStatus status{TestStatus::Status::NotRun};
    std::exception_ptr exception;
    ExecutionTimer executionTimer;
};

}

#endif
