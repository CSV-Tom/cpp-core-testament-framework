#include "Testament/SuiteOptions.hpp"
#include "Internal/OptionsStorage.hpp"

#include <utility>

namespace Testament {

class SuiteOptions::Impl : public OptionsStorage<Attribute> {};

SuiteOptions::SuiteOptions() : impl(std::make_shared<Impl>()) {}
SuiteOptions::~SuiteOptions() = default;
SuiteOptions::SuiteOptions(const SuiteOptions& other) = default;
SuiteOptions& SuiteOptions::operator=(const SuiteOptions& other) = default;
SuiteOptions::SuiteOptions(SuiteOptions&& other) noexcept = default;
SuiteOptions& SuiteOptions::operator=(SuiteOptions&& other) noexcept = default;

void SuiteOptions::detach() {
    if (!impl) {
        impl = std::make_shared<Impl>();
    } else if (impl.use_count() != 1) {
        impl = std::make_shared<Impl>(*impl);
    }
}

const SuiteOptions::Impl& SuiteOptions::read() const noexcept {
    static const Impl empty;
    return impl ? *impl : empty;
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
SuiteOptions& SuiteOptions::execution(Execution value) {
    detach();
    impl->execution = value;
    return *this;
}
std::optional<int> SuiteOptions::order() const noexcept { return read().order; }
std::span<const std::string> SuiteOptions::tags() const noexcept { return read().tags; }
std::span<const SuiteOptions::Attribute> SuiteOptions::attributes() const noexcept {
    return read().attributes;
}
std::optional<std::string_view> SuiteOptions::attribute(std::string_view key) const noexcept {
    return read().findAttribute(key);
}
Execution SuiteOptions::execution() const noexcept { return read().execution; }

}
