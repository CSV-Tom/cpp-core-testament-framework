#include <string>
#include <variant>
#include <functional>

#include "tests/TestStatus.hpp"
#include "ExecutionTimer.hpp"

class  TestSuite;

using FunctionVariant = std::variant<std::function<void()>, std::function<void(TestSuite&)>>;

class TestCase {
public:
    TestCase(const std::string& name, FunctionVariant function) : name(name), function(std::move(function)) {}

    const std::string& getName() const { return name; }

    std::variant<std::monostate, std::exception_ptr> execute(const TestSuite* suite = nullptr) {
        if (!suite) {
            return std::monostate{}; // No exception, as Suite is nullptr
        }

        if (status == TestStatus::Status::Skipped) {
            return std::monostate{}; // Neither success nor exception
        }

        try {
            executionTimer.start();
            std::visit([suite](auto&& func) {
                using T = std::decay_t<decltype(func)>;
                if constexpr (std::is_same_v<T, std::function<void()>>) {
                    func();
                } else if constexpr (std::is_same_v<T, std::function<void(TestSuite&)>>) {
                    func(const_cast<TestSuite&>(*suite));
                } else {
                    throw std::runtime_error("Invalid FunctionVariant type");
                }
            }, function);
            executionTimer.stop();
            status = TestStatus::Status::Passed;
            return std::monostate{}; // Successful execution
        } catch (...) {
            status = TestStatus::Status::Failed;
            exception = std::current_exception();
            return exception; // Return the exception
        }
    }

    std::string getDescription() const {
        return "TestCase: " + name;
    }

    const ExecutionTimer& getExecutionTimer() const {
        return executionTimer;
    }

    const TestStatus& getStatus() const {
        return status;
    }

    std::exception_ptr getException() const {
        return exception;
    }

private:
    std::string name;
    FunctionVariant function;
    TestStatus status{TestStatus::Status::NotRun};
    std::exception_ptr exception;
    ExecutionTimer executionTimer;
};
