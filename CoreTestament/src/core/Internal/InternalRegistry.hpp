#ifndef TESTFRAMEWORK_TESTAMENT_INTERNALREGISTRY_HPP
#define TESTFRAMEWORK_TESTAMENT_INTERNALREGISTRY_HPP

#include <memory>
#include <shared_mutex>  // Für std::shared_mutex
#include <vector>
#include <functional>

#include "InternalSuite.hpp"
namespace Testament {


class InternalRegistry {
public:
    static InternalRegistry& getInstance();

    std::shared_ptr<InternalSuite> registerSuite(std::shared_ptr<InternalSuite> suite);

    [[nodiscard]] std::vector<std::shared_ptr<InternalSuite>> getAllSuites() const;

    template <typename Predicate>
    [[nodiscard]] std::vector<std::shared_ptr<InternalSuite>> getSuitesByFilter(Predicate&& filter) const {
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




    static std::function<bool(const std::shared_ptr<InternalSuite>&)> createFilter(const std::string& type, const std::string& value = "");

private:
    InternalRegistry() = default;

    InternalRegistry(const InternalRegistry&) = delete;
    InternalRegistry& operator=(const InternalRegistry&) = delete;

    mutable std::shared_mutex registryMutex;
    std::vector<std::shared_ptr<InternalSuite>> registeredSuites;
};

}

#endif
