#ifndef TESTFRAMEWORK_TESTAMENT_TESTSTATISTICS_HPP
#define TESTFRAMEWORK_TESTAMENT_TESTSTATISTICS_HPP

namespace Testament {

template <typename CounterType = unsigned int>
class TestStatistics {
public:
    TestStatistics() = default;

    [[nodiscard]] CounterType getSkippedTests() const noexcept {
        return skippedTests;
    }

    [[nodiscard]] CounterType getFailedTests() const noexcept {
        return failedTests;
    }

    [[nodiscard]] CounterType getPassedTests() const noexcept {
        return successfulTests;
    }

    [[nodiscard]] CounterType getErrors() const noexcept {
        return errors;
    }

    [[nodiscard]] CounterType getTotalTests() const noexcept {
        return skippedTests + failedTests + successfulTests + errors;
    }

    void incrementSkippedTests() noexcept {
        ++skippedTests;
    }

    void incrementFailedTests() noexcept {
        ++failedTests;
    }

    void incrementPassedTests() noexcept {
        ++successfulTests;
    }

    void incrementErrors() noexcept {
        ++errors;
    }

    void reset() noexcept {
        skippedTests = 0;
        failedTests = 0;
        successfulTests = 0;
        errors = 0;
    }

    [[nodiscard]] TestStatistics operator+(const TestStatistics& other) const noexcept {
        TestStatistics result;
        result.skippedTests = this->skippedTests + other.skippedTests;
        result.failedTests = this->failedTests + other.failedTests;
        result.successfulTests = this->successfulTests + other.successfulTests;
        result.errors = this->errors + other.errors;
        return result;
    }

    TestStatistics& operator+=(const TestStatistics& other) noexcept {
        this->skippedTests += other.skippedTests;
        this->failedTests += other.failedTests;
        this->successfulTests += other.successfulTests;
        this->errors += other.errors;
        return *this;
    }

private:
    CounterType skippedTests{};
    CounterType failedTests{};
    CounterType successfulTests{};
    CounterType errors{};
};

}

#endif
