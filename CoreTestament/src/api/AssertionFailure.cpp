#include "Testament/AssertionFailure.hpp"

#include <sstream>
#include <utility>

namespace Testament {

class AssertionFailure::Impl {
public:
    Impl(std::string assertionName, std::string expectedValue, std::string actualValue,
         std::string failureMessage, std::source_location definition)
        : assertion(std::move(assertionName)), expected(std::move(expectedValue)),
          actual(std::move(actualValue)), message(std::move(failureMessage)),
          location(definition) {
        std::ostringstream output;
        output << assertion << " failed";
        if (!message.empty()) {
            output << ": " << message;
        }
        output << "\n"
               << "  expected: " << expected << ", actual: " << actual << "\n"
               << "  at " << location.file_name() << ':' << location.line()
               << " in " << location.function_name();
        description = output.str();
    }

    std::string assertion;
    std::string expected;
    std::string actual;
    std::string message;
    std::source_location location;
    std::string description;
};

AssertionFailure::AssertionFailure(std::string assertion, std::string expected,
                                   std::string actual, std::string message,
                                   std::source_location location)
    : pImpl(std::make_shared<Impl>(std::move(assertion), std::move(expected),
                                  std::move(actual), std::move(message), location)) {}

AssertionFailure::~AssertionFailure() = default;
AssertionFailure::AssertionFailure(const AssertionFailure&) noexcept = default;
AssertionFailure& AssertionFailure::operator=(const AssertionFailure&) noexcept = default;
AssertionFailure::AssertionFailure(AssertionFailure&& other) noexcept
    : pImpl(std::move(other.pImpl)) {}
AssertionFailure& AssertionFailure::operator=(AssertionFailure&& other) noexcept {
    pImpl = std::move(other.pImpl);
    return *this;
}

const char* AssertionFailure::what() const noexcept { return pImpl->description.c_str(); }
std::string_view AssertionFailure::assertion() const noexcept { return pImpl->assertion; }
std::string_view AssertionFailure::expected() const noexcept { return pImpl->expected; }
std::string_view AssertionFailure::actual() const noexcept { return pImpl->actual; }
std::string_view AssertionFailure::message() const noexcept { return pImpl->message; }
std::source_location AssertionFailure::location() const noexcept { return pImpl->location; }

}
