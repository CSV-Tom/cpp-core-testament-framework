#include "SuiteRegistry.hpp"

#include <algorithm>
#include <mutex>
#include <stdexcept>
#include <string>
#include <utility>

#include "SuiteInstance.hpp"


namespace Testament::detail {

SuiteRegistry& SuiteRegistry::instance() {
    static SuiteRegistry instance;
    return instance;
}

void SuiteRegistry::unregisterSuite(const std::shared_ptr<SuiteInstance>& suite) {
    std::unique_lock lock(mMutex);
    std::erase(mSuites, suite);
}

std::shared_ptr<SuiteInstance> SuiteRegistry::registerSuite(std::shared_ptr<SuiteInstance> suite) {
    if (!suite) {
        throw std::invalid_argument("Cannot register a null Suite");
    }

    std::unique_lock lock(mMutex);
    if (std::ranges::any_of(mSuites, [&suite](const auto& registered) {
        return registered->name() == suite->name();
    })) {
        throw std::logic_error("Suite name must be unique: " + suite->name());
    }
    mSuites.push_back(std::move(suite));

    return mSuites.back();
}

std::vector<std::shared_ptr<SuiteInstance>> SuiteRegistry::suites() const {
    std::shared_lock lock(mMutex);
    return mSuites;
}

ConfigurationErrorStore::Id SuiteRegistry::recordConfigurationError(std::string error) {
    return mConfigurationErrorStore.record(std::move(error));
}

void SuiteRegistry::removeConfigurationError(ConfigurationErrorStore::Id id) {
    mConfigurationErrorStore.remove(id);
}

std::vector<std::string> SuiteRegistry::configurationErrors() const {
    return mConfigurationErrorStore.errors();
}

}
