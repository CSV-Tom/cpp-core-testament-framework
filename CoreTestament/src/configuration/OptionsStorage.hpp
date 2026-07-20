#pragma once

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
    void setOrder(int value) noexcept { mOrder = value; }
    void addTag(std::string value) { mTags.push_back(std::move(value)); }

    void setAttribute(std::string key, std::string value) {
        const auto existing = std::ranges::find(mAttributes, key, &Attribute::first);
        if (existing == mAttributes.end()) {
            mAttributes.emplace_back(std::move(key), std::move(value));
        } else {
            existing->second = std::move(value);
        }
    }

    [[nodiscard]] std::optional<std::string_view> findAttribute(
        std::string_view key
    ) const noexcept {
        const auto existing = std::ranges::find(mAttributes, key, &Attribute::first);
        if (existing == mAttributes.end()) return std::nullopt;
        return existing->second;
    }

    void setExecution(Execution value) noexcept { mExecution = value; }

    [[nodiscard]] std::optional<int> order() const noexcept { return mOrder; }
    [[nodiscard]] std::span<const std::string> tags() const noexcept { return mTags; }
    [[nodiscard]] std::span<const Attribute> attributes() const noexcept {
        return mAttributes;
    }
    [[nodiscard]] Execution execution() const noexcept { return mExecution; }

private:
    std::optional<int> mOrder;
    std::vector<std::string> mTags;
    std::vector<Attribute> mAttributes;
    Execution mExecution{Execution::Inherit};
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
