#include "ConfigurationDiagnostics.hpp"

#include <algorithm>
#include <mutex>
#include <utility>

namespace Testament {

ConfigurationDiagnostics::Id ConfigurationDiagnostics::record(std::string error) {
    std::unique_lock lock(mutex);
    const auto id = nextId++;
    recordedErrors.emplace_back(id, std::move(error));
    return id;
}

void ConfigurationDiagnostics::remove(Id id) {
    std::unique_lock lock(mutex);
    std::erase_if(recordedErrors, [id](const auto& entry) {
        return entry.first == id;
    });
}

std::vector<std::string> ConfigurationDiagnostics::errors() const {
    std::shared_lock lock(mutex);
    std::vector<std::string> errors;
    errors.reserve(recordedErrors.size());
    for (const auto& entry : recordedErrors) {
        errors.push_back(entry.second);
    }
    return errors;
}

}
