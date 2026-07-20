#include "ConfigurationErrorStore.hpp"

#include <algorithm>
#include <mutex>
#include <utility>

namespace Testament::detail {

ConfigurationErrorStore::Id ConfigurationErrorStore::record(std::string error) {
    std::unique_lock lock(mMutex);
    const auto id = mNextId++;
    mRecordedErrors.emplace_back(id, std::move(error));
    return id;
}

void ConfigurationErrorStore::remove(Id id) {
    std::unique_lock lock(mMutex);
    std::erase_if(mRecordedErrors, [id](const auto& entry) {
        return entry.first == id;
    });
}

std::vector<std::string> ConfigurationErrorStore::errors() const {
    std::shared_lock lock(mMutex);
    std::vector<std::string> errors;
    errors.reserve(mRecordedErrors.size());
    for (const auto& entry : mRecordedErrors) {
        errors.push_back(entry.second);
    }
    return errors;
}

}
