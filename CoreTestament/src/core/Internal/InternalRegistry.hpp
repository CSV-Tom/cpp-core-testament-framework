#ifndef TESTFRAMEWORK_TESTAMENT_INTERNALREGISTRY_HPP
#define TESTFRAMEWORK_TESTAMENT_INTERNALREGISTRY_HPP

#include <memory>
#include <shared_mutex>  // Für std::shared_mutex
#include <vector>
#include <functional>
#include <ranges>

#include "InternalSuite.hpp"
namespace Testament {


class InternalRegistry {
public:
    static InternalRegistry& getInstance();

    std::shared_ptr<InternalSuite> registerSuite(std::shared_ptr<InternalSuite> suite);

    [[nodiscard]] std::vector<std::shared_ptr<InternalSuite>> getAllSuites() const;

    template <typename Predicate>
    [[nodiscard]] auto getSuitesByFilter(Predicate&& filter) const {
        std::shared_lock lock(registryMutex);
        return registeredSuites | std::views::filter(std::forward<Predicate>(filter));
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