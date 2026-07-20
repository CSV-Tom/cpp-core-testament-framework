#ifndef TESTAMENT_INTERNAL_OPTIONSSTORAGE_HPP
#define TESTAMENT_INTERNAL_OPTIONSSTORAGE_HPP

#include "Testament/Execution.hpp"

#include <algorithm>
#include <optional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Testament::detail {

template <typename Attribute>
class OptionsStorage {
public:
    void setOrder(int value) noexcept { order_ = value; }
    void addTag(std::string value) { tags_.push_back(std::move(value)); }

    void setAttribute(std::string key, std::string value) {
        const auto existing = std::ranges::find(attributes_, key, &Attribute::first);
        if (existing == attributes_.end()) {
            attributes_.emplace_back(std::move(key), std::move(value));
        } else {
            existing->second = std::move(value);
        }
    }

    [[nodiscard]] std::optional<std::string_view> findAttribute(
        std::string_view key
    ) const noexcept {
        const auto existing = std::ranges::find(attributes_, key, &Attribute::first);
        if (existing == attributes_.end()) return std::nullopt;
        return existing->second;
    }

    void setExecution(Execution value) noexcept { execution_ = value; }

    [[nodiscard]] std::optional<int> order() const noexcept { return order_; }
    [[nodiscard]] std::span<const std::string> tags() const noexcept { return tags_; }
    [[nodiscard]] std::span<const Attribute> attributes() const noexcept {
        return attributes_;
    }
    [[nodiscard]] Execution execution() const noexcept { return execution_; }

private:
    std::optional<int> order_;
    std::vector<std::string> tags_;
    std::vector<Attribute> attributes_;
    Execution execution_{Execution::Inherit};
};

template <typename Storage>
Storage& writeOptions(std::shared_ptr<Storage>& storage) {
    if (!storage) {
        storage = std::make_shared<Storage>();
    } else if (storage.use_count() != 1) {
        storage = std::make_shared<Storage>(*storage);
    }
    return *storage;
}

template <typename Storage>
const Storage& readOptions(const std::shared_ptr<Storage>& storage) noexcept {
    static const Storage empty;
    return storage ? *storage : empty;
}

}

#endif
