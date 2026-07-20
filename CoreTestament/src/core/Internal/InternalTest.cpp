#include "InternalTest.hpp"
#include "AssertionCollection.hpp"
#include "Testament/SkipRequest.hpp"

#include "utils/TestStatistics.hpp"

#include <stdexcept>
#include <type_traits>
#include <utility>

namespace Testament {

namespace {

std::string definitionLocation(std::source_location definition) {
    return std::string{" at "} + definition.file_name() + ':' + std::to_string(definition.line());
}

}

InternalTest::InternalTest(std::string testName, FunctionVariant testFunction)
    : InternalTest(std::move(testName), std::source_location::current(), TestOptions{},
                   std::move(testFunction)) {}

InternalTest::InternalTest(std::string testName, std::source_location definition,
                           TestOptions testOptions, FunctionVariant testFunction,
                           std::optional<std::type_index> expectedFixture)
    : name_(std::move(testName)), location_(definition), options_(std::move(testOptions)),
      function(std::move(testFunction)), fixtureType_(expectedFixture) {
    if (name_.empty()) {
        throw std::invalid_argument("Test name cannot be empty" + definitionLocation(location_));
    }
    if (options_.isDisabled()) {
        status_ = TestStatus::Status::Skipped;
    }
}

InternalTest::~InternalTest() = default;

std::expected<void, std::exception_ptr> InternalTest::execute(LifecycleSuite* fixture) {
    if (options_.isDisabled()) {
        return {};
    }

    exception_ = nullptr;
    executionTimer_.reset();
    executionTimer_.start();
    Asserts::detail::beginAssertionCollection();

    try {
        std::visit([fixture](auto&& func) {
            using T = std::decay_t<decltype(func)>;
            if constexpr (std::is_same_v<T, std::move_only_function<void()>>) {
                func();
            } else if constexpr (
                std::is_same_v<T, std::move_only_function<void(LifecycleSuite&)>>
            ) {
                if (!fixture) {
                    throw std::logic_error("Suite context is required for this test");
                }
                func(*fixture);
            } else {
                throw std::runtime_error("Invalid FunctionVariant type");
            }
        }, function);
    } catch (const SkipRequest&) {
        exception_ = std::current_exception();
        (void)Asserts::detail::finishAssertionCollection();
        executionTimer_.stop();
        status_ = TestStatus::Status::Skipped;
        return {};
    } catch (...) {
        exception_ = Asserts::detail::finishAssertionCollection(std::current_exception());
    }

    if (!exception_) exception_ = Asserts::detail::finishAssertionCollection();
    executionTimer_.stop();
    status_ = exception_ ? TestStatus::Status::Failed : TestStatus::Status::Passed;
    return exception_ ? std::expected<void, std::exception_ptr>{std::unexpected(exception_)}
                     : std::expected<void, std::exception_ptr>{};
}

const std::string& InternalTest::name() const {
    return name_;
}

const TestOptions& InternalTest::options() const {
    return options_;
}

std::source_location InternalTest::location() const noexcept {
    return location_;
}

std::optional<std::type_index> InternalTest::fixtureType() const noexcept {
    return fixtureType_;
}

const ExecutionTimer& InternalTest::executionTimer() const {
    return executionTimer_;
}

const TestStatus& InternalTest::status() const {
    return status_;
}

std::exception_ptr InternalTest::exception() const {
    return exception_;
}

}
