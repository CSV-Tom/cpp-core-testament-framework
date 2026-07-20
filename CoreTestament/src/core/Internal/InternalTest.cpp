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
    : mName(std::move(testName)), mLocation(definition), mOptions(std::move(testOptions)),
      function(std::move(testFunction)), mFixtureType(expectedFixture) {
    if (mName.empty()) {
        throw std::invalid_argument("Test name cannot be empty" + definitionLocation(mLocation));
    }
    if (mOptions.isDisabled()) {
        mStatus = TestStatus::Status::Skipped;
    }
}

InternalTest::~InternalTest() = default;

std::expected<void, std::exception_ptr> InternalTest::execute(LifecycleSuite* fixture) {
    if (mOptions.isDisabled()) {
        return {};
    }

    mException = nullptr;
    mExecutionTimer.reset();
    mExecutionTimer.start();
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
        mException = std::current_exception();
        (void)Asserts::detail::finishAssertionCollection();
        mExecutionTimer.stop();
        mStatus = TestStatus::Status::Skipped;
        return {};
    } catch (...) {
        mException = Asserts::detail::finishAssertionCollection(std::current_exception());
    }

    if (!mException) mException = Asserts::detail::finishAssertionCollection();
    mExecutionTimer.stop();
    mStatus = mException ? TestStatus::Status::Failed : TestStatus::Status::Passed;
    return mException ? std::expected<void, std::exception_ptr>{std::unexpected(mException)}
                     : std::expected<void, std::exception_ptr>{};
}

const std::string& InternalTest::name() const {
    return mName;
}

const TestOptions& InternalTest::options() const {
    return mOptions;
}

std::source_location InternalTest::location() const noexcept {
    return mLocation;
}

std::optional<std::type_index> InternalTest::fixtureType() const noexcept {
    return mFixtureType;
}

const ExecutionTimer& InternalTest::executionTimer() const {
    return mExecutionTimer;
}

const TestStatus& InternalTest::status() const {
    return mStatus;
}

std::exception_ptr InternalTest::exception() const {
    return mException;
}

}
