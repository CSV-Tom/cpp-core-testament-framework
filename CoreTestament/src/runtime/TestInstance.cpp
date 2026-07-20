#include "TestInstance.hpp"
#include "AssertionContext.hpp"
#include "Testament/SkipRequest.hpp"

#include "TestCounts.hpp"

#include <stdexcept>
#include <type_traits>
#include <utility>

namespace Testament::detail {

namespace {

std::string definitionLocation(std::source_location definition) {
    return std::string{" at "} + definition.file_name() + ':' + std::to_string(definition.line());
}

}

TestInstance::TestInstance(std::string testName, TestFunction testFunction)
    : TestInstance(std::move(testName), std::source_location::current(), TestOptions{},
                   std::move(testFunction)) {}

TestInstance::TestInstance(std::string testName, std::source_location definition,
                           TestOptions testOptions, TestFunction testFunction,
                           std::optional<std::type_index> expectedFixture)
    : mName(std::move(testName)), mLocation(definition), mOptions(std::move(testOptions)),
      mFunction(std::move(testFunction)), mFixtureType(expectedFixture) {
    if (mName.empty()) {
        throw std::invalid_argument("Test name cannot be empty" + definitionLocation(mLocation));
    }
    if (mOptions.isDisabled()) {
        mStatus = TestState::Status::Skipped;
    }
}

TestInstance::~TestInstance() = default;

std::expected<void, std::exception_ptr> TestInstance::execute(LifecycleSuite* fixture) {
    if (mOptions.isDisabled()) {
        return {};
    }

    mException = nullptr;
    mStopwatch.reset();
    mStopwatch.start();
    Asserts::detail::beginAssertionContext();

    try {
        std::visit([fixture](auto&& func) {
            using T = std::decay_t<decltype(func)>;
            if constexpr (std::is_same_v<T, MoveOnlyFunction<void()>>) {
                func();
            } else if constexpr (
                std::is_same_v<T, MoveOnlyFunction<void(LifecycleSuite&)>>
            ) {
                if (!fixture) {
                    throw std::logic_error("Suite context is required for this test");
                }
                func(*fixture);
            } else {
                throw std::runtime_error("Invalid test function type");
            }
        }, mFunction);
    } catch (const SkipRequest&) {
        mException = std::current_exception();
        (void)Asserts::detail::finishAssertionContext();
        mStopwatch.stop();
        mStatus = TestState::Status::Skipped;
        return {};
    } catch (...) {
        mException = Asserts::detail::finishAssertionContext(std::current_exception());
    }

    if (!mException) mException = Asserts::detail::finishAssertionContext();
    mStopwatch.stop();
    mStatus = mException ? TestState::Status::Failed : TestState::Status::Passed;
    return mException ? std::expected<void, std::exception_ptr>{std::unexpected(mException)}
                     : std::expected<void, std::exception_ptr>{};
}

const std::string& TestInstance::name() const {
    return mName;
}

const TestOptions& TestInstance::options() const {
    return mOptions;
}

std::source_location TestInstance::location() const noexcept {
    return mLocation;
}

std::optional<std::type_index> TestInstance::fixtureType() const noexcept {
    return mFixtureType;
}

const Stopwatch& TestInstance::executionTimer() const {
    return mStopwatch;
}

const TestState& TestInstance::status() const {
    return mStatus;
}

std::exception_ptr TestInstance::exception() const {
    return mException;
}

}
