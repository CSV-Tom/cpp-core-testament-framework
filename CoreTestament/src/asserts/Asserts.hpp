#include <stdexcept>
#include <sstream>
#include <source_location>

namespace Asserts {
    void assertTrue(bool condition, const std::string_view& message,
                    const std::source_location location = std::source_location::current()) {
        if (!condition) {
            std::ostringstream oss;
            oss << "AssertTrue failed: " << message << "\n"
                << "Location: " << location.file_name() << ":" << location.line() << " in " << location.function_name();
            throw std::logic_error(oss.str());
        }
    }

    void assertFalse(bool condition, const std::string_view& message,
                     const std::source_location location = std::source_location::current()) {
        if (condition) {
            std::ostringstream oss;
            oss << "AssertFalse failed: " << message << "\n"
                << "Location: " << location.file_name() << ":" << location.line() << " in " << location.function_name();
            throw std::logic_error(oss.str());
        }
    }

    template <typename T>
    void assertEquals(const T& expected, const T& actual, const std::string_view& message = "",
                      const std::source_location location = std::source_location::current()) {
        if (!(expected == actual)) {
            std::ostringstream oss;
            oss << "AssertEquals failed: " << message << "\n"
                << "Expected: " << expected << ", Actual: " << actual << "\n"
                << "Location: " << location.file_name() << ":" << location.line() << " in " << location.function_name();
            throw std::logic_error(oss.str());
        }
    }

    template <typename T>
    void assertNotNull(const T* ptr, const std::string_view& message = "",
                       const std::source_location location = std::source_location::current()) {
        if (ptr == nullptr) {
            std::ostringstream oss;
            oss << "AssertNotNull failed: " << message << "\n"
                << "Location: " << location.file_name() << ":" << location.line() << " in " << location.function_name();
            throw std::logic_error(oss.str());
        }
    }

    template <typename T>
    void assertInRange(const T& value, const T& min, const T& max, const std::string_view& message = "",
                       const std::source_location location = std::source_location::current()) {
        if (value < min || value > max) {
            std::ostringstream oss;
            oss << "AssertInRange failed: " << message << "\n"
                << "Value: " << value << ", Expected Range: [" << min << ", " << max << "]\n"
                << "Location: " << location.file_name() << ":" << location.line() << " in " << location.function_name();
            throw std::logic_error(oss.str());
        }
    }
}