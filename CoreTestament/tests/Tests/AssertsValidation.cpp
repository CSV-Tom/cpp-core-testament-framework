#include "Testament/Asserts.hpp"

#include <initializer_list>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

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
    Testament::Asserts::assertNotEquals(3, value, "not-equals succeeds");
    Testament::Asserts::assertNotNull(&value, "not-null succeeds");
    Testament::Asserts::assertNull<int>(nullptr, "null succeeds");
    Testament::Asserts::assertInRange(value, 1, 3, "range succeeds");
    Testament::Asserts::assertLessThan(value, 3, "less-than succeeds");
    Testament::Asserts::assertLessThanOrEqual(value, 2, "less-than-or-equal succeeds");
    Testament::Asserts::assertGreaterThan(value, 1, "greater-than succeeds");
    Testament::Asserts::assertGreaterThanOrEqual(value, 2, "greater-than-or-equal succeeds");
    Testament::Asserts::assertNear(1.0, 1.01, 0.02, "near succeeds");
    Testament::Asserts::assertNear(
        std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::infinity(), 0.0, "infinity succeeds"
    );
    Testament::Asserts::assertThrows<std::runtime_error>([] {
        throw std::runtime_error("expected");
    }, "throws succeeds");
    Testament::Asserts::assertDoesNotThrow([] {}, "does-not-throw succeeds");

    const bool trueFailure = failsWith([] {
        Testament::Asserts::assertTrue(false, "true failure");
    }, {"assertTrue failed", "expected: true, actual: false", "true failure"});
    const bool falseFailure = failsWith([] {
        Testament::Asserts::assertFalse(true, "false failure");
    }, {"assertFalse failed", "expected: false, actual: true", "false failure"});
    const bool equalsFailure = failsWith([] {
        Testament::Asserts::assertEquals(2, 3, "equals failure");
    }, {"assertEquals failed", "expected: 2, actual: 3", "equals failure"});
    const bool notEqualsFailure = failsWith([] {
        Testament::Asserts::assertNotEquals(2, 2, "not-equals failure");
    }, {"assertNotEquals failed", "expected: not 2, actual: 2", "not-equals failure"});
    const bool nullFailure = failsWith([] {
        Testament::Asserts::assertNotNull<int>(nullptr, "null failure");
    }, {"assertNotNull failed", "expected: non-null, actual: null", "null failure"});
    const bool expectedNullFailure = failsWith([] {
        const int nonNull = 1;
        Testament::Asserts::assertNull(&nonNull, "expected-null failure");
    }, {"assertNull failed", "expected: null, actual: non-null", "expected-null failure"});
    const bool rangeFailure = failsWith([] {
        Testament::Asserts::assertInRange(4, 1, 3, "range failure");
    }, {"assertInRange failed", "expected: [1, 3], actual: 4", "range failure"});
    const bool lessThanFailure = failsWith([] {
        Testament::Asserts::assertLessThan(3, 3, "less-than failure");
    }, {"assertLessThan failed", "expected: < 3, actual: 3", "less-than failure"});
    const bool lessThanOrEqualFailure = failsWith([] {
        Testament::Asserts::assertLessThanOrEqual(4, 3, "less-than-or-equal failure");
    }, {"assertLessThanOrEqual failed", "expected: <= 3, actual: 4",
        "less-than-or-equal failure"});
    const bool greaterThanFailure = failsWith([] {
        Testament::Asserts::assertGreaterThan(3, 3, "greater-than failure");
    }, {"assertGreaterThan failed", "expected: > 3, actual: 3", "greater-than failure"});
    const bool greaterThanOrEqualFailure = failsWith([] {
        Testament::Asserts::assertGreaterThanOrEqual(2, 3, "greater-than-or-equal failure");
    }, {"assertGreaterThanOrEqual failed", "expected: >= 3, actual: 2",
        "greater-than-or-equal failure"});
    const bool nearFailure = failsWith([] {
        Testament::Asserts::assertNear(1.0, 1.2, 0.1, "near failure");
    }, {"assertNear failed", "expected: 1 +/- 0.1, actual: 1.2", "near failure"});
    const bool invalidToleranceFailure = failsWith([] {
        Testament::Asserts::assertNear(1.0, 1.0, -0.1, "invalid tolerance");
    }, {"assertNear failed", "invalid tolerance"});
    const bool nanFailure = failsWith([] {
        const auto nan = std::numeric_limits<double>::quiet_NaN();
        Testament::Asserts::assertNear(nan, nan, 0.1, "NaN failure");
    }, {"assertNear failed", "NaN failure"});
    const bool noExceptionFailure = failsWith([] {
        Testament::Asserts::assertThrows<std::runtime_error>([] {}, "missing exception");
    }, {"assertThrows failed", "expected: exception matching requested type, actual: no exception",
        "missing exception"});
    const bool wrongExceptionFailure = failsWith([] {
        Testament::Asserts::assertThrows<std::runtime_error>([] {
            throw std::logic_error("wrong type");
        }, "wrong exception");
    }, {"assertThrows failed", "actual: exception: wrong type", "wrong exception"});
    const bool nonStandardExceptionFailure = failsWith([] {
        Testament::Asserts::assertThrows<std::runtime_error>([] { throw 42; });
    }, {"assertThrows failed", "actual: non-standard exception"});
    const bool unexpectedExceptionFailure = failsWith([] {
        Testament::Asserts::assertDoesNotThrow([] {
            throw std::runtime_error("unexpected");
        }, "unexpected exception");
    }, {"assertDoesNotThrow failed", "expected: no exception",
        "actual: exception: unexpected", "unexpected exception"});

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
        auto moveSource = failure;
        const auto movedFailure = std::move(moveSource);
        structuredFailure = failure.assertion() == "assertEquals"
            && failure.expected() == "2"
            && failure.actual() == "3"
            && failure.message() == "structured failure"
            && failure.location().line() != 0
            && std::string_view{copiedFailure.what()} == failure.what()
            && std::string_view{moveSource.what()} == movedFailure.what();
    }

    return trueFailure && falseFailure && equalsFailure && notEqualsFailure && nullFailure
        && expectedNullFailure && rangeFailure && lessThanFailure && lessThanOrEqualFailure
        && greaterThanFailure && greaterThanOrEqualFailure && nearFailure
        && invalidToleranceFailure && nanFailure && noExceptionFailure
        && wrongExceptionFailure && nonStandardExceptionFailure && unexpectedExceptionFailure
        && optionalMessageFailure && unformattableEqualsFailure && unformattableRangeFailure
        && locationFailure && structuredFailure
        ? 0
        : 1;
}
