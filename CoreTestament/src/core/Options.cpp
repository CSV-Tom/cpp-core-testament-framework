#include "Testament/Options.hpp"

#include <algorithm>
#include <vector>

namespace Testament {

namespace {

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

    std::optional<std::string_view> findAttribute(std::string_view key) const noexcept {
        const auto existing = std::ranges::find(attributes, key, &Attribute::first);
        if (existing == attributes.end()) {
            return std::nullopt;
        }
        return existing->second;
    }

    std::optional<int> order;
    std::vector<std::string> tags;
    std::vector<Attribute> attributes;
};

}

class SuiteOptions::Impl : public OptionsStorage<Attribute> {};

SuiteOptions::SuiteOptions() : impl(std::make_unique<Impl>()) {}
SuiteOptions::~SuiteOptions() = default;
SuiteOptions::SuiteOptions(const SuiteOptions& other) : impl(std::make_unique<Impl>(*other.impl)) {}
SuiteOptions& SuiteOptions::operator=(const SuiteOptions& other) {
    if (this != &other) {
        *impl = *other.impl;
    }
    return *this;
}
SuiteOptions::SuiteOptions(SuiteOptions&&) noexcept = default;
SuiteOptions& SuiteOptions::operator=(SuiteOptions&&) noexcept = default;

SuiteOptions& SuiteOptions::order(int value) {
    impl->order = value;
    return *this;
}
SuiteOptions& SuiteOptions::tag(std::string value) {
    impl->tags.push_back(std::move(value));
    return *this;
}
SuiteOptions& SuiteOptions::attribute(std::string key, std::string value) {
    impl->setAttribute(std::move(key), std::move(value));
    return *this;
}
std::optional<int> SuiteOptions::order() const noexcept { return impl->order; }
std::span<const std::string> SuiteOptions::tags() const noexcept { return impl->tags; }
std::span<const SuiteOptions::Attribute> SuiteOptions::attributes() const noexcept {
    return impl->attributes;
}
std::optional<std::string_view> SuiteOptions::attribute(std::string_view key) const noexcept {
    return impl->findAttribute(key);
}

class TestOptions::Impl : public OptionsStorage<Attribute> {
public:
    bool disabled{};
    unsigned int retries{};
};

TestOptions::TestOptions() : impl(std::make_unique<Impl>()) {}
TestOptions::~TestOptions() = default;
TestOptions::TestOptions(const TestOptions& other) : impl(std::make_unique<Impl>(*other.impl)) {}
TestOptions& TestOptions::operator=(const TestOptions& other) {
    if (this != &other) {
        *impl = *other.impl;
    }
    return *this;
}
TestOptions::TestOptions(TestOptions&&) noexcept = default;
TestOptions& TestOptions::operator=(TestOptions&&) noexcept = default;

TestOptions& TestOptions::order(int value) {
    impl->order = value;
    return *this;
}
TestOptions& TestOptions::tag(std::string value) {
    impl->tags.push_back(std::move(value));
    return *this;
}
TestOptions& TestOptions::attribute(std::string key, std::string value) {
    impl->setAttribute(std::move(key), std::move(value));
    return *this;
}
TestOptions& TestOptions::disabled(bool value) {
    impl->disabled = value;
    return *this;
}
TestOptions& TestOptions::retries(unsigned int value) {
    impl->retries = value;
    return *this;
}
std::optional<int> TestOptions::order() const noexcept { return impl->order; }
std::span<const std::string> TestOptions::tags() const noexcept { return impl->tags; }
std::span<const TestOptions::Attribute> TestOptions::attributes() const noexcept {
    return impl->attributes;
}
std::optional<std::string_view> TestOptions::attribute(std::string_view key) const noexcept {
    return impl->findAttribute(key);
}
bool TestOptions::isDisabled() const noexcept { return impl->disabled; }
unsigned int TestOptions::retryCount() const noexcept { return impl->retries; }

}
