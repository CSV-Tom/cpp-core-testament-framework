#include "Testament/Asserts.hpp"

#include <initializer_list>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

template <typename Callable>
bool failsWith(Callable&& callable, std::initializer_list<std::string_view> fragments) {
    try {
        callable();
    } catch (const std::logic_error& error) {
        const std::string_view message{error.what()};
        for (const auto fragment : fragments) {
            if (!message.contains(fragment)) {
                return false;
            }
        }
        return true;
    } catch (...) {
        return false;
    }
    return false;
}

}

int main() {
    const int value = 2;
    Testament::Asserts::assertTrue(true, "true succeeds");
    Testament::Asserts::assertFalse(false, "false succeeds");
    Testament::Asserts::assertEquals(2, value, "equals succeeds");
    Testament::Asserts::assertNotNull(&value, "not-null succeeds");
    Testament::Asserts::assertInRange(value, 1, 3, "range succeeds");

    const bool trueFailure = failsWith([] {
        Testament::Asserts::assertTrue(false, "true failure");
    }, {"assertTrue failed", "true failure"});
    const bool falseFailure = failsWith([] {
        Testament::Asserts::assertFalse(true, "false failure");
    }, {"assertFalse failed", "false failure"});
    const bool equalsFailure = failsWith([] {
        Testament::Asserts::assertEquals(2, 3, "equals failure");
    }, {"assertEquals failed", "expected: 2, actual: 3", "equals failure"});
    const bool nullFailure = failsWith([] {
        Testament::Asserts::assertNotNull<int>(nullptr, "null failure");
    }, {"assertNotNull failed", "null failure"});
    const bool rangeFailure = failsWith([] {
        Testament::Asserts::assertInRange(4, 1, 3, "range failure");
    }, {"assertInRange failed", "value: 4, range: [1, 3]", "range failure"});

    const auto locationLine = __LINE__ + 1;
    const bool locationFailure = failsWith([] { Testament::Asserts::assertTrue(false, "location failure"); },
                                           {__FILE__, std::to_string(locationLine), "location failure"});

    return trueFailure && falseFailure && equalsFailure && nullFailure
        && rangeFailure && locationFailure
        ? 0
        : 1;
}
