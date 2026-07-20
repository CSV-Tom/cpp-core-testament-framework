#include "Testament/LifecycleSuite.hpp"

#include "runtime/TestFunction.hpp"
#include "runtime/TestInstance.hpp"

#include <exception>
#include <stdexcept>

int main() {
    bool testBodyEntered = false;
    Testament::detail::TestInstance test(
        "missing suite context",
        Testament::detail::TestFunction{std::move_only_function<void(Testament::LifecycleSuite&)>(
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
        return !testBodyEntered && test.status().isFailed() ? 0 : 1;
    } catch (...) {
        return 1;
    }
}
