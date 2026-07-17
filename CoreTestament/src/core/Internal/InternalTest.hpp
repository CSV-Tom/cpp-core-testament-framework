
#ifndef TESTFRAMEWORK_TESTAMENT_INTERNALTEST_HPP
#define TESTFRAMEWORK_TESTAMENT_INTERNALTEST_HPP

#include <string>
#include <variant>
#include <exception>

#include "Testament/Options.hpp"
#include "FunctionVariant.hpp"


#include "utils/TestStatus.hpp"
#include "utils/ExecutionTimer.hpp"


namespace Testament {

class InternalTest {
public:
    InternalTest(std::string name, FunctionVariant function);
    InternalTest(std::string name, TestOptions options, FunctionVariant function);
    InternalTest(InternalTest&&) noexcept = default;
    InternalTest& operator=(InternalTest&&) noexcept = default;
    ~InternalTest();

    std::variant<std::monostate, std::exception_ptr> execute(LifecycleSuite* fixture = nullptr);

    const std::string& getName() const;
    const TestOptions& getOptions() const;

    const ExecutionTimer& getExecutionTimer() const;
    const TestStatus& getStatus() const;
    std::exception_ptr getException() const;

private:
    std::string name;
    TestOptions options;
    FunctionVariant function;
    TestStatus status{TestStatus::Status::NotRun};
    std::exception_ptr exception;
    ExecutionTimer executionTimer;
};

}

#endif
