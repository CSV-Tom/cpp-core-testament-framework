#include "InternalSuite.hpp"
#include "InternalTest.hpp"
#include "LifecycleAccess.hpp"
#include "TestExecutor.hpp"
#include "TestAccess.hpp"
#include "FilterPattern.hpp"

#include "Testament/LifecycleSuite.hpp"
#include "Testament/detail/TestHandle.hpp"
#include "Testament/TestEventHandler.hpp"

#include <algorithm>
#include <chrono>
#include <exception>
#include <future>
#include <random>
#include <ranges>
#include <stdexcept>
#include <string>
#include <utility>


namespace Testament {

namespace {

std::string definitionLocation(std::source_location definition) {
    return std::string{" at "} + definition.file_name() + ':' + std::to_string(definition.line());
}

}

InternalSuite::InternalSuite(std::string suiteName, std::source_location definition,
                             SuiteOptions suiteOptions)
    : mName(std::move(suiteName)), mLocation(definition), mOptions(std::move(suiteOptions)),
      testManager(statistic) {
    if (mName.empty()) {
        throw std::logic_error("Suite name cannot be empty!");
    }
}

InternalSuite::InternalSuite(std::string suiteName, std::source_location definition,
                             std::type_index expectedFixture,
                             FixtureFactory factory, SuiteOptions suiteOptions)
    : InternalSuite(std::move(suiteName), definition, std::move(suiteOptions)) {
    if (!factory) throw std::invalid_argument("Lifecycle suite fixture factory cannot be empty");
    fixtureFactory = std::move(factory);
    mFixtureType = expectedFixture;
}

InternalSuite::~InternalSuite() = default;


void InternalSuite::addTest(detail::TestHandle test) {
    auto internalTest = detail::TestAccess::release(std::move(test));
    if (const auto expectedFixture = internalTest->fixtureType();
        expectedFixture && expectedFixture != mFixtureType) {
        throw std::invalid_argument(
            "Test fixture type does not match suite fixture type"
            + definitionLocation(internalTest->location())
        );
    }
    if (internalTest->options().maxAttempts() == 0) {
        throw std::invalid_argument(
            "Test maxAttempts must be greater than zero"
            + definitionLocation(internalTest->location())
        );
    }
    if (std::ranges::any_of(mTests, [&internalTest](const auto& registered) {
        return registered->name() == internalTest->name();
    })) {
        throw std::logic_error("Test name must be unique within a suite: "
                               + internalTest->name()
                               + definitionLocation(internalTest->location()));
    }
    mTests.push_back(std::move(internalTest));
}

void InternalSuite::setBeforeSuite(Callback callback) {
    hookManager.setBeforeSuite(std::move(callback));
}
void InternalSuite::setBeforeEach(Callback callback)  {
    hookManager.setBeforeEach(std::move(callback));
}
void InternalSuite::setAfterEach(Callback callback)   {
    hookManager.setAfterEach(std::move(callback));
}
void InternalSuite::setAfterSuite(Callback callback)  {
    hookManager.setAfterSuite(std::move(callback));
}

bool InternalSuite::run(TestEventHandler* handler) {
    return run(handler, RunConfiguration{});
}

bool InternalSuite::run(TestEventHandler* handler, RunConfiguration configuration) {
    statistic.reset();
    mTotalTimer.reset();
    hookManager.resetErrors();
    prepareTests(configuration.shuffleSeed);
    mTotalTimer.start();
    if (handler) handler->onSuiteStart(suiteInfo());
    const auto selectedTests = selectTests(configuration);

    std::unique_ptr<LifecycleSuite> fixture;
    if (fixtureFactory) {
        Callback createFixture = [this, &fixture] {
            fixture = fixtureFactory();
            if (!fixture) throw std::runtime_error("Fixture factory returned null");
        };
        if (!hookManager.invoke(createFixture, "fixture construction")) {
            return abortRun(selectedTests, handler, hookManager.errors().back());
        }
    }

    Callback beforeAll = [&fixture] {
        if (fixture) detail::LifecycleAccess::beforeAll(*fixture);
    };
    if (!hookManager.invokeBeforeSuiteHook()
        || !hookManager.invoke(beforeAll, "beforeAll")) {
        return abortRun(selectedTests, handler, hookManager.errors().back());
    }

    const bool canRunInParallel = !fixtureFactory && !hookManager.hasPerTestHooks()
        && configuration.maxParallelTests > 1;
    bool hooksSucceeded = canRunInParallel
        ? executeFixturelessTests(selectedTests, handler, configuration.maxParallelTests)
        : executeLifecycleTests(selectedTests, fixture.get(), handler);

    Callback afterAll = [&fixture] {
        if (fixture) detail::LifecycleAccess::afterAll(*fixture);
    };
    if (!hookManager.invoke(afterAll, "afterAll")
        || !hookManager.invokeAfterSuiteHook()) {
        hooksSucceeded = false;
        statistic.incrementErrors();
        if (handler) handler->onSuiteAbort(suiteInfo(), hookManager.errors().back());
    }

    mTotalTimer.stop();

    if (handler) handler->onSuiteEnd(suiteInfo());

    return hooksSucceeded;
}

void InternalSuite::prepareTests(std::optional<std::uint64_t> shuffleSeed) {
    if (shuffleSeed) {
        std::mt19937_64 random{*shuffleSeed};
        std::ranges::shuffle(mTests, random);
    }
    std::ranges::stable_sort(mTests, [shuffle = shuffleSeed.has_value()](
        const auto& left, const auto& right
    ) {
        const auto leftOrder = left->options().order().value_or(0);
        const auto rightOrder = right->options().order().value_or(0);
        if (leftOrder != rightOrder) return leftOrder < rightOrder;
        return !shuffle && left->name() < right->name();
    });
}

std::vector<InternalTest*> InternalSuite::selectTests(
    const RunConfiguration& configuration
) const {
    std::vector<InternalTest*> selected;
    for (const auto& test : mTests) {
        const bool nameMatches = configuration.testNameFilter.empty()
            || detail::matchesNameFilter(test->name(), configuration.testNameFilter);
        const bool expressionMatches = configuration.filterExpression.empty()
            || detail::matchesTestFilter(
                mName, mOptions.tags(), test->name(), test->options().tags(),
                configuration.filterExpression
            );
        if (nameMatches && expressionMatches) selected.push_back(test.get());
    }
    return selected;
}

TestEventHandler::SuiteInfo InternalSuite::suiteInfo() const {
    return {
        mName, mLocation, statistic.passedTests(), statistic.failedTests(),
        statistic.skippedTests(), statistic.errors(), mOptions
    };
}

void InternalSuite::skipTests(
    std::span<InternalTest* const> selectedTests, TestEventHandler* handler
) {
    for (auto* test : selectedTests) {
        testManager.reportResult(
            suiteInfo(), *test, TestEventHandler::TestResultStatus::Skipped,
            std::chrono::duration<double>::zero(), {}, handler
        );
    }
}

bool InternalSuite::abortRun(
    std::span<InternalTest* const> selectedTests, TestEventHandler* handler,
    std::string_view error
) {
    statistic.incrementErrors();
    skipTests(selectedTests, handler);
    mTotalTimer.stop();
    if (handler) {
        handler->onSuiteAbort(suiteInfo(), error);
        handler->onSuiteEnd(suiteInfo());
    }
    return false;
}

bool InternalSuite::executeFixturelessTests(
    std::span<InternalTest* const> selectedTests, TestEventHandler* handler,
    std::size_t maxParallelTests
) {
    detail::TestExecutor executor{testManager, hookManager};
    const auto consume = [this, handler](InternalTest* test, detail::TestExecutionResult result) {
        testManager.reportResult(
            suiteInfo(), *test, result.status, result.duration,
            std::move(result.exception), handler
        );
    };
    const auto start = [this, handler](InternalTest* test) {
        if (!test->options().isDisabled()) {
            testManager.reportStart(suiteInfo(), *test, handler);
        }
    };

    std::size_t index{};
    while (index < selectedTests.size()) {
        if (selectedTests[index]->options().execution() == Execution::Serial) {
            start(selectedTests[index]);
            consume(selectedTests[index], executor.executeFixtureless(*selectedTests[index]));
            ++index;
            continue;
        }
        const auto concurrentEnd = std::ranges::find_if(
            selectedTests.begin() + static_cast<std::ptrdiff_t>(index), selectedTests.end(),
            [](const auto* test) {
                return test->options().execution() == Execution::Serial;
            }
        );
        const auto endIndex = static_cast<std::size_t>(concurrentEnd - selectedTests.begin());
        while (index < endIndex) {
            const auto count = std::min(maxParallelTests, endIndex - index);
            std::vector<std::future<detail::TestExecutionResult>> running;
            running.reserve(count);
            for (std::size_t offset = 0; offset < count; ++offset) {
                auto* test = selectedTests[index + offset];
                start(test);
                running.push_back(std::async(std::launch::async, [&executor, test] {
                    return executor.executeFixtureless(*test);
                }));
            }
            for (std::size_t offset = 0; offset < count; ++offset) {
                consume(selectedTests[index + offset], running[offset].get());
            }
            index += count;
        }
    }
    return true;
}

bool InternalSuite::executeLifecycleTests(
    std::span<InternalTest* const> selectedTests, LifecycleSuite* fixture,
    TestEventHandler* handler
) {
    Callback beforeEach = [fixture] {
        if (fixture) detail::LifecycleAccess::beforeEach(*fixture);
    };
    Callback afterEach = [fixture] {
        if (fixture) detail::LifecycleAccess::afterEach(*fixture);
    };
    detail::TestExecutor executor{testManager, hookManager};
    bool hooksSucceeded = true;
    for (auto* test : selectedTests) {
        if (!test->options().isDisabled()) {
            testManager.reportStart(suiteInfo(), *test, handler);
        }
        auto result = executor.executeWithLifecycle(*test, fixture, beforeEach, afterEach);
        hooksSucceeded = result.lifecycleSucceeded() && hooksSucceeded;
        testManager.reportResult(
            suiteInfo(), *test, result.status, result.duration,
            std::move(result.exception), handler
        );
    }
    return hooksSucceeded;
}

const std::string& InternalSuite::name() const {
    return mName;
}

const SuiteOptions& InternalSuite::options() const {
    return mOptions;
}

std::span<const std::unique_ptr<InternalTest>> InternalSuite::tests() const noexcept {
    return mTests;
}

std::source_location InternalSuite::location() const noexcept {
    return mLocation;
}

const TestStatistics<unsigned int>& InternalSuite::statistics() const {
    return statistic;
}

const ExecutionTimer& InternalSuite::totalTimer() const {
    return mTotalTimer;
}

}
