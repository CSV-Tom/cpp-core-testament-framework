#ifndef TESTFRAMEWORK_TESTAMENT_INTERNALSUITE_HPP
#define TESTFRAMEWORK_TESTAMENT_INTERNALSUITE_HPP

#include "Testament/Suite.hpp"

#include <vector>
#include <functional>
#include <variant>
#include <regex>

#include "utils/ExecutionTimer.hpp"
#include "utils/TestStatistics.hpp"
#include "utils/HookManager.hpp"
#include "utils/TestManager.hpp"
#include "EventHandlers/TestEventHandler.hpp"


namespace Testament {

class InternalTest;

class InternalSuite : public Suite {
public:
    using Callback = std::function<void()>;
    using TestFilter = std::function<bool(const std::string&)>;

    explicit InternalSuite(const std::string& name);
    explicit InternalSuite(const std::string& name, std::shared_ptr<Suite> suite);
    ~InternalSuite() override;

    void addTest(const std::shared_ptr<Test>& test) override;

    void setBeforeSuite(Callback callback);
    void setBeforeEach(Callback callback);
    void setAfterEach(Callback callback);
    void setAfterSuite(Callback callback);

    void setTestFilter(std::variant<std::string, std::regex> filter);
    void setHandler(TestEventHandler* handler);

    void run();

    const std::string& getName() const;
    const TestStatistics<unsigned int>& getStatistics() const;
    const ExecutionTimer& getTotalTimer() const;
private:
    TestStatistics<unsigned int> statistic;
    TestEventHandler* handler{nullptr};

    std::string name;
    std::vector<std::shared_ptr<InternalTest>> tests;

    ExecutionTimer totalTimer;
    ExecutionTimer hookTimer;
    ExecutionTimer testTimer;

    HookManager hookManager;
    TestManager testManager;
    TestFilter testFilter = nullptr;

    std::shared_ptr<Suite> suite = nullptr;
};

}

#endif