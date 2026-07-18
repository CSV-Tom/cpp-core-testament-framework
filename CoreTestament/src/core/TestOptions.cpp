#include "Testament/TestOptions.hpp"

#include "Internal/OptionsStorage.hpp"

#include <utility>

namespace Testament {

class TestOptions::Impl : public OptionsStorage<Attribute> {
public:
    bool disabled{};
    unsigned int maxAttempts{1};
};

TestOptions::TestOptions() : impl(std::make_shared<Impl>()) {}
TestOptions::~TestOptions() = default;
TestOptions::TestOptions(const TestOptions& other) = default;
TestOptions& TestOptions::operator=(const TestOptions& other) = default;
TestOptions::TestOptions(TestOptions&& other) noexcept : impl(other.impl) {}
TestOptions& TestOptions::operator=(TestOptions&& other) noexcept {
    impl = other.impl;
    return *this;
}

void TestOptions::detach() {
    if (!impl) {
        impl = std::make_shared<Impl>();
    } else if (impl.use_count() != 1) {
        impl = std::make_shared<Impl>(*impl);
    }
}

TestOptions& TestOptions::order(int value) {
    detach();
    impl->order = value;
    return *this;
}

TestOptions& TestOptions::tag(std::string value) {
    detach();
    impl->tags.push_back(std::move(value));
    return *this;
}

TestOptions& TestOptions::attribute(std::string key, std::string value) {
    detach();
    impl->setAttribute(std::move(key), std::move(value));
    return *this;
}

TestOptions& TestOptions::disabled(bool value) {
    detach();
    impl->disabled = value;
    return *this;
}

TestOptions& TestOptions::maxAttempts(unsigned int value) {
    detach();
    impl->maxAttempts = value;
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
unsigned int TestOptions::maxAttempts() const noexcept { return impl->maxAttempts; }

}
