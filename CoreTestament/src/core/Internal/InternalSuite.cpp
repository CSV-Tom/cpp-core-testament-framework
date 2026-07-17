#include "InternalSuite.hpp"
#include "InternalTest.hpp"
#include "TestAccess.hpp"

#include "Testament/Test.hpp"

#include <algorithm>
#include <ranges>
#include <stdexcept>
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
    setBeforeSuite([this] { fixture->beforeAll(); });
    setBeforeEach([this] { fixture->beforeEach(); });
    setAfterEach([this] { fixture->afterEach(); });
    setAfterSuite([this] { fixture->afterAll(); });
}

InternalSuite::~InternalSuite() = default;


void InternalSuite::addTest(Test test) {
    auto internalTest = detail::TestAccess::release(std::move(test));
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

    const auto reportHookErrors = [this] {
        if (!handler) {
            return;
        }
        const TestEventHandler::SuiteInfo suiteInfo{
            name,
            statistic.getPassedTests(),
            statistic.getFailedTests(),
            statistic.getSkippedTests(),
            options
        };
        for (const auto& error : hookManager.getErrors()) {
            handler->onSuiteAbort(suiteInfo, error);
        }
    };

    if (!hookManager.invokeBeforeSuiteHook()) {
        totalTimer.stop();
        reportHookErrors();
        return false;
    }

    bool hooksSucceeded = true;
    for (auto& test : tests | std::views::filter([this](const auto& t) {
    return !testFilter || testFilter(t->getName());
    })) {
        if (!hookManager.invokeBeforeEachHook()) {
            hooksSucceeded = false;
            continue;
        }
        testManager.executeTest(fixture.get(), test, {
            name,
            statistic.getPassedTests(),
            statistic.getFailedTests(),
            statistic.getSkippedTests(),
            options
        }, handler);
        hooksSucceeded = hookManager.invokeAfterEachHook() && hooksSucceeded;
    }

    hooksSucceeded = hookManager.invokeAfterSuiteHook() && hooksSucceeded;

    totalTimer.stop();
    reportHookErrors();

    if (handler) {
        handler->onSuiteEnd({name, statistic.getPassedTests(), statistic.getFailedTests(),
                             statistic.getSkippedTests(), options});
    }

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
