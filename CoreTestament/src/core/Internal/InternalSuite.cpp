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
      hookManager(hookTimer), testManager(testTimer, statistic) {
    if (name.empty()) {
        throw std::logic_error("Suite name cannot be empty!");
    }
}

InternalSuite::InternalSuite(std::string name_, std::unique_ptr<LifecycleSuite> fixture_,
                             SuiteOptions options_)
    : InternalSuite(std::move(name_), std::move(options_)) {
    fixture = std::move(fixture_);
    if (!fixture) throw std::invalid_argument("Lifecycle suite fixture cannot be null");
    fixtureType = std::type_index(typeid(*fixture));
    setBeforeSuite([this] { fixture->beforeAll(); });
    setBeforeEach([this] { fixture->beforeEach(); });
    setAfterEach([this] { fixture->afterEach(); });
    setAfterSuite([this] { fixture->afterAll(); });
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
    hookTimer.reset();
    testTimer.reset();
    hookManager.resetErrors();

    std::ranges::stable_sort(tests, {}, [](const auto& test) {
        return test->getOptions().order().value_or(0);
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

    if (!hookManager.invokeBeforeSuiteHook()) {
        const auto error = hookManager.getErrors().back();
        for (auto& test : tests | std::views::filter(selected)) {
            testManager.reportStart(suiteInfo(), test, handler);
            testManager.reportResult(
                suiteInfo(), test, TestEventHandler::TestResultStatus::LifecycleError,
                std::chrono::duration<double>::zero(),
                std::make_exception_ptr(std::runtime_error(error)), handler
            );
        }
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
            const bool beforeEachSucceeded = hookManager.invokeBeforeEachHook();
            if (!beforeEachSucceeded) lifecycleError = hookManager.getErrors().back();

            TestManager::Result result{std::monostate{}};
            if (beforeEachSucceeded) {
                result = testManager.executeAttempt(fixture.get(), test);
                duration += test->getExecutionTimer().getDuration();
            }

            if (!hookManager.invokeAfterEachHook()) {
                if (!lifecycleError.empty()) lifecycleError += "; ";
                lifecycleError += hookManager.getErrors().back();
            }

            if (!lifecycleError.empty()) {
                status = TestEventHandler::TestResultStatus::LifecycleError;
                exception = std::make_exception_ptr(std::runtime_error(lifecycleError));
            } else if (std::holds_alternative<std::exception_ptr>(result)) {
                status = TestEventHandler::TestResultStatus::Failed;
                exception = std::get<std::exception_ptr>(result);
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

    if (!hookManager.invokeAfterSuiteHook()) {
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
