#pragma once

#include "Testament/SuiteOptions.hpp"
#include "Testament/TestEventHandler.hpp"

#include "Stopwatch.hpp"
#include "HookExecutor.hpp"
#include "TestResultRecorder.hpp"
#include "TestCounts.hpp"

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <source_location>
#include <span>
#include <string>
#include <string_view>
#include <typeindex>
#include <cstdint>
#include <vector>

namespace Testament {
class LifecycleSuite;

namespace detail {
class TestInstance;
class TestHandle;

class SuiteInstance {
public:
    using Callback = HookExecutor::Callback;
    using FixtureFactory = std::move_only_function<std::unique_ptr<LifecycleSuite>()>;
    struct RunConfiguration {
        std::string_view testNameFilter;
        std::string_view filterExpression;
        std::size_t maxParallelTests{1};
        std::optional<std::uint64_t> shuffleSeed;
    };

    explicit SuiteInstance(std::string name, std::source_location location = {},
                           SuiteOptions options = {});
    explicit SuiteInstance(std::string name, std::source_location location,
                           std::type_index fixtureType,
                           FixtureFactory fixtureFactory, SuiteOptions options = {});
    ~SuiteInstance();

    void addTest(TestHandle test);

    void setBeforeSuite(Callback callback);
    void setBeforeEach(Callback callback);
    void setAfterEach(Callback callback);
    void setAfterSuite(Callback callback);

    [[nodiscard]] bool run(TestEventHandler* handler = nullptr);
    [[nodiscard]] bool run(TestEventHandler* handler, RunConfiguration configuration);

    const std::string& name() const;
    const SuiteOptions& options() const;
    [[nodiscard]] std::span<const std::unique_ptr<TestInstance>> tests() const noexcept;
    [[nodiscard]] std::source_location location() const noexcept;
    const TestCounts<unsigned int>& statistics() const;
    const Stopwatch& totalTimer() const;

private:
    void prepareTests(std::optional<std::uint64_t> shuffleSeed);
    [[nodiscard]] std::vector<TestInstance*> selectTests(
        const RunConfiguration& configuration
    ) const;
    [[nodiscard]] TestEventHandler::SuiteInfo suiteInfo() const;
    void skipTests(std::span<TestInstance* const> selectedTests, TestEventHandler* handler);
    [[nodiscard]] bool abortRun(std::span<TestInstance* const> selectedTests,
                                TestEventHandler* handler, std::string_view error);
    [[nodiscard]] bool executeFixturelessTests(
        std::span<TestInstance* const> selectedTests, TestEventHandler* handler,
        std::size_t maxParallelTests
    );
    [[nodiscard]] bool executeLifecycleTests(
        std::span<TestInstance* const> selectedTests, LifecycleSuite* fixture,
        TestEventHandler* handler
    );

    TestCounts<unsigned int> mCounts;

    std::string mName;
    std::source_location mLocation;
    SuiteOptions mOptions;
    std::vector<std::unique_ptr<TestInstance>> mTests;

    Stopwatch mTotalStopwatch;

    HookExecutor mHookExecutor;
    TestResultRecorder mResultRecorder;
    FixtureFactory mFixtureFactory;
    std::optional<std::type_index> mFixtureType;
};

}
}
