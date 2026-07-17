#ifndef TESTFRAMEWORK_TESTAMENT_ASSERTS_HPP
#define TESTFRAMEWORK_TESTAMENT_ASSERTS_HPP

#include "Testament/AssertionFailure.hpp"

#include <concepts>
#include <ostream>
#include <sstream>
#include <source_location>
#include <string>
#include <string_view>

namespace Testament::Asserts {

namespace detail {

template <typename T>
concept Streamable = requires(std::ostream& output, const T& value) {
    output << value;
};

template <typename T>
concept EqualityComparable = requires(const T& left, const T& right) {
    { left == right } -> std::convertible_to<bool>;
};

template <typename T>
concept RangeComparable = requires(const T& value, const T& boundary) {
    { value < boundary } -> std::convertible_to<bool>;
    { boundary < value } -> std::convertible_to<bool>;
};

template <typename T>
std::string formatValue(const T& value) {
    if constexpr (Streamable<T>) {
        std::ostringstream output;
        output << value;
        return output.str();
    } else {
        return "<unformattable value>";
    }
}

[[noreturn]] void failAssertion(std::string assertion, std::string expected,
                                std::string actual, std::string_view message,
                                std::source_location location);

}

void assertTrue(bool condition, std::string_view message = {},
                std::source_location location = std::source_location::current());

void assertFalse(bool condition, std::string_view message = {},
                 std::source_location location = std::source_location::current());

template <typename T>
requires detail::EqualityComparable<T>
void assertEquals(const T& expected, const T& actual, std::string_view message = {},
                  std::source_location location = std::source_location::current()) {
    if (!(expected == actual)) {
        detail::failAssertion("assertEquals", detail::formatValue(expected),
                              detail::formatValue(actual), message, location);
    }
}

template <typename T>
void assertNotNull(const T* pointer, std::string_view message = {},
                   std::source_location location = std::source_location::current()) {
    if (!pointer) {
        detail::failAssertion("assertNotNull", "non-null", "null", message, location);
    }
}

template <typename T>
requires detail::RangeComparable<T>
void assertInRange(const T& value, const T& minimum, const T& maximum,
                   std::string_view message = {},
                   std::source_location location = std::source_location::current()) {
    if (value < minimum || maximum < value) {
        detail::failAssertion(
            "assertInRange",
            '[' + detail::formatValue(minimum) + ", " + detail::formatValue(maximum) + ']',
            detail::formatValue(value), message, location
        );
    }
}

}

#endif
