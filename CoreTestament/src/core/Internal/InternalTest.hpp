
#ifndef TESTFRAMEWORK_TESTAMENT_INTERNALTEST_HPP
#define TESTFRAMEWORK_TESTAMENT_INTERNALTEST_HPP

#include <string>
#include <variant>
#include <exception>

#include "Testament/Test.hpp"
#include "Testament/Suite.hpp"
#include "Testament/FunctionVariant.hpp"


#include "utils/TestStatus.hpp"
#include "utils/ExecutionTimer.hpp"


namespace Testament {

class InternalTest : public Test {
public:
    InternalTest(const std::string& name, FunctionVariant function);
    InternalTest(InternalTest&&) noexcept = default;
    InternalTest& operator=(InternalTest&&) noexcept = default;
    ~InternalTest() override;

    std::variant<std::monostate, std::exception_ptr> execute(const Suite* suite = nullptr);
        
    const std::string& getName() const;

    const ExecutionTimer& getExecutionTimer() const;
    const TestStatus& getStatus() const;
    std::exception_ptr getException() const;

private:
    std::string name;
    FunctionVariant function;    
    TestStatus status{TestStatus::Status::NotRun}; 
    std::exception_ptr exception;
    ExecutionTimer executionTimer;    
};

}

#endif