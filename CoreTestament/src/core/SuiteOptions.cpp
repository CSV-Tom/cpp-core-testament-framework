#include "Testament/SuiteOptions.hpp"
#include "Internal/OptionsStorage.hpp"

#include <utility>

namespace Testament {

class SuiteOptions::Impl : public OptionsStorage<Attribute> {};

SuiteOptions::SuiteOptions() : impl(std::make_shared<Impl>()) {}
SuiteOptions::~SuiteOptions() = default;
SuiteOptions::SuiteOptions(const SuiteOptions& other) = default;
SuiteOptions& SuiteOptions::operator=(const SuiteOptions& other) = default;
SuiteOptions::SuiteOptions(SuiteOptions&& other) noexcept : impl(other.impl) {}
SuiteOptions& SuiteOptions::operator=(SuiteOptions&& other) noexcept {
    impl = other.impl;
    return *this;
}

void SuiteOptions::detach() {
    if (!impl) {
        impl = std::make_shared<Impl>();
    } else if (impl.use_count() != 1) {
        impl = std::make_shared<Impl>(*impl);
    }
}

SuiteOptions& SuiteOptions::order(int value) {
    detach();
    impl->order = value;
    return *this;
}
SuiteOptions& SuiteOptions::tag(std::string value) {
    detach();
    impl->tags.push_back(std::move(value));
    return *this;
}
SuiteOptions& SuiteOptions::attribute(std::string key, std::string value) {
    detach();
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

}
