#ifndef TESTAMENT_SUITEOPTIONS_HPP
#define TESTAMENT_SUITEOPTIONS_HPP

#include "Testament/Execution.hpp"

#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace Testament {

class SuiteOptions {
public:
    using Attribute = std::pair<std::string, std::string>;

    SuiteOptions();
    ~SuiteOptions();

    SuiteOptions(const SuiteOptions& other);
    SuiteOptions& operator=(const SuiteOptions& other);
    SuiteOptions(SuiteOptions&&) noexcept;
    SuiteOptions& operator=(SuiteOptions&&) noexcept;

    SuiteOptions& order(int value);
    SuiteOptions& tag(std::string value);
    SuiteOptions& attribute(std::string key, std::string value);
    SuiteOptions& execution(Execution value);

    [[nodiscard]] std::optional<int> order() const noexcept;
    [[nodiscard]] std::span<const std::string> tags() const noexcept;
    [[nodiscard]] std::span<const Attribute> attributes() const noexcept;
    [[nodiscard]] std::optional<std::string_view> attribute(std::string_view key) const noexcept;
    [[nodiscard]] Execution execution() const noexcept;

private:
    class Impl;
    std::shared_ptr<Impl> impl;
};

}

#endif
