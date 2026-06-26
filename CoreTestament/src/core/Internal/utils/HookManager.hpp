#ifndef TESTFRAMEWORK_TESTAMENT_HOOKMANAGER_HPP
#define TESTFRAMEWORK_TESTAMENT_HOOKMANAGER_HPP

#include "ExecutionTimer.hpp"

#include <functional>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

namespace Testament {

class HookManager {
public:
    using Callback = std::function<void()>;

    explicit HookManager(ExecutionTimer& timer) : hookTimer(timer) {}

    void setBeforeSuite(Callback callback) {
        beforeSuiteHook = std::move(callback);
    }
    void setBeforeEach(Callback callback) {
        beforeEachHook = std::move(callback);
    }
    void setAfterEach(Callback callback) {
        afterEachHook = std::move(callback);
    }
    void setAfterSuite(Callback callback) {
        afterSuiteHook = std::move(callback);
    }

    void invokeBeforeSuiteHook() {
        invokeHook(beforeSuiteHook, "beforeSuite");
    }
    void invokeBeforeEachHook() {
        invokeHook(beforeEachHook, "beforeEach");
    }
    void invokeAfterEachHook() {
        invokeHook(afterEachHook, "afterEach");
    }
    void invokeAfterSuiteHook() {
        invokeHook(afterSuiteHook, "afterSuite");
    }

    void reportErrors() const {
        if (!errors.empty()) {
            std::cerr << "=== Hook Errors ===\n";
            for (const auto& error : errors) {
                std::cerr << error << std::endl;
            }
        }
    }

private:
    ExecutionTimer& hookTimer;
    std::vector<std::string> errors;

    Callback beforeSuiteHook;
    Callback beforeEachHook;
    Callback afterEachHook;
    Callback afterSuiteHook;

    void invokeHook(const Callback& hook, const std::string& hookName) {
        if (hook) {
            try {
                hookTimer.start();
                hook();
                hookTimer.stop();
            } catch (const std::exception& e) {
                hookTimer.stop();
                std::ostringstream oss;
                oss << "Error in " << hookName << ": " << e.what();
                errors.push_back(oss.str());
            } catch (...) {
                hookTimer.stop();
                std::ostringstream oss;
                oss << "Unknown error in " << hookName;
                errors.push_back(oss.str());
            }
        }
    }
};

}

#endif
