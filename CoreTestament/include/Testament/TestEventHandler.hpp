#ifndef TESTAMENT_TESTEVENTHANDLER_HPP
#define TESTAMENT_TESTEVENTHANDLER_HPP

#include <chrono>
#include <exception>
#include <string>

namespace Testament {

class TestEventHandler {
public:
    struct TestInfo {
        std::string name;
        std::chrono::duration<double> duration;
        std::exception_ptr exception;
    };

    struct SuiteInfo {
        std::string name;
        unsigned int passed{};
        unsigned int failed{};
        unsigned int skipped{};
    };

    virtual ~TestEventHandler() = default;

    virtual void onStartReport(unsigned int) {}
    virtual void onSuiteStart(const SuiteInfo&) {}
    virtual void onSuiteEnd(const SuiteInfo&) {}
    virtual void onSuiteAbort(const SuiteInfo&, const std::string&) {}
    virtual void onTestPassed(const SuiteInfo&, const TestInfo&) {}
    virtual void onTestFailed(const SuiteInfo&, const TestInfo&) {}
    virtual void onTestSkipped(const SuiteInfo&, const TestInfo&) {}
    virtual void onFinalReport(unsigned int, unsigned int, unsigned int, unsigned int) {}

    [[nodiscard]] virtual std::string errorMessage() const { return {}; }
};

}

#endif
