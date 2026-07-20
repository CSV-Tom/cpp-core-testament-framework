#ifndef TESTAMENT_INTERNAL_TESTCATALOG_HPP
#define TESTAMENT_INTERNAL_TESTCATALOG_HPP

#include <cstdint>
#include <memory>
#include <optional>
#include <ostream>
#include <string_view>
#include <vector>

namespace Testament {
class InternalSuite;
}

namespace Testament::detail {

class TestCatalog {
public:
    TestCatalog(std::vector<std::shared_ptr<InternalSuite>> suites,
                std::optional<std::string_view> suiteFilter);

    [[nodiscard]] const std::vector<std::shared_ptr<InternalSuite>>& suites() const noexcept;
    [[nodiscard]] std::vector<std::shared_ptr<InternalSuite>> forRun(
        std::optional<std::uint64_t> shuffleSeed
    ) const;
    void list(std::ostream& output, std::string_view testFilter,
              std::string_view expression) const;

private:
    std::vector<std::shared_ptr<InternalSuite>> registeredSuites;
};

}

#endif
