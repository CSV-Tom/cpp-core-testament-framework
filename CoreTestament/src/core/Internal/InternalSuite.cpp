#include "InternalSuite.hpp"
#include "InternalTest.hpp"

#include <ranges>


namespace Testament {

InternalSuite::InternalSuite(const std::string& name_)
    : name(name_), hookManager(hookTimer), testManager(testTimer, statistic) {
    if (name.empty()) {
        throw std::logic_error("Suite name cannot be empty!");
    }
}

InternalSuite::InternalSuite(const std::string& name_, std::shared_ptr<Suite> suite_)
    : name(name_), hookManager(hookTimer), testManager(testTimer, statistic), suite(std::move(suite_)) {}

InternalSuite::~InternalSuite() = default;


void InternalSuite::addTest(const std::shared_ptr<Test>& test) {
    auto internalTest = std::dynamic_pointer_cast<InternalTest>(test);
    if (!internalTest) {
        throw std::logic_error("The provided test is not of type InternalTest");
    }
    tests.push_back(std::move(internalTest));
}

void InternalSuite::setBeforeSuite(Callback callback) { hookManager.setBeforeSuite(std::move(callback)); }
void InternalSuite::setBeforeEach(Callback callback)  { hookManager.setBeforeEach(std::move(callback)); }
void InternalSuite::setAfterEach(Callback callback)   { hookManager.setAfterEach(std::move(callback)); }
void InternalSuite::setAfterSuite(Callback callback)  { hookManager.setAfterSuite(std::move(callback)); }

void InternalSuite::setHandler(TestEventHandler* h) { handler = h; }


void InternalSuite::run() {
    totalTimer.start();

    if (handler) {
        handler->onSuiteStart({name});
    }

    hookManager.invokeBeforeSuiteHook();

    Suite& suiteRef = suite ? *suite : static_cast<Suite&>(*this);
    for (auto& test : tests | std::views::filter([this](const auto& t) {
        return !testFilter || testFilter(t->getName());
    })) {
        hookManager.invokeBeforeEachHook();
        testManager.executeTest(suiteRef, test, name, handler);
        hookManager.invokeAfterEachHook();
    }

    hookManager.invokeAfterSuiteHook();

    totalTimer.stop();
    hookManager.reportErrors();

    if (handler) {
        handler->onSuiteEnd({name, statistic.getPassedTests(), statistic.getFailedTests(), statistic.getSkippedTests()});
    }
}

const std::string& InternalSuite::getName() const { return name; }

const TestStatistics<unsigned int>& InternalSuite::getStatistics() const { return statistic; }

const ExecutionTimer& InternalSuite::getTotalTimer() const { return totalTimer; }

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
