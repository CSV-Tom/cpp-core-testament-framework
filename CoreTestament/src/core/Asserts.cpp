#include "Testament/Asserts.hpp"

#include <exception>
#include <string>
#include <utility>

namespace Testament::Asserts {

namespace detail {

void failAssertion(std::string assertion, std::string expected, std::string actual,
                   std::string_view message, std::source_location location) {
    throw AssertionFailure(std::move(assertion), std::move(expected), std::move(actual),
                           std::string{message}, location);
}

void failUnexpectedException(std::string assertion, std::string expected,
                             std::exception_ptr exception, std::string_view message,
                             std::source_location location) {
    try {
        std::rethrow_exception(exception);
    } catch (const std::exception& error) {
        failAssertion(std::move(assertion), std::move(expected),
                      "exception: " + std::string{error.what()}, message, location);
    } catch (...) {
        failAssertion(std::move(assertion), std::move(expected),
                      "non-standard exception", message, location);
    }
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
