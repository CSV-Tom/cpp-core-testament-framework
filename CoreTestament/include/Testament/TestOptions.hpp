#ifndef TESTAMENT_TESTOPTIONS_HPP
#define TESTAMENT_TESTOPTIONS_HPP

#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace Testament {

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
    TestOptions& disabled(bool value = true);
    TestOptions& maxAttempts(unsigned int value);

    [[nodiscard]] std::optional<int> order() const noexcept;
    [[nodiscard]] std::span<const std::string> tags() const noexcept;
    [[nodiscard]] std::span<const Attribute> attributes() const noexcept;
    [[nodiscard]] std::optional<std::string_view> attribute(std::string_view key) const noexcept;
    [[nodiscard]] bool isDisabled() const noexcept;
    [[nodiscard]] unsigned int maxAttempts() const noexcept;

private:
    class Impl;
    void detach();
    std::shared_ptr<Impl> impl;
};

}

#endif
