#pragma once

namespace Testament::detail {

template <typename CounterType = unsigned int>
class TestCounts {
public:
    TestCounts() = default;

    [[nodiscard]] CounterType skippedTests() const noexcept {
        return mSkippedTests;
    }

    [[nodiscard]] CounterType failedTests() const noexcept {
        return mFailedTests;
    }

    [[nodiscard]] CounterType passedTests() const noexcept {
        return mSuccessfulTests;
    }

    [[nodiscard]] CounterType errors() const noexcept {
        return mErrors;
    }

    [[nodiscard]] CounterType totalTests() const noexcept {
        return mSkippedTests + mFailedTests + mSuccessfulTests + mErrors;
    }

    void incrementSkippedTests() noexcept {
        ++mSkippedTests;
    }

    void incrementFailedTests() noexcept {
        ++mFailedTests;
    }

    void incrementPassedTests() noexcept {
        ++mSuccessfulTests;
    }

    void incrementErrors() noexcept {
        ++mErrors;
    }

    void reset() noexcept {
        mSkippedTests = 0;
        mFailedTests = 0;
        mSuccessfulTests = 0;
        mErrors = 0;
    }

    [[nodiscard]] TestCounts operator+(const TestCounts& other) const noexcept {
        TestCounts result;
        result.mSkippedTests = mSkippedTests + other.mSkippedTests;
        result.mFailedTests = mFailedTests + other.mFailedTests;
        result.mSuccessfulTests = mSuccessfulTests + other.mSuccessfulTests;
        result.mErrors = mErrors + other.mErrors;
        return result;
    }

    TestCounts& operator+=(const TestCounts& other) noexcept {
        mSkippedTests += other.mSkippedTests;
        mFailedTests += other.mFailedTests;
        mSuccessfulTests += other.mSuccessfulTests;
        mErrors += other.mErrors;
        return *this;
    }

private:
    CounterType mSkippedTests{};
    CounterType mFailedTests{};
    CounterType mSuccessfulTests{};
    CounterType mErrors{};
};

}
