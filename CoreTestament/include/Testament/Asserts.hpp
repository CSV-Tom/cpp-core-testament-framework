#ifndef TESTFRAMEWORK_TESTAMENT_ASSERTS_HPP
#define TESTFRAMEWORK_TESTAMENT_ASSERTS_HPP

#include "Testament/AssertionFailure.hpp"
#include "Testament/Export.hpp"

#include <cmath>
#include <algorithm>
#include <concepts>
#include <exception>
#include <functional>
#include <ostream>
#include <ranges>
#include <sstream>
#include <source_location>
#include <string>
#include <string_view>
#include <type_traits>
#include <tuple>
#include <utility>

namespace Testament::Asserts {

namespace detail {

template <typename T>
concept Streamable = requires(std::ostream& output, const T& value) {
    output << value;
};

template <typename Left, typename Right>
concept EqualityComparable = requires(const Left& left, const Right& right) {
    { left == right } -> std::convertible_to<bool>;
};

template <typename T>
concept RangeComparable = requires(const T& value, const T& boundary) {
    { value < boundary } -> std::convertible_to<bool>;
    { boundary < value } -> std::convertible_to<bool>;
};

template <typename T>
std::string formatValue(const T& value);

template <typename Range>
requires std::ranges::input_range<const Range>
std::string formatRange(const Range& range) {
    std::string formatted{"["};
    bool first = true;
    for (const auto& value : range) {
        if (!first) formatted += ", ";
        first = false;
        formatted += formatValue(value);
    }
    return formatted + ']';
}

template <typename T>
struct IsPair : std::false_type {};

template <typename First, typename Second>
struct IsPair<std::pair<First, Second>> : std::true_type {};

template <typename T>
struct IsTuple : std::false_type {};

template <typename... Values>
struct IsTuple<std::tuple<Values...>> : std::true_type {};

template <typename T>
concept PairOrTuple = IsPair<std::remove_cvref_t<T>>::value
    || IsTuple<std::remove_cvref_t<T>>::value;

template <PairOrTuple T>
std::string formatTuple(const T& value) {
    std::string formatted{"("};
    bool first = true;
    std::apply([&](const auto&... elements) {
        ((formatted += std::exchange(first, false) ? "" : ", ",
          formatted += formatValue(elements)), ...);
    }, value);
    return formatted + ')';
}

template <typename T>
std::string formatValue(const T& value) {
    if constexpr (Streamable<T>) {
        std::ostringstream output;
        output << value;
        return output.str();
    } else if constexpr (PairOrTuple<T>) {
        return formatTuple(value);
    } else if constexpr (std::ranges::input_range<const T>) {
        return formatRange(value);
    } else {
        return "<unformattable value>";
    }
}

[[nodiscard]] TESTAMENT_EXPORT bool recordNonFatalFailure(const AssertionFailure& failure);

[[noreturn]] TESTAMENT_EXPORT void failAssertion(std::string assertion, std::string expected,
                                std::string actual, std::string_view message,
                                std::source_location location);

[[noreturn]] TESTAMENT_EXPORT void failUnexpectedException(std::string assertion, std::string expected,
                                          std::exception_ptr exception,
                                          std::string_view message,
                                          std::source_location location);

}

TESTAMENT_EXPORT void assertTrue(bool condition, std::string_view message = {},
                std::source_location location = std::source_location::current());

TESTAMENT_EXPORT void assertFalse(bool condition, std::string_view message = {},
                 std::source_location location = std::source_location::current());

template <typename Unexpected, typename Actual>
requires detail::EqualityComparable<Unexpected, Actual>
void assertNotEquals(const Unexpected& unexpected, const Actual& actual,
                     std::string_view message = {},
                     std::source_location location = std::source_location::current()) {
    if (unexpected == actual) {
        detail::failAssertion("assertNotEquals", "not " + detail::formatValue(unexpected),
                              detail::formatValue(actual), message, location);
    }
}

template <typename Expected, typename Actual>
requires detail::EqualityComparable<Expected, Actual>
void assertEquals(const Expected& expected, const Actual& actual, std::string_view message = {},
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

template <typename Pointer>
requires (!std::is_pointer_v<std::remove_cvref_t<Pointer>>)
    && requires(const Pointer& pointer) {
        { static_cast<bool>(pointer) } -> std::same_as<bool>;
    }
void assertNotNull(const Pointer& pointer, std::string_view message = {},
                   std::source_location location = std::source_location::current()) {
    if (!pointer) {
        detail::failAssertion("assertNotNull", "non-null", "null", message, location);
    }
}

template <typename Pointer>
requires (!std::is_pointer_v<std::remove_cvref_t<Pointer>>)
    && requires(const Pointer& pointer) {
        { static_cast<bool>(pointer) } -> std::same_as<bool>;
    }
void assertNull(const Pointer& pointer, std::string_view message = {},
                std::source_location location = std::source_location::current()) {
    if (pointer) {
        detail::failAssertion("assertNull", "null", "non-null", message, location);
    }
}

template <typename Expected, typename Actual>
requires std::ranges::input_range<const Expected>
    && std::ranges::input_range<const Actual>
    && std::indirect_binary_predicate<
    std::ranges::equal_to, std::ranges::iterator_t<const Expected>,
    std::ranges::iterator_t<const Actual>
>
void assertRangeEquals(const Expected& expected, const Actual& actual,
                       std::string_view message = {},
                       std::source_location location = std::source_location::current()) {
    if (!std::ranges::equal(expected, actual)) {
        detail::failAssertion("assertRangeEquals", detail::formatRange(expected),
                              detail::formatRange(actual), message, location);
    }
}

template <typename Range, typename Value>
requires std::ranges::input_range<const Range>
    && detail::EqualityComparable<std::ranges::range_reference_t<const Range>, Value>
void assertContains(const Range& range, const Value& value, std::string_view message = {},
                    std::source_location location = std::source_location::current()) {
    if (std::ranges::find(range, value) == std::ranges::end(range)) {
        detail::failAssertion("assertContains", "range containing " + detail::formatValue(value),
                              detail::formatRange(range), message, location);
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

template <typename Callable>
requires std::invocable<Callable>
void expect(Callable&& assertion) {
    try {
        std::invoke(std::forward<Callable>(assertion));
    } catch (const AssertionFailure& failure) {
        if (!detail::recordNonFatalFailure(failure)) throw;
    }
}

}

#endif
