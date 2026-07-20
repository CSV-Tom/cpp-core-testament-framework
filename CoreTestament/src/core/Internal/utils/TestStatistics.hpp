#ifndef TESTFRAMEWORK_TESTAMENT_TESTSTATISTICS_HPP
#define TESTFRAMEWORK_TESTAMENT_TESTSTATISTICS_HPP

namespace Testament {

template <typename CounterType = unsigned int>
class TestStatistics {
public:
    TestStatistics() = default;

    [[nodiscard]] CounterType skippedTests() const noexcept {
        return skippedTests_;
    }

    [[nodiscard]] CounterType failedTests() const noexcept {
        return failedTests_;
    }

    [[nodiscard]] CounterType passedTests() const noexcept {
        return successfulTests_;
    }

    [[nodiscard]] CounterType errors() const noexcept {
        return errors_;
    }

    [[nodiscard]] CounterType totalTests() const noexcept {
        return skippedTests_ + failedTests_ + successfulTests_ + errors_;
    }

    void incrementSkippedTests() noexcept {
        ++skippedTests_;
    }

    void incrementFailedTests() noexcept {
        ++failedTests_;
    }

    void incrementPassedTests() noexcept {
        ++successfulTests_;
    }

    void incrementErrors() noexcept {
        ++errors_;
    }

    void reset() noexcept {
        skippedTests_ = 0;
        failedTests_ = 0;
        successfulTests_ = 0;
        errors_ = 0;
    }

    [[nodiscard]] TestStatistics operator+(const TestStatistics& other) const noexcept {
        TestStatistics result;
        result.skippedTests_ = skippedTests_ + other.skippedTests_;
        result.failedTests_ = failedTests_ + other.failedTests_;
        result.successfulTests_ = successfulTests_ + other.successfulTests_;
        result.errors_ = errors_ + other.errors_;
        return result;
    }

    TestStatistics& operator+=(const TestStatistics& other) noexcept {
        skippedTests_ += other.skippedTests_;
        failedTests_ += other.failedTests_;
        successfulTests_ += other.successfulTests_;
        errors_ += other.errors_;
        return *this;
    }

private:
    CounterType skippedTests_{};
    CounterType failedTests_{};
    CounterType successfulTests_{};
    CounterType errors_{};
};

}

#endif
