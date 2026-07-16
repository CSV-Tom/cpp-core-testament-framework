#pragma once
#include "Testament/TestEventHandler.hpp"
#include <vector>

class TestEventHandlerChain : public Testament::TestEventHandler {
public:
    void add(TestEventHandler* handler) {
        if (handler) handlers.push_back(handler);
    }

    void onStartReport(unsigned int suiteCount) override {
        for (auto* h : handlers) h->onStartReport(suiteCount);
    }
    void onSuiteStart(const SuiteInfo& suite) override {
        for (auto* h : handlers) h->onSuiteStart(suite);
    }
    void onSuiteEnd(const SuiteInfo& suite) override {
        for (auto* h : handlers) h->onSuiteEnd(suite);
    }
    void onSuiteAbort(const SuiteInfo& suite, const std::string& message) override {
        for (auto* h : handlers) h->onSuiteAbort(suite, message);
    }
    void onTestPassed(const SuiteInfo& suite, const TestInfo& test) override {
        for (auto* h : handlers) h->onTestPassed(suite, test);
    }
    void onTestFailed(const SuiteInfo& suite, const TestInfo& test) override {
        for (auto* h : handlers) h->onTestFailed(suite, test);
    }
    void onTestSkipped(const SuiteInfo& suite, const TestInfo& test) override {
        for (auto* h : handlers) h->onTestSkipped(suite, test);
    }
    void onFinalReport(unsigned int suites, unsigned int passed, unsigned int failed, unsigned int skipped) override {
        for (auto* h : handlers) h->onFinalReport(suites, passed, failed, skipped);
    }

private:
    std::vector<TestEventHandler*> handlers;
};
