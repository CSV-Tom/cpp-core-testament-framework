#include "InternalTest.hpp"


#include "utils/TestStatistics.hpp"

#include <stdexcept>
#include <type_traits>
#include <utility>

namespace Testament {

InternalTest::InternalTest(std::string name_, FunctionVariant function_)
    : InternalTest(std::move(name_), TestOptions{}, std::move(function_)) {}

InternalTest::InternalTest(std::string name_, TestOptions options_, FunctionVariant function_)
    : name(std::move(name_)), options(std::move(options_)), function(std::move(function_)) {
    if (options.isDisabled()) {
        status = TestStatus::Status::Skipped;
    }
}

InternalTest::~InternalTest() = default;

std::variant<std::monostate, std::exception_ptr> InternalTest::execute(LifecycleSuite* fixture) {
    if (status == TestStatus::Status::Skipped) {
        return std::monostate{}; // Neither success nor exception
    }

    exception = nullptr;
    executionTimer.reset();
    executionTimer.start();

    try {
        std::visit([fixture](auto&& func) {
            using T = std::decay_t<decltype(func)>;
            if constexpr (std::is_same_v<T, std::function<void()>>) {
                func();
            } else if constexpr (std::is_same_v<T, std::function<void(LifecycleSuite&)>>) {
                if (!fixture) {
                    throw std::logic_error("Suite context is required for this test");
                }
                func(*fixture);
            } else {
                throw std::runtime_error("Invalid FunctionVariant type");
            }
        }, function);
        executionTimer.stop();
        status = TestStatus::Status::Passed;
        return std::monostate{}; // Successful execution
    } catch (...) {
        executionTimer.stop();
        status = TestStatus::Status::Failed;
        exception = std::current_exception();
        return exception; // Return the exception
    }
}

const std::string& InternalTest::getName() const {
    return name;
}

const TestOptions& InternalTest::getOptions() const {
    return options;
}

const ExecutionTimer& InternalTest::getExecutionTimer() const {
    return executionTimer;
}

const TestStatus& InternalTest::getStatus() const {
    return status;
}

std::exception_ptr InternalTest::getException() const {
    return exception;
}

}
