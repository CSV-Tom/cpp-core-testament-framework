#pragma once

#include <exception>
#include <functional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Testament::detail {

class HookExecutor {
public:
    using Callback = std::move_only_function<void()>;

    void setBeforeSuite(Callback callback) {
        mBeforeSuiteHook = std::move(callback);
    }
    void setBeforeEach(Callback callback) {
        mBeforeEachHook = std::move(callback);
    }
    void setAfterEach(Callback callback) {
        mAfterEachHook = std::move(callback);
    }
    void setAfterSuite(Callback callback) {
        mAfterSuiteHook = std::move(callback);
    }

    bool invokeBeforeSuiteHook() {
        return invokeHook(mBeforeSuiteHook, "beforeSuite");
    }
    bool invokeBeforeEachHook() {
        return invokeHook(mBeforeEachHook, "beforeEach");
    }
    bool invokeAfterEachHook() {
        return invokeHook(mAfterEachHook, "afterEach");
    }
    bool invokeAfterSuiteHook() {
        return invokeHook(mAfterSuiteHook, "afterSuite");
    }

    bool invoke(Callback& hook, std::string_view hookName) {
        return invokeHook(hook, hookName);
    }

    [[nodiscard]] const std::vector<std::string>& errors() const noexcept {
        return mErrors;
    }

    void resetErrors() {
        mErrors.clear();
    }

    [[nodiscard]] bool hasPerTestHooks() const noexcept {
        return static_cast<bool>(mBeforeEachHook) || static_cast<bool>(mAfterEachHook);
    }

private:
    std::vector<std::string> mErrors;

    Callback mBeforeSuiteHook;
    Callback mBeforeEachHook;
    Callback mAfterEachHook;
    Callback mAfterSuiteHook;

    bool invokeHook(Callback& hook, std::string_view hookName) {
        if (hook) {
            try {
                hook();
                return true;
            } catch (const std::exception& e) {
                std::ostringstream oss;
                oss << "Error in " << hookName << ": " << e.what();
                mErrors.push_back(oss.str());
                return false;
            } catch (...) {
                std::ostringstream oss;
                oss << "Unknown error in " << hookName;
                mErrors.push_back(oss.str());
                return false;
            }
        }
        return true;
    }
};

}
