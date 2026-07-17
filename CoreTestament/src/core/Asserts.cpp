#include "Testament/Asserts.hpp"

#include <utility>

namespace Testament::Asserts {

namespace detail {

void failAssertion(std::string assertion, std::string expected, std::string actual,
                   std::string_view message, std::source_location location) {
    throw AssertionFailure(std::move(assertion), std::move(expected), std::move(actual),
                           std::string{message}, location);
}

}

void assertTrue(bool condition, std::string_view message, std::source_location location) {
    if (!condition) {
        detail::failAssertion("assertTrue", "true", "false", message, location);
    }
}

void assertFalse(bool condition, std::string_view message, std::source_location location) {
    if (condition) {
        detail::failAssertion("assertFalse", "false", "true", message, location);
    }
}

}
