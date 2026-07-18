#include "ConfigurationDiagnostics.hpp"

#include <mutex>
#include <utility>

namespace Testament {

void ConfigurationDiagnostics::record(std::string error) {
    std::unique_lock lock(mutex);
    recordedErrors.push_back(std::move(error));
}

std::vector<std::string> ConfigurationDiagnostics::errors() const {
    std::shared_lock lock(mutex);
    return recordedErrors;
}

}
