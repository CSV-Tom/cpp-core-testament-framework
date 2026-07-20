#pragma once

#include "configuration/ConfigurationErrorStore.hpp"

#include <functional>
#include <memory>
#include <shared_mutex>
#include <string>
#include <vector>

namespace Testament::detail {

class SuiteInstance;

class SuiteRegistry {
public:
    static SuiteRegistry& instance();

    std::shared_ptr<SuiteInstance> registerSuite(std::shared_ptr<SuiteInstance> suite);
    void unregisterSuite(const std::shared_ptr<SuiteInstance>& suite);

    [[nodiscard]] std::vector<std::shared_ptr<SuiteInstance>> suites() const;
    [[nodiscard]] ConfigurationErrorStore::Id recordConfigurationError(std::string error);
    void removeConfigurationError(ConfigurationErrorStore::Id id);
    [[nodiscard]] std::vector<std::string> configurationErrors() const;

    template <typename Predicate>
    [[nodiscard]] std::vector<std::shared_ptr<SuiteInstance>> suitesBy(
        Predicate&& filter
    ) const {
        std::shared_lock lock(mMutex);
        std::vector<std::shared_ptr<SuiteInstance>> matchingSuites;
        matchingSuites.reserve(mSuites.size());
        for (const auto& suite : mSuites) {
            if (std::invoke(filter, suite)) {
                matchingSuites.push_back(suite);
            }
        }
        return matchingSuites;
    }
private:
    SuiteRegistry() = default;

    SuiteRegistry(const SuiteRegistry&) = delete;
    SuiteRegistry& operator=(const SuiteRegistry&) = delete;

    mutable std::shared_mutex mMutex;
    std::vector<std::shared_ptr<SuiteInstance>> mSuites;
    ConfigurationErrorStore mConfigurationErrorStore;
};

}
