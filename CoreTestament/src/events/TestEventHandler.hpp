#include <string>
#include <vector>
#include <memory>

class TestCase;
class TestSuite;

class TestEventHandler {
public:
    virtual ~TestEventHandler() = default;

    virtual void onStartReport(const std::vector<std::shared_ptr<TestSuite>>& suites) {}
    virtual void onFinalReport(const std::vector<std::shared_ptr<TestSuite>>& suites) {}

    virtual void onSuiteStart(const TestSuite& suite) {}
    virtual void onSuiteEnd(const TestSuite& suite) {}
    virtual void onSuiteAbort(const std::string& message) {}
    virtual void onSuiteSummary(const TestSuite& suite, const std::vector<std::shared_ptr<TestCase>>& testCases) {}

    virtual void onLogEvent(const std::string message) {}

    virtual void onTestPassed(const TestSuite& suite, const TestCase& testCase) {}
    virtual void onTestSkipped(const TestSuite& suite, const TestCase& testCase) {}
    virtual void onTestFailed(const TestSuite& suite, const TestCase& testCase, std::exception_ptr exception) {}

};