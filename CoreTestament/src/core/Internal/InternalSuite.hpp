#ifndef TESTFRAMEWORK_TESTAMENT_INTERNALSUITE_HPP
#define TESTFRAMEWORK_TESTAMENT_INTERNALSUITE_HPP

#include "Testament/SuiteOptions.hpp"

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
class TestEventHandler;
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

    const std::string& getName() const;
    const SuiteOptions& getOptions() const;
    [[nodiscard]] std::span<const std::unique_ptr<InternalTest>> getTests() const noexcept;
    [[nodiscard]] std::source_location getLocation() const noexcept;
    const TestStatistics<unsigned int>& getStatistics() const;
    const ExecutionTimer& getTotalTimer() const;

private:
    TestStatistics<unsigned int> statistic;

    std::string name;
    std::source_location location;
    SuiteOptions options;
    std::vector<std::unique_ptr<InternalTest>> tests;

    ExecutionTimer totalTimer;

    HookManager hookManager;
    TestManager testManager;
    FixtureFactory fixtureFactory;
    std::optional<std::type_index> fixtureType;
};

}

#endif
