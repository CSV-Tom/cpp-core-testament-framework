#ifndef TESTAMENT_ASSERTIONFAILURE_HPP
#define TESTAMENT_ASSERTIONFAILURE_HPP

#include <exception>
#include <memory>
#include <source_location>
#include <string>
#include <string_view>

namespace Testament {

class AssertionFailure final : public std::exception {
public:
    AssertionFailure(std::string assertion, std::string expected, std::string actual,
                     std::string message, std::source_location location);
    ~AssertionFailure() override;

    AssertionFailure(const AssertionFailure&) noexcept;
    AssertionFailure& operator=(const AssertionFailure&) noexcept;
    AssertionFailure(AssertionFailure&&) noexcept;
    AssertionFailure& operator=(AssertionFailure&&) noexcept;

    [[nodiscard]] const char* what() const noexcept override;
    [[nodiscard]] std::string_view assertion() const noexcept;
    [[nodiscard]] std::string_view expected() const noexcept;
    [[nodiscard]] std::string_view actual() const noexcept;
    [[nodiscard]] std::string_view message() const noexcept;
    [[nodiscard]] std::source_location location() const noexcept;

private:
    class Impl;
    std::shared_ptr<const Impl> pImpl;
};

}

#endif
