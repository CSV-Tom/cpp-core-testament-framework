#include "Testament/Asserts.hpp"

#include <initializer_list>
#include <string>
#include <string_view>

namespace {

template <typename Callable>
bool failsWith(Callable&& callable, std::initializer_list<std::string_view> fragments) {
    try {
        callable();
    } catch (const Testament::AssertionFailure& error) {
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

struct ComparableOnly {
    int value;

    friend bool operator==(const ComparableOnly&, const ComparableOnly&) = default;
    friend bool operator<(const ComparableOnly& left, const ComparableOnly& right) {
        return left.value < right.value;
    }
};

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
    }, {"assertTrue failed", "expected: true, actual: false", "true failure"});
    const bool falseFailure = failsWith([] {
        Testament::Asserts::assertFalse(true, "false failure");
    }, {"assertFalse failed", "expected: false, actual: true", "false failure"});
    const bool equalsFailure = failsWith([] {
        Testament::Asserts::assertEquals(2, 3, "equals failure");
    }, {"assertEquals failed", "expected: 2, actual: 3", "equals failure"});
    const bool nullFailure = failsWith([] {
        Testament::Asserts::assertNotNull<int>(nullptr, "null failure");
    }, {"assertNotNull failed", "expected: non-null, actual: null", "null failure"});
    const bool rangeFailure = failsWith([] {
        Testament::Asserts::assertInRange(4, 1, 3, "range failure");
    }, {"assertInRange failed", "expected: [1, 3], actual: 4", "range failure"});

    const bool optionalMessageFailure = failsWith([] {
        Testament::Asserts::assertTrue(false);
    }, {"assertTrue failed", "expected: true, actual: false"});

    const bool unformattableEqualsFailure = failsWith([] {
        Testament::Asserts::assertEquals(ComparableOnly{1}, ComparableOnly{2});
    }, {"assertEquals failed", "expected: <unformattable value>",
        "actual: <unformattable value>"});

    const bool unformattableRangeFailure = failsWith([] {
        Testament::Asserts::assertInRange(
            ComparableOnly{4}, ComparableOnly{1}, ComparableOnly{3}
        );
    }, {"assertInRange failed", "<unformattable value>"});

    const auto locationLine = __LINE__ + 1;
    const bool locationFailure = failsWith([] { Testament::Asserts::assertTrue(false, "location failure"); },
                                           {__FILE__, std::to_string(locationLine), "location failure"});

    bool structuredFailure = false;
    try {
        Testament::Asserts::assertEquals(2, 3, "structured failure");
    } catch (const Testament::AssertionFailure& failure) {
        const auto copiedFailure = failure;
        structuredFailure = failure.assertion() == "assertEquals"
            && failure.expected() == "2"
            && failure.actual() == "3"
            && failure.message() == "structured failure"
            && failure.location().line() != 0
            && std::string_view{copiedFailure.what()} == failure.what();
    }

    return trueFailure && falseFailure && equalsFailure && nullFailure
        && rangeFailure && optionalMessageFailure && unformattableEqualsFailure
        && unformattableRangeFailure && locationFailure && structuredFailure
        ? 0
        : 1;
}
