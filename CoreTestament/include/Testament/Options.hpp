#ifndef TESTAMENT_OPTIONS_HPP
#define TESTAMENT_OPTIONS_HPP

#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace Testament {

class SuiteOptions {
public:
    using Attribute = std::pair<std::string, std::string>;

    SuiteOptions();
    ~SuiteOptions();

    SuiteOptions(const SuiteOptions& other);
    SuiteOptions& operator=(const SuiteOptions& other);
    SuiteOptions(SuiteOptions&&) noexcept;
    SuiteOptions& operator=(SuiteOptions&&) noexcept;

    SuiteOptions& order(int value);
    SuiteOptions& tag(std::string value);
    SuiteOptions& attribute(std::string key, std::string value);

    [[nodiscard]] std::optional<int> order() const noexcept;
    [[nodiscard]] std::span<const std::string> tags() const noexcept;
    [[nodiscard]] std::span<const Attribute> attributes() const noexcept;
    [[nodiscard]] std::optional<std::string_view> attribute(std::string_view key) const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

class TestOptions {
public:
    using Attribute = std::pair<std::string, std::string>;

    TestOptions();
    ~TestOptions();

    TestOptions(const TestOptions& other);
    TestOptions& operator=(const TestOptions& other);
    TestOptions(TestOptions&&) noexcept;
    TestOptions& operator=(TestOptions&&) noexcept;

    TestOptions& order(int value);
    TestOptions& tag(std::string value);
    TestOptions& attribute(std::string key, std::string value);

    [[nodiscard]] std::optional<int> order() const noexcept;
    [[nodiscard]] std::span<const std::string> tags() const noexcept;
    [[nodiscard]] std::span<const Attribute> attributes() const noexcept;
    [[nodiscard]] std::optional<std::string_view> attribute(std::string_view key) const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

}

#endif
