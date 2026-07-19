#ifndef TESTAMENT_DETAIL_LOCATEDNAME_HPP
#define TESTAMENT_DETAIL_LOCATEDNAME_HPP

#include <concepts>
#include <source_location>
#include <string>
#include <string_view>
#include <utility>

namespace Testament::detail {

class LocatedName {
public:
    template <typename Name>
    requires std::constructible_from<std::string_view, Name&&>
    LocatedName(Name&& name, std::source_location location = std::source_location::current())
        : value_(std::string_view{std::forward<Name>(name)}), location_(location) {}

    [[nodiscard]] const std::string& value() const noexcept { return value_; }
    [[nodiscard]] std::source_location location() const noexcept { return location_; }

private:
    std::string value_;
    std::source_location location_;
};

}

#endif
