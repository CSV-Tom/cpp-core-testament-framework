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

    // Variante 1: Einzelner Filter
    template <typename Predicate>
    [[nodiscard]] auto getSuitesByFilter(Predicate&& filter) const {
        std::shared_lock lock(registryMutex);
        return registeredSuites | std::views::filter(std::forward<Predicate>(filter));
    }

    /*
    [[nodiscard]] auto getSuitesByFilter(const std::vector<std::function<bool(const std::shared_ptr<InternalSuite>&)>>& filters) const {
        std::shared_lock lock(registryMutex);

        // Initiale Range-View über alle registrierten Suiten
        auto filteredSuites = registeredSuites | std::views::all;

        // Anwenden aller Filter durch Verkettung
        for (const auto& filter : filters) {
            filteredSuites = filteredSuites | std::views::filter(filter); // ✅ Jetzt korrekt
        }

        return filteredSuites; // Gibt eine View zurück
    }
    */




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


/*
#ifndef TESTFRAMEWORK_TESTAMENT_INTERNALREGISTRY_HPP
#define TESTFRAMEWORK_TESTAMENT_INTERNALREGISTRY_HPP

#include <memory>
#include <mutex>
#include <vector>
#include <functional>

namespace Testament {

class Suite;

class InternalRegistry {
public:
    static InternalRegistry& getInstance();

    std::shared_ptr<Suite> registerSuite(std::shared_ptr<Suite> suite);

    std::vector<std::shared_ptr<Suite>> getAllSuites();
    std::vector<std::shared_ptr<Suite>> getSuitesByFilter(std::function<bool(const std::shared_ptr<Suite>&)> filter);


private:
    InternalRegistry() = default;

    InternalRegistry(const InternalRegistry&) = delete;
    InternalRegistry& operator=(const InternalRegistry&) = delete;

    std::vector<std::shared_ptr<Suite>> registeredSuites;
    std::mutex registryMutex;
};

}

#endif
*/