#ifndef TESTFRAMEWORK_TESTAMENT_HOOKMANAGER_HPP
#define TESTFRAMEWORK_TESTAMENT_HOOKMANAGER_HPP

#include <exception>
#include <functional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Testament {

class HookManager {
public:
    using Callback = std::move_only_function<void()>;

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

    bool invoke(Callback& hook, std::string_view hookName) {
        return invokeHook(hook, hookName);
    }

    [[nodiscard]] const std::vector<std::string>& errors() const noexcept {
        return errors_;
    }

    void resetErrors() {
        errors_.clear();
    }

    [[nodiscard]] bool hasPerTestHooks() const noexcept {
        return static_cast<bool>(beforeEachHook) || static_cast<bool>(afterEachHook);
    }

private:
    std::vector<std::string> errors_;

    Callback beforeSuiteHook;
    Callback beforeEachHook;
    Callback afterEachHook;
    Callback afterSuiteHook;

    bool invokeHook(Callback& hook, std::string_view hookName) {
        if (hook) {
            try {
                hook();
                return true;
            } catch (const std::exception& e) {
                std::ostringstream oss;
                oss << "Error in " << hookName << ": " << e.what();
                errors_.push_back(oss.str());
                return false;
            } catch (...) {
                std::ostringstream oss;
                oss << "Unknown error in " << hookName;
                errors_.push_back(oss.str());
                return false;
            }
        }
        return true;
    }
};

}

#endif
