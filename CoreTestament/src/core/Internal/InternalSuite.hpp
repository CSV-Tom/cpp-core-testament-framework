#ifndef TESTFRAMEWORK_TESTAMENT_INTERNALSUITE_HPP
#define TESTFRAMEWORK_TESTAMENT_INTERNALSUITE_HPP

#include "Testament/LifecycleSuite.hpp"
#include "Testament/Test.hpp"
#include "Testament/TestEventHandler.hpp"

#include "utils/ExecutionTimer.hpp"
#include "utils/HookManager.hpp"
#include "utils/TestManager.hpp"
#include "utils/TestStatistics.hpp"

#include <functional>
#include <memory>
#include <regex>
#include <string>
#include <variant>
#include <vector>

namespace Testament {

class InternalTest;

class InternalSuite {
public:
    using Callback = std::function<void()>;
    using TestFilter = std::function<bool(const std::string&)>;

    explicit InternalSuite(std::string name, SuiteOptions options = {});
    explicit InternalSuite(std::string name, std::unique_ptr<LifecycleSuite> fixture,
                           SuiteOptions options = {});
    ~InternalSuite();

    void addTest(Test test);

    void setBeforeSuite(Callback callback);
    void setBeforeEach(Callback callback);
    void setAfterEach(Callback callback);
    void setAfterSuite(Callback callback);

    void setTestFilter(std::variant<std::string, std::regex> filter);
    void setHandler(TestEventHandler* handler);

    [[nodiscard]] bool run();

    const std::string& getName() const;
    const SuiteOptions& getOptions() const;
    const TestStatistics<unsigned int>& getStatistics() const;
    const ExecutionTimer& getTotalTimer() const;

private:
    TestStatistics<unsigned int> statistic;
    TestEventHandler* handler{nullptr};

    std::string name;
    SuiteOptions options;
    std::vector<std::unique_ptr<InternalTest>> tests;

    ExecutionTimer totalTimer;
    ExecutionTimer hookTimer;
    ExecutionTimer testTimer;

    HookManager hookManager;
    TestManager testManager;
    TestFilter testFilter = nullptr;

    std::unique_ptr<LifecycleSuite> fixture;
};

}

#endif
