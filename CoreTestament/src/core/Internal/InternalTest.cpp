#include "InternalTest.hpp"


#include "utils/TestStatistics.hpp"


namespace Testament {


InternalTest::InternalTest(const std::string& name, FunctionVariant function) : name(name), function(std::move(function)) {

}

InternalTest::~InternalTest() = default;

std::variant<std::monostate, std::exception_ptr> InternalTest::execute(const Suite* suite) {
  //  if (!suite) {
 //       return std::monostate{}; // No exception, as Suite is nullptr
 //   }

    if (status == TestStatus::Status::Skipped) {
        return std::monostate{}; // Neither success nor exception
    }

    try {
        executionTimer.start();
        std::visit([suite](auto&& func) {
            using T = std::decay_t<decltype(func)>;
            if constexpr (std::is_same_v<T, std::function<void()>>) {
                func();
            } else if constexpr (std::is_same_v<T, std::function<void(Suite&)>>) {
                func(const_cast<Suite&>(*suite));
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

const std::string& InternalTest::getName() const {
    return name;
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
