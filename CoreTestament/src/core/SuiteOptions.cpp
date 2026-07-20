#include "Testament/SuiteOptions.hpp"
#include "Internal/OptionsStorage.hpp"

#include <utility>

namespace Testament {

class SuiteOptions::Impl : public detail::OptionsStorage<Attribute> {};

SuiteOptions::SuiteOptions() : pImpl(std::make_shared<Impl>()) {}
SuiteOptions::~SuiteOptions() = default;
SuiteOptions::SuiteOptions(const SuiteOptions& other) = default;
SuiteOptions& SuiteOptions::operator=(const SuiteOptions& other) = default;
SuiteOptions::SuiteOptions(SuiteOptions&& other) noexcept = default;
SuiteOptions& SuiteOptions::operator=(SuiteOptions&& other) noexcept = default;

SuiteOptions& SuiteOptions::order(int value) {
    detail::writeOptions(pImpl).setOrder(value);
    return *this;
}
SuiteOptions& SuiteOptions::tag(std::string value) {
    detail::writeOptions(pImpl).addTag(std::move(value));
    return *this;
}
SuiteOptions& SuiteOptions::attribute(std::string key, std::string value) {
    detail::writeOptions(pImpl).setAttribute(std::move(key), std::move(value));
    return *this;
}
SuiteOptions& SuiteOptions::execution(Execution value) {
    detail::writeOptions(pImpl).setExecution(value);
    return *this;
}
std::optional<int> SuiteOptions::order() const noexcept {
    return detail::readOptions(pImpl).order();
}
std::span<const std::string> SuiteOptions::tags() const noexcept {
    return detail::readOptions(pImpl).tags();
}
std::span<const SuiteOptions::Attribute> SuiteOptions::attributes() const noexcept {
    return detail::readOptions(pImpl).attributes();
}
std::optional<std::string_view> SuiteOptions::attribute(std::string_view key) const noexcept {
    return detail::readOptions(pImpl).findAttribute(key);
}
Execution SuiteOptions::execution() const noexcept {
    return detail::readOptions(pImpl).execution();
}

}
