#ifndef TESTAMENT_INTERNAL_OPTIONSSTORAGE_HPP
#define TESTAMENT_INTERNAL_OPTIONSSTORAGE_HPP

#include "Testament/Execution.hpp"

#include <algorithm>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Testament {

template <typename Attribute>
class OptionsStorage {
public:
    void setAttribute(std::string key, std::string value) {
        const auto existing = std::ranges::find(attributes, key, &Attribute::first);
        if (existing == attributes.end()) {
            attributes.emplace_back(std::move(key), std::move(value));
        } else {
            existing->second = std::move(value);
        }
    }

    [[nodiscard]] std::optional<std::string_view> findAttribute(
        std::string_view key
    ) const noexcept {
        const auto existing = std::ranges::find(attributes, key, &Attribute::first);
        if (existing == attributes.end()) return std::nullopt;
        return existing->second;
    }

    std::optional<int> order;
    std::vector<std::string> tags;
    std::vector<Attribute> attributes;
    Execution execution{Execution::Inherit};
};

}

#endif
