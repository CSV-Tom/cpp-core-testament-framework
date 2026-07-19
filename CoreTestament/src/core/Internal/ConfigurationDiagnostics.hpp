#ifndef TESTAMENT_INTERNAL_CONFIGURATIONDIAGNOSTICS_HPP
#define TESTAMENT_INTERNAL_CONFIGURATIONDIAGNOSTICS_HPP

#include <shared_mutex>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace Testament {

class ConfigurationDiagnostics {
public:
    using Id = std::uint64_t;

    [[nodiscard]] Id record(std::string error);
    void remove(Id id);
    [[nodiscard]] std::vector<std::string> errors() const;

private:
    mutable std::shared_mutex mutex;
    Id nextId{1};
    std::vector<std::pair<Id, std::string>> recordedErrors;
};

}

#endif
