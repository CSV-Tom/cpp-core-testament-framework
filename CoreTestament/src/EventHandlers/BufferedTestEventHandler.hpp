#pragma once

#include "Testament/TestEventHandler.hpp"

#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Testament::detail {

class BufferedTestEventHandler final : public TestEventHandler {
public:
    void onSuiteStart(const SuiteInfo& suite) override {
        record([suite](TestEventHandler& handler) { handler.onSuiteStart(suite); });
    }

    void onSuiteEnd(const SuiteInfo& suite) override {
        record([suite](TestEventHandler& handler) { handler.onSuiteEnd(suite); });
    }

    void onSuiteAbort(const SuiteInfo& suite, std::string_view message) override {
        record([suite, message = std::string{message}](TestEventHandler& handler) {
            handler.onSuiteAbort(suite, message);
        });
    }

    void onTestStart(const SuiteInfo& suite, const TestInfo& test) override {
        record([suite, test](TestEventHandler& handler) { handler.onTestStart(suite, test); });
    }

    void onTestPassed(const SuiteInfo& suite, const TestInfo& test) override {
        record([suite, test](TestEventHandler& handler) { handler.onTestPassed(suite, test); });
    }

    void onTestFailed(const SuiteInfo& suite, const TestInfo& test) override {
        record([suite, test](TestEventHandler& handler) { handler.onTestFailed(suite, test); });
    }

    void onTestSkipped(const SuiteInfo& suite, const TestInfo& test) override {
        record([suite, test](TestEventHandler& handler) { handler.onTestSkipped(suite, test); });
    }

    void replay(TestEventHandler& handler) {
        for (auto& event : events) event(handler);
        events.clear();
    }

private:
    using Event = std::move_only_function<void(TestEventHandler&)>;

    template <typename Callback>
    void record(Callback&& callback) {
        events.emplace_back(std::forward<Callback>(callback));
    }

    std::vector<Event> events;
};

}
