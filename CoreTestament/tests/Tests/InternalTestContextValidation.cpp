#include "Testament/LifecycleSuite.hpp"

#include "core/Internal/FunctionVariant.hpp"
#include "core/Internal/InternalTest.hpp"

#include <exception>
#include <stdexcept>

int main() {
    bool testBodyEntered = false;
    Testament::InternalTest test(
        "missing suite context",
        Testament::FunctionVariant{std::move_only_function<void(Testament::LifecycleSuite&)>(
            [&testBodyEntered](Testament::LifecycleSuite&) {
                testBodyEntered = true;
            }
        )}
    );

    const auto result = test.execute();
    if (result) {
        return 1;
    }

    try {
        std::rethrow_exception(result.error());
    } catch (const std::logic_error&) {
        return !testBodyEntered && test.getStatus().isFailed() ? 0 : 1;
    } catch (...) {
        return 1;
    }
}
