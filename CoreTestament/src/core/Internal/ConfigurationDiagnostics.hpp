#ifndef TESTAMENT_INTERNAL_CONFIGURATIONDIAGNOSTICS_HPP
#define TESTAMENT_INTERNAL_CONFIGURATIONDIAGNOSTICS_HPP

#include <shared_mutex>
#include <string>
#include <vector>

namespace Testament {

class ConfigurationDiagnostics {
public:
    void record(std::string error);
    [[nodiscard]] std::vector<std::string> errors() const;

private:
    mutable std::shared_mutex mutex;
    std::vector<std::string> recordedErrors;
};

}

#endif
