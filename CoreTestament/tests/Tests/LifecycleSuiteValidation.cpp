#include "Testament/Testament.hpp"

#include <vector>
#include <string>

namespace Testament {

class LifecycleSuiteValidation : public LifecycleSuite {
public:
    void beforeAll() override {
        lifecycleLog.push_back("beforeAll");
    }

    void beforeEach() override {
        lifecycleLog.push_back("beforeEach");
    }

    void afterEach() override {
        lifecycleLog.push_back("afterEach");
    }

    void afterAll() override {
        lifecycleLog.push_back("afterAll");

        if (lifecycleLog.front() != "beforeAll") {
            throw std::runtime_error("beforeAll must be the first call");
        }
        if (lifecycleLog.back() != "afterAll") {
            throw std::runtime_error("afterAll must be the last call");
        }

        int beforeEachCalls = 0;
        int afterEachCalls = 0;
        for (const auto& entry : lifecycleLog) {
            if (entry == "beforeEach") beforeEachCalls++;
            if (entry == "afterEach") afterEachCalls++;
        }

        if (beforeEachCalls != executedTests) {
            throw std::runtime_error("beforeEach must be called for each test");
        }
        if (afterEachCalls != executedTests) {
            throw std::runtime_error("afterEach must be called for each test");
        }
    }

    void incrementTestCounter() { 
        executedTests++; 
    }

    int executedTests = 0;

private:
    std::vector<std::string> lifecycleLog;
};

static auto suite = makeSuite<LifecycleSuiteValidation>("Lifecycle Validation Suite",
    makeTest<LifecycleSuiteValidation>("Basic Test", [](LifecycleSuiteValidation& suite) {
        suite.incrementTestCounter();
    }),
    makeTest<LifecycleSuiteValidation>("Additional Test", [](LifecycleSuiteValidation& suite) {
        suite.incrementTestCounter();
    })
);

}
