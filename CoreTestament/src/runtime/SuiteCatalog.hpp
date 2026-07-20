#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <ostream>
#include <string_view>
#include <vector>

namespace Testament::detail {

class SuiteInstance;

class SuiteCatalog {
public:
    SuiteCatalog(std::vector<std::shared_ptr<SuiteInstance>> suites,
                std::optional<std::string_view> suiteFilter);

    [[nodiscard]] const std::vector<std::shared_ptr<SuiteInstance>>& suites() const noexcept;
    [[nodiscard]] std::vector<std::shared_ptr<SuiteInstance>> forRun(
        std::optional<std::uint64_t> shuffleSeed
    ) const;
    void list(std::ostream& output, std::string_view testFilter,
              std::string_view expression) const;

private:
    std::vector<std::shared_ptr<SuiteInstance>> mSuites;
};

}
