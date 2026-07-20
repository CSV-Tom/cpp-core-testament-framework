#include "Testament/SuiteOptions.hpp"
#include "Internal/OptionsStorage.hpp"

#include <utility>

namespace Testament {

class SuiteOptions::Impl : public detail::OptionsStorage<Attribute> {};

SuiteOptions::SuiteOptions() : impl(std::make_shared<Impl>()) {}
SuiteOptions::~SuiteOptions() = default;
SuiteOptions::SuiteOptions(const SuiteOptions& other) = default;
SuiteOptions& SuiteOptions::operator=(const SuiteOptions& other) = default;
SuiteOptions::SuiteOptions(SuiteOptions&& other) noexcept = default;
SuiteOptions& SuiteOptions::operator=(SuiteOptions&& other) noexcept = default;

SuiteOptions& SuiteOptions::order(int value) {
    detail::writeOptions(impl).setOrder(value);
    return *this;
}
SuiteOptions& SuiteOptions::tag(std::string value) {
    detail::writeOptions(impl).addTag(std::move(value));
    return *this;
}
SuiteOptions& SuiteOptions::attribute(std::string key, std::string value) {
    detail::writeOptions(impl).setAttribute(std::move(key), std::move(value));
    return *this;
}
SuiteOptions& SuiteOptions::execution(Execution value) {
    detail::writeOptions(impl).setExecution(value);
    return *this;
}
std::optional<int> SuiteOptions::order() const noexcept {
    return detail::readOptions(impl).order();
}
std::span<const std::string> SuiteOptions::tags() const noexcept {
    return detail::readOptions(impl).tags();
}
std::span<const SuiteOptions::Attribute> SuiteOptions::attributes() const noexcept {
    return detail::readOptions(impl).attributes();
}
std::optional<std::string_view> SuiteOptions::attribute(std::string_view key) const noexcept {
    return detail::readOptions(impl).findAttribute(key);
}
Execution SuiteOptions::execution() const noexcept {
    return detail::readOptions(impl).execution();
}

}
