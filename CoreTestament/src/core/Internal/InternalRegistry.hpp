#ifndef TESTFRAMEWORK_TESTAMENT_INTERNALREGISTRY_HPP
#define TESTFRAMEWORK_TESTAMENT_INTERNALREGISTRY_HPP

#include "ConfigurationDiagnostics.hpp"

#include <functional>
#include <memory>
#include <shared_mutex>
#include <string>
#include <vector>

namespace Testament {

class InternalSuite;

class InternalRegistry {
public:
    static InternalRegistry& getInstance();

    std::shared_ptr<InternalSuite> registerSuite(std::shared_ptr<InternalSuite> suite);
    void unregisterSuite(const std::shared_ptr<InternalSuite>& suite);

    [[nodiscard]] std::vector<std::shared_ptr<InternalSuite>> getAllSuites() const;
    [[nodiscard]] ConfigurationDiagnostics::Id recordConfigurationError(std::string error);
    void removeConfigurationError(ConfigurationDiagnostics::Id id);
    [[nodiscard]] std::vector<std::string> getConfigurationErrors() const;

    template <typename Predicate>
    [[nodiscard]] std::vector<std::shared_ptr<InternalSuite>> getSuitesByFilter(
        Predicate&& filter
    ) const {
        std::shared_lock lock(registryMutex);
        std::vector<std::shared_ptr<InternalSuite>> matchingSuites;
        matchingSuites.reserve(registeredSuites.size());
        for (const auto& suite : registeredSuites) {
            if (std::invoke(filter, suite)) {
                matchingSuites.push_back(suite);
            }
        }
        return matchingSuites;
    }
private:
    InternalRegistry() = default;

    InternalRegistry(const InternalRegistry&) = delete;
    InternalRegistry& operator=(const InternalRegistry&) = delete;

    mutable std::shared_mutex registryMutex;
    std::vector<std::shared_ptr<InternalSuite>> registeredSuites;
    ConfigurationDiagnostics configurationDiagnostics;
};

}

#endif
