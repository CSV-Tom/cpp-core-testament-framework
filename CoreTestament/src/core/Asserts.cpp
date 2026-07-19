#include "Testament/Asserts.hpp"

#include "Internal/AssertionCollection.hpp"

#include <exception>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace Testament::Asserts {

namespace detail {

namespace {

thread_local bool collectionActive = false;
thread_local std::vector<AssertionFailure> collectedFailures;

std::exception_ptr combinedFailure() {
    if (collectedFailures.empty()) return {};
    if (collectedFailures.size() == 1) {
        return std::make_exception_ptr(collectedFailures.front());
    }

    std::ostringstream message;
    message << collectedFailures.size() << " non-fatal assertions failed:";
    for (const auto& failure : collectedFailures) {
        message << "\n\n" << failure.what();
    }
    const auto location = collectedFailures.front().location();
    return std::make_exception_ptr(AssertionFailure{
        "expect", "all expectations pass",
        std::to_string(collectedFailures.size()) + " failures", message.str(), location
    });
}

}

void beginAssertionCollection() {
    collectedFailures.clear();
    collectionActive = true;
}

std::exception_ptr finishAssertionCollection(std::exception_ptr terminalFailure) {
    collectionActive = false;
    if (terminalFailure) {
        try {
            std::rethrow_exception(terminalFailure);
        } catch (const AssertionFailure& failure) {
            collectedFailures.push_back(failure);
        } catch (...) {
            collectedFailures.clear();
            return terminalFailure;
        }
    }

    auto failure = combinedFailure();
    collectedFailures.clear();
    return failure;
}

bool recordNonFatalFailure(const AssertionFailure& failure) {
    if (!collectionActive) return false;
    collectedFailures.push_back(failure);
    return true;
}

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
