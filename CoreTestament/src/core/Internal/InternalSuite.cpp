#include "InternalSuite.hpp"
#include "InternalTest.hpp"
#include "TestAccess.hpp"

#include "Testament/LifecycleSuite.hpp"
#include "Testament/detail/TestHandle.hpp"
#include "Testament/TestEventHandler.hpp"

#include <algorithm>
#include <chrono>
#include <exception>
#include <ranges>
#include <stdexcept>
#include <string>
#include <utility>


namespace Testament {

InternalSuite::InternalSuite(std::string name_, SuiteOptions options_)
    : name(std::move(name_)), options(std::move(options_)),
      testManager(statistic) {
    if (name.empty()) {
        throw std::logic_error("Suite name cannot be empty!");
    }
}

InternalSuite::InternalSuite(std::string name_, std::type_index fixtureType_,
                             FixtureFactory fixtureFactory_, SuiteOptions options_)
    : InternalSuite(std::move(name_), std::move(options_)) {
    if (!fixtureFactory_) throw std::invalid_argument("Lifecycle suite fixture factory cannot be empty");
    fixtureFactory = std::move(fixtureFactory_);
    fixtureType = fixtureType_;
}

InternalSuite::~InternalSuite() = default;


void InternalSuite::addTest(detail::TestHandle test) {
    auto internalTest = detail::TestAccess::release(std::move(test));
    if (const auto expectedFixture = internalTest->getFixtureType();
        expectedFixture && expectedFixture != fixtureType) {
        throw std::invalid_argument("Test fixture type does not match suite fixture type");
    }
    if (internalTest->getOptions().maxAttempts() == 0) {
        throw std::invalid_argument("Test maxAttempts must be greater than zero");
    }
    if (std::ranges::any_of(tests, [&internalTest](const auto& registered) {
        return registered->getName() == internalTest->getName();
    })) {
        throw std::logic_error("Test name must be unique within a suite: "
                               + internalTest->getName());
    }
    tests.push_back(std::move(internalTest));
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

void InternalSuite::setHandler(TestEventHandler* h) {
    handler = h;
}


bool InternalSuite::run() {
    statistic.reset();
    totalTimer.reset();
    hookManager.resetErrors();

    std::ranges::stable_sort(tests, [](const auto& left, const auto& right) {
        const auto leftOrder = left->getOptions().order().value_or(0);
        const auto rightOrder = right->getOptions().order().value_or(0);
        return leftOrder != rightOrder ? leftOrder < rightOrder : left->getName() < right->getName();
    });

    totalTimer.start();

    if (handler) {
        handler->onSuiteStart({name, 0, 0, 0, options});
    }

    const auto suiteInfo = [this] {
        return TestEventHandler::SuiteInfo{
            name,
            statistic.getPassedTests(),
            statistic.getFailedTests(),
            statistic.getSkippedTests(),
            options
        };
    };
    const auto reportSuiteError = [this, &suiteInfo](std::string_view error) {
        if (handler) handler->onSuiteAbort(suiteInfo(), error);
    };
    const auto selected = [this](const auto& test) {
        return !testFilter || testFilter(test->getName());
    };
    const auto skipSelectedTests = [this, &suiteInfo, &selected] {
        for (auto& test : tests | std::views::filter(selected)) {
            testManager.reportResult(
                suiteInfo(), test, TestEventHandler::TestResultStatus::Skipped,
                std::chrono::duration<double>::zero(), {}, handler
            );
        }
    };

    std::unique_ptr<LifecycleSuite> fixture;
    if (fixtureFactory) {
        Callback createFixture = [this, &fixture] {
            fixture = fixtureFactory();
            if (!fixture) throw std::runtime_error("Fixture factory returned null");
        };
        if (!hookManager.invoke(createFixture, "fixture construction")) {
            const auto error = hookManager.getErrors().back();
            skipSelectedTests();
            totalTimer.stop();
            reportSuiteError(error);
            if (handler) handler->onSuiteEnd(suiteInfo());
            return false;
        }
    }

    Callback beforeAll = [&fixture] {
        if (fixture) fixture->beforeAll();
    };
    if (!hookManager.invokeBeforeSuiteHook()
        || !hookManager.invoke(beforeAll, "beforeAll")) {
        const auto error = hookManager.getErrors().back();
        skipSelectedTests();
        totalTimer.stop();
        reportSuiteError(error);
        if (handler) handler->onSuiteEnd(suiteInfo());
        return false;
    }

    bool hooksSucceeded = true;
    for (auto& test : tests | std::views::filter(selected)) {
        if (test->getOptions().isDisabled()) {
            testManager.reportResult(
                suiteInfo(), test, TestEventHandler::TestResultStatus::Skipped,
                std::chrono::duration<double>::zero(), {}, handler
            );
            continue;
        }

        testManager.reportStart(suiteInfo(), test, handler);
        auto remainingAttempts = test->getOptions().maxAttempts();
        auto duration = std::chrono::duration<double>::zero();
        auto status = TestEventHandler::TestResultStatus::Failed;
        std::exception_ptr exception;

        while (true) {
            std::string lifecycleError;
            Callback beforeEach = [&fixture] {
                if (fixture) fixture->beforeEach();
            };
            const bool beforeEachSucceeded = hookManager.invokeBeforeEachHook()
                && hookManager.invoke(beforeEach, "beforeEach");
            if (!beforeEachSucceeded) lifecycleError = hookManager.getErrors().back();

            TestManager::Result result{};
            if (beforeEachSucceeded) {
                result = testManager.executeAttempt(fixture.get(), test);
                duration += test->getExecutionTimer().getDuration();
            }

            Callback afterEach = [&fixture] {
                if (fixture) fixture->afterEach();
            };
            if (!hookManager.invoke(afterEach, "afterEach")
                || !hookManager.invokeAfterEachHook()) {
                if (!lifecycleError.empty()) lifecycleError += "; ";
                lifecycleError += hookManager.getErrors().back();
            }

            if (!lifecycleError.empty()) {
                status = TestEventHandler::TestResultStatus::LifecycleError;
                exception = std::make_exception_ptr(std::runtime_error(lifecycleError));
            } else if (!result) {
                status = TestEventHandler::TestResultStatus::Failed;
                exception = result.error();
            } else {
                status = TestEventHandler::TestResultStatus::Passed;
                exception = {};
            }

            if (status == TestEventHandler::TestResultStatus::Passed || remainingAttempts <= 1) break;
            --remainingAttempts;
        }

        hooksSucceeded = status != TestEventHandler::TestResultStatus::LifecycleError
            && hooksSucceeded;
        testManager.reportResult(suiteInfo(), test, status, duration, exception, handler);
    }

    Callback afterAll = [&fixture] {
        if (fixture) fixture->afterAll();
    };
    if (!hookManager.invoke(afterAll, "afterAll")
        || !hookManager.invokeAfterSuiteHook()) {
        hooksSucceeded = false;
        reportSuiteError(hookManager.getErrors().back());
    }

    totalTimer.stop();

    if (handler) handler->onSuiteEnd(suiteInfo());

    return hooksSucceeded;
}

const std::string& InternalSuite::getName() const {
    return name;
}

const SuiteOptions& InternalSuite::getOptions() const {
    return options;
}

const TestStatistics<unsigned int>& InternalSuite::getStatistics() const {
    return statistic;
}

const ExecutionTimer& InternalSuite::getTotalTimer() const {
    return totalTimer;
}

void InternalSuite::setTestFilter(std::variant<std::string, std::regex> filter) {
    if (std::holds_alternative<std::string>(filter)) {
        std::string exactName = std::get<std::string>(filter);
        testFilter = [exactName](std::string_view testName) {
            return testName == exactName;
        };
    } else if (std::holds_alternative<std::regex>(filter)) {
        std::regex pattern = std::get<std::regex>(filter);
        testFilter = [pattern](std::string_view testName) {
            return std::regex_match(testName.begin(), testName.end(), pattern);
        };
    }
}

}
