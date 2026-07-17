#include "Testament/Suite.hpp"

#include "core/Internal/FunctionVariant.hpp"
#include "core/Internal/InternalTest.hpp"

#include <exception>
#include <stdexcept>
#include <variant>

int main() {
    bool testBodyEntered = false;
    Testament::InternalTest test(
        "missing suite context",
        Testament::FunctionVariant{std::function<void(Testament::Suite&)>(
            [&testBodyEntered](Testament::Suite&) {
                testBodyEntered = true;
            }
        )}
    );

    const auto result = test.execute();
    if (!std::holds_alternative<std::exception_ptr>(result)) {
        return 1;
    }

    try {
        std::rethrow_exception(std::get<std::exception_ptr>(result));
    } catch (const std::logic_error&) {
        return !testBodyEntered && test.getStatus().isFailed() ? 0 : 1;
    } catch (...) {
        return 1;
    }
}
