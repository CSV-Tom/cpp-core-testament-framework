#ifndef TESTFRAMEWORK_TESTAMENT_INTERNALSUITE_HPP
#define TESTFRAMEWORK_TESTAMENT_INTERNALSUITE_HPP

#include "Testament/SuiteOptions.hpp"
#include "Testament/TestEventHandler.hpp"

#include "utils/ExecutionTimer.hpp"
#include "utils/HookManager.hpp"
#include "utils/TestManager.hpp"
#include "utils/TestStatistics.hpp"

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
class InternalTest;
namespace detail {
class TestHandle;
}

class InternalSuite {
public:
    using Callback = HookManager::Callback;
    using FixtureFactory = std::move_only_function<std::unique_ptr<LifecycleSuite>()>;
    struct RunConfiguration {
        std::string_view testNameFilter;
        std::string_view filterExpression;
        std::size_t maxParallelTests{1};
        std::optional<std::uint64_t> shuffleSeed;
    };

    explicit InternalSuite(std::string name, std::source_location location = {},
                           SuiteOptions options = {});
    explicit InternalSuite(std::string name, std::source_location location,
                           std::type_index fixtureType,
                           FixtureFactory fixtureFactory, SuiteOptions options = {});
    ~InternalSuite();

    void addTest(detail::TestHandle test);

    void setBeforeSuite(Callback callback);
    void setBeforeEach(Callback callback);
    void setAfterEach(Callback callback);
    void setAfterSuite(Callback callback);

    [[nodiscard]] bool run(TestEventHandler* handler = nullptr);
    [[nodiscard]] bool run(TestEventHandler* handler, RunConfiguration configuration);

    const std::string& name() const;
    const SuiteOptions& options() const;
    [[nodiscard]] std::span<const std::unique_ptr<InternalTest>> tests() const noexcept;
    [[nodiscard]] std::source_location location() const noexcept;
    const TestStatistics<unsigned int>& statistics() const;
    const ExecutionTimer& totalTimer() const;

private:
    void prepareTests(std::optional<std::uint64_t> shuffleSeed);
    [[nodiscard]] std::vector<InternalTest*> selectTests(
        const RunConfiguration& configuration
    ) const;
    [[nodiscard]] TestEventHandler::SuiteInfo suiteInfo() const;
    void skipTests(std::span<InternalTest* const> selectedTests, TestEventHandler* handler);
    [[nodiscard]] bool abortRun(std::span<InternalTest* const> selectedTests,
                                TestEventHandler* handler, std::string_view error);
    [[nodiscard]] bool executeFixturelessTests(
        std::span<InternalTest* const> selectedTests, TestEventHandler* handler,
        std::size_t maxParallelTests
    );
    [[nodiscard]] bool executeLifecycleTests(
        std::span<InternalTest* const> selectedTests, LifecycleSuite* fixture,
        TestEventHandler* handler
    );

    TestStatistics<unsigned int> statistic;

    std::string name_;
    std::source_location location_;
    SuiteOptions options_;
    std::vector<std::unique_ptr<InternalTest>> tests_;

    ExecutionTimer totalTimer_;

    HookManager hookManager;
    TestManager testManager;
    FixtureFactory fixtureFactory;
    std::optional<std::type_index> fixtureType_;
};

}

#endif
