#pragma once
#include "Testament/TestEventHandler.hpp"
#include <exception>
#include <functional>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Testament::detail {

class CompositeTestEventHandler final : public TestEventHandler {
public:
    void add(TestEventHandler* handler) {
        if (handler) mHandlers.push_back(handler);
    }

    [[nodiscard]] std::expected<void, std::string> configure(Arguments arguments) override {
        std::scoped_lock lock(mMutex);
        mErrors.clear();
        for (auto* handler : mHandlers) {
            try {
                if (auto result = handler->configure(arguments); !result) {
                    recordError("configure", result.error());
                }
            } catch (const std::exception& error) {
                recordError("configure", error.what());
            } catch (...) {
                recordError("configure", "unknown non-standard exception");
            }
        }
        if (!mErrors.empty()) return std::unexpected(joinErrors(mErrors));
        return {};
    }

    void onStartReport(unsigned int suiteCount) override {
        dispatch("onStartReport", [suiteCount](auto& handler) {
            handler.onStartReport(suiteCount);
        });
    }
    void onSuiteStart(const SuiteInfo& suite) override {
        dispatch("onSuiteStart", [&suite](auto& handler) {
            handler.onSuiteStart(suite);
        });
    }
    void onSuiteEnd(const SuiteInfo& suite) override {
        dispatch("onSuiteEnd", [&suite](auto& handler) {
            handler.onSuiteEnd(suite);
        });
    }
    void onSuiteAbort(const SuiteInfo& suite, std::string_view message) override {
        dispatch("onSuiteAbort", [&suite, message](auto& handler) {
            handler.onSuiteAbort(suite, message);
        });
    }
    void onTestStart(const SuiteInfo& suite, const TestInfo& test) override {
        dispatch("onTestStart", [&suite, &test](auto& handler) {
            handler.onTestStart(suite, test);
        });
    }
    void onTestPassed(const SuiteInfo& suite, const TestInfo& test) override {
        dispatch("onTestPassed", [&suite, &test](auto& handler) {
            handler.onTestPassed(suite, test);
        });
    }
    void onTestFailed(const SuiteInfo& suite, const TestInfo& test) override {
        dispatch("onTestFailed", [&suite, &test](auto& handler) {
            handler.onTestFailed(suite, test);
        });
    }
    void onTestSkipped(const SuiteInfo& suite, const TestInfo& test) override {
        dispatch("onTestSkipped", [&suite, &test](auto& handler) {
            handler.onTestSkipped(suite, test);
        });
    }
    void onEnvironmentError(std::string_view phase, std::string_view message) override {
        dispatch("onEnvironmentError", [=](auto& handler) {
            handler.onEnvironmentError(phase, message);
        });
    }
    void onFinalReport(const RunSummary& summary) override {
        dispatch("onFinalReport", [&summary](auto& handler) {
            handler.onFinalReport(summary);
        });
    }

    [[nodiscard]] std::string errorMessage() const override {
        std::scoped_lock lock(mMutex);
        auto collectedErrors = mErrors;
        for (auto* handler : mHandlers) {
            try {
                if (auto error = handler->errorMessage(); !error.empty()) {
                    collectedErrors.push_back(std::move(error));
                }
            } catch (const std::exception& error) {
                collectedErrors.emplace_back("errorMessage failed: " + std::string{error.what()});
            } catch (...) {
                collectedErrors.emplace_back("errorMessage failed: unknown non-standard exception");
            }
        }
        return joinErrors(collectedErrors);
    }

private:
    template <typename Callback>
    void dispatch(std::string_view callbackName, Callback&& callback) {
        std::scoped_lock lock(mMutex);
        for (auto* handler : mHandlers) {
            try {
                std::invoke(callback, *handler);
            } catch (const std::exception& error) {
                recordError(callbackName, error.what());
            } catch (...) {
                recordError(callbackName, "unknown non-standard exception");
            }
        }
    }

    void recordError(std::string_view callbackName, std::string_view message) {
        mErrors.emplace_back(std::string{callbackName} + " failed: " + std::string{message});
    }

    static std::string joinErrors(const std::vector<std::string>& messages) {
        std::string result;
        for (const auto& message : messages) {
            if (!result.empty()) result += '\n';
            result += message;
        }
        return result;
    }

    std::vector<TestEventHandler*> mHandlers;
    std::vector<std::string> mErrors;
    mutable std::mutex mMutex;
};

}
