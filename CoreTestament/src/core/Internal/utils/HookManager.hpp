#ifndef TESTFRAMEWORK_TESTAMENT_HOOKMANAGER_HPP
#define TESTFRAMEWORK_TESTAMENT_HOOKMANAGER_HPP

#include "ExecutionTimer.hpp"

#include <functional>
#include <vector>
#include <string>
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

    bool invokeBeforeSuiteHook() {
        return invokeHook(beforeSuiteHook, "beforeSuite");
    }
    bool invokeBeforeEachHook() {
        return invokeHook(beforeEachHook, "beforeEach");
    }
    bool invokeAfterEachHook() {
        return invokeHook(afterEachHook, "afterEach");
    }
    bool invokeAfterSuiteHook() {
        return invokeHook(afterSuiteHook, "afterSuite");
    }

    [[nodiscard]] const std::vector<std::string>& getErrors() const noexcept {
        return errors;
    }

    void resetErrors() {
        errors.clear();
    }

private:
    ExecutionTimer& hookTimer;
    std::vector<std::string> errors;

    Callback beforeSuiteHook;
    Callback beforeEachHook;
    Callback afterEachHook;
    Callback afterSuiteHook;

    bool invokeHook(const Callback& hook, const std::string& hookName) {
        if (hook) {
            try {
                hookTimer.start();
                hook();
                hookTimer.stop();
                return true;
            } catch (const std::exception& e) {
                hookTimer.stop();
                std::ostringstream oss;
                oss << "Error in " << hookName << ": " << e.what();
                errors.push_back(oss.str());
                return false;
            } catch (...) {
                hookTimer.stop();
                std::ostringstream oss;
                oss << "Unknown error in " << hookName;
                errors.push_back(oss.str());
                return false;
            }
        }
        return true;
    }
};

}

#endif
