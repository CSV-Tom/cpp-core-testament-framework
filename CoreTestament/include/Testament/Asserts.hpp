#ifndef TESTFRAMEWORK_TESTAMENT_ASSERTS_HPP
#define TESTFRAMEWORK_TESTAMENT_ASSERTS_HPP

#include "Testament/AssertionFailure.hpp"

#include <cmath>
#include <concepts>
#include <exception>
#include <functional>
#include <ostream>
#include <sstream>
#include <source_location>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

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

[[noreturn]] void failUnexpectedException(std::string assertion, std::string expected,
                                          std::exception_ptr exception,
                                          std::string_view message,
                                          std::source_location location);

}

void assertTrue(bool condition, std::string_view message = {},
                std::source_location location = std::source_location::current());

void assertFalse(bool condition, std::string_view message = {},
                 std::source_location location = std::source_location::current());

template <typename T>
requires detail::EqualityComparable<T>
void assertNotEquals(const T& unexpected, const T& actual, std::string_view message = {},
                     std::source_location location = std::source_location::current()) {
    if (unexpected == actual) {
        detail::failAssertion("assertNotEquals", "not " + detail::formatValue(unexpected),
                              detail::formatValue(actual), message, location);
    }
}

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
void assertNull(const T* pointer, std::string_view message = {},
                std::source_location location = std::source_location::current()) {
    if (pointer) {
        detail::failAssertion("assertNull", "null", "non-null", message, location);
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

template <typename T>
requires detail::RangeComparable<T>
void assertLessThan(const T& value, const T& upperBound, std::string_view message = {},
                    std::source_location location = std::source_location::current()) {
    if (!(value < upperBound)) {
        detail::failAssertion("assertLessThan", "< " + detail::formatValue(upperBound),
                              detail::formatValue(value), message, location);
    }
}

template <typename T>
requires detail::RangeComparable<T>
void assertLessThanOrEqual(const T& value, const T& upperBound,
                           std::string_view message = {},
                           std::source_location location = std::source_location::current()) {
    if (upperBound < value) {
        detail::failAssertion("assertLessThanOrEqual", "<= " + detail::formatValue(upperBound),
                              detail::formatValue(value), message, location);
    }
}

template <typename T>
requires detail::RangeComparable<T>
void assertGreaterThan(const T& value, const T& lowerBound, std::string_view message = {},
                       std::source_location location = std::source_location::current()) {
    if (!(lowerBound < value)) {
        detail::failAssertion("assertGreaterThan", "> " + detail::formatValue(lowerBound),
                              detail::formatValue(value), message, location);
    }
}

template <typename T>
requires detail::RangeComparable<T>
void assertGreaterThanOrEqual(const T& value, const T& lowerBound,
                              std::string_view message = {},
                              std::source_location location = std::source_location::current()) {
    if (value < lowerBound) {
        detail::failAssertion("assertGreaterThanOrEqual", ">= " + detail::formatValue(lowerBound),
                              detail::formatValue(value), message, location);
    }
}

template <std::floating_point T>
void assertNear(T expected, T actual, T tolerance, std::string_view message = {},
                std::source_location location = std::source_location::current()) {
    const bool validTolerance = tolerance >= T{};
    const bool withinTolerance = expected == actual
        || (validTolerance && std::abs(expected - actual) <= tolerance);
    if (!validTolerance || !withinTolerance) {
        detail::failAssertion(
            "assertNear",
            detail::formatValue(expected) + " +/- " + detail::formatValue(tolerance),
            detail::formatValue(actual), message, location
        );
    }
}

template <typename ExpectedException, typename Callable>
requires std::invocable<Callable>
void assertThrows(Callable&& callable, std::string_view message = {},
                  std::source_location location = std::source_location::current()) {
    using Exception = std::remove_cvref_t<ExpectedException>;
    try {
        std::invoke(std::forward<Callable>(callable));
    } catch (const Exception&) {
        return;
    } catch (...) {
        detail::failUnexpectedException(
            "assertThrows", "exception matching requested type", std::current_exception(),
            message, location
        );
    }
    detail::failAssertion("assertThrows", "exception matching requested type",
                          "no exception", message, location);
}

template <typename Callable>
requires std::invocable<Callable>
void assertDoesNotThrow(Callable&& callable, std::string_view message = {},
                        std::source_location location = std::source_location::current()) {
    try {
        std::invoke(std::forward<Callable>(callable));
    } catch (...) {
        detail::failUnexpectedException(
            "assertDoesNotThrow", "no exception", std::current_exception(), message, location
        );
    }
}

}

#endif
