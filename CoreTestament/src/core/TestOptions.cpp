#include "Testament/TestOptions.hpp"

#include "Internal/OptionsStorage.hpp"

#include <utility>

namespace Testament {

class TestOptions::Impl : public detail::OptionsStorage<Attribute> {
public:
    bool disabled{};
    unsigned int maxAttempts{1};
};

TestOptions::TestOptions() : impl(std::make_shared<Impl>()) {}
TestOptions::~TestOptions() = default;
TestOptions::TestOptions(const TestOptions& other) = default;
TestOptions& TestOptions::operator=(const TestOptions& other) = default;
TestOptions::TestOptions(TestOptions&& other) noexcept = default;
TestOptions& TestOptions::operator=(TestOptions&& other) noexcept = default;

TestOptions& TestOptions::order(int value) {
    detail::writeOptions(impl).setOrder(value);
    return *this;
}

TestOptions& TestOptions::tag(std::string value) {
    detail::writeOptions(impl).addTag(std::move(value));
    return *this;
}

TestOptions& TestOptions::attribute(std::string key, std::string value) {
    detail::writeOptions(impl).setAttribute(std::move(key), std::move(value));
    return *this;
}

TestOptions& TestOptions::disabled(bool value) {
    detail::writeOptions(impl).disabled = value;
    return *this;
}

TestOptions& TestOptions::maxAttempts(unsigned int value) {
    detail::writeOptions(impl).maxAttempts = value;
    return *this;
}

TestOptions& TestOptions::execution(Execution value) {
    detail::writeOptions(impl).setExecution(value);
    return *this;
}

std::optional<int> TestOptions::order() const noexcept {
    return detail::readOptions(impl).order();
}
std::span<const std::string> TestOptions::tags() const noexcept {
    return detail::readOptions(impl).tags();
}
std::span<const TestOptions::Attribute> TestOptions::attributes() const noexcept {
    return detail::readOptions(impl).attributes();
}
std::optional<std::string_view> TestOptions::attribute(std::string_view key) const noexcept {
    return detail::readOptions(impl).findAttribute(key);
}
bool TestOptions::isDisabled() const noexcept { return detail::readOptions(impl).disabled; }
unsigned int TestOptions::maxAttempts() const noexcept {
    return detail::readOptions(impl).maxAttempts;
}
Execution TestOptions::execution() const noexcept {
    return detail::readOptions(impl).execution();
}

}
