#include "Testament/AssertionFailure.hpp"

#include <sstream>
#include <utility>

namespace Testament {

class AssertionFailure::Impl {
public:
    Impl(std::string assertion_, std::string expected_, std::string actual_,
         std::string message_, std::source_location location_)
        : assertion(std::move(assertion_)), expected(std::move(expected_)),
          actual(std::move(actual_)), message(std::move(message_)), location(location_) {
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
    : impl(std::make_shared<Impl>(std::move(assertion), std::move(expected),
                                  std::move(actual), std::move(message), location)) {}

AssertionFailure::~AssertionFailure() = default;
AssertionFailure::AssertionFailure(const AssertionFailure&) noexcept = default;
AssertionFailure& AssertionFailure::operator=(const AssertionFailure&) noexcept = default;
AssertionFailure::AssertionFailure(AssertionFailure&& other) noexcept : impl(other.impl) {}
AssertionFailure& AssertionFailure::operator=(AssertionFailure&& other) noexcept {
    impl = other.impl;
    return *this;
}

const char* AssertionFailure::what() const noexcept { return impl->description.c_str(); }
std::string_view AssertionFailure::assertion() const noexcept { return impl->assertion; }
std::string_view AssertionFailure::expected() const noexcept { return impl->expected; }
std::string_view AssertionFailure::actual() const noexcept { return impl->actual; }
std::string_view AssertionFailure::message() const noexcept { return impl->message; }
std::source_location AssertionFailure::location() const noexcept { return impl->location; }

}
