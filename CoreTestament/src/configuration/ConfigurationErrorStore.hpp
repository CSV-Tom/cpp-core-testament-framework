#pragma once

#include <shared_mutex>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace Testament::detail {

class ConfigurationErrorStore {
public:
    using Id = std::uint64_t;

    [[nodiscard]] Id record(std::string error);
    void remove(Id id);
    [[nodiscard]] std::vector<std::string> errors() const;

private:
    mutable std::shared_mutex mMutex;
    Id mNextId{1};
    std::vector<std::pair<Id, std::string>> mRecordedErrors;
};

}
