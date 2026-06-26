#ifndef TESTFRAMEWORK_TESTAMENT_ASSERTS_HPP
#define TESTFRAMEWORK_TESTAMENT_ASSERTS_HPP

#include <string_view>
#include <stdexcept>
#include <sstream>
#include <source_location>

namespace Testament {
namespace Asserts {

inline void assertTrue(bool condition, std::string_view message,
                const std::source_location location = std::source_location::current()) {
    if (!condition) {
        std::ostringstream oss;
        oss << "assertTrue failed: " << message << "\n"
            << "  at " << location.file_name() << ":" << location.line()
            << " in " << location.function_name();
        throw std::logic_error(oss.str());
    }
}

inline void assertFalse(bool condition, std::string_view message,
                 const std::source_location location = std::source_location::current()) {
    if (condition) {
        std::ostringstream oss;
        oss << "assertFalse failed: " << message << "\n"
            << "  at " << location.file_name() << ":" << location.line()
            << " in " << location.function_name();
        throw std::logic_error(oss.str());
    }
}

template <typename T>
void assertEquals(const T& expected, const T& actual, std::string_view message = "",
                  const std::source_location location = std::source_location::current()) {
    if (!(expected == actual)) {
        std::ostringstream oss;
        oss << "assertEquals failed: " << message << "\n"
            << "  expected: " << expected << ", actual: " << actual << "\n"
            << "  at " << location.file_name() << ":" << location.line()
            << " in " << location.function_name();
        throw std::logic_error(oss.str());
    }
}

template <typename T>
void assertNotNull(const T* ptr, std::string_view message = "",
                   const std::source_location location = std::source_location::current()) {
    if (ptr == nullptr) {
        std::ostringstream oss;
        oss << "assertNotNull failed: " << message << "\n"
            << "  at " << location.file_name() << ":" << location.line()
            << " in " << location.function_name();
        throw std::logic_error(oss.str());
    }
}

template <typename T>
void assertInRange(const T& value, const T& min, const T& max, std::string_view message = "",
                   const std::source_location location = std::source_location::current()) {
    if (value < min || value > max) {
        std::ostringstream oss;
        oss << "assertInRange failed: " << message << "\n"
            << "  value: " << value << ", range: [" << min << ", " << max << "]\n"
            << "  at " << location.file_name() << ":" << location.line()
            << " in " << location.function_name();
        throw std::logic_error(oss.str());
    }
}

}
}

#endif
