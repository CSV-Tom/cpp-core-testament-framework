template <typename CounterType = unsigned int>
class TestStatistics {
public:
    // Konstruktor
    TestStatistics() : skippedTests(0), failedTests(0), successfulTests(0) {}

    // Getter für die Testanzahlen
    CounterType getSkippedTests() const {
        return skippedTests;
    }

    CounterType getFailedTests() const {
        return failedTests;
    }

    CounterType getPassedTests() const {
        return successfulTests;
    }

    CounterType getTotalTests() const {
        return skippedTests + failedTests + successfulTests;
    }

    // Setter zum Ändern der Werte
    void incrementSkippedTests() {
        ++skippedTests;
    }

    void incrementFailedTests() {
        ++failedTests;
    }

    void incrementPassedTests() {
        ++successfulTests;
    }

    void reset() {
        skippedTests = 0;
        failedTests = 0;
        successfulTests = 0;
    }

    // Überladen des + Operators
    TestStatistics operator+(const TestStatistics& other) const {
        TestStatistics result;
        result.skippedTests = this->skippedTests + other.skippedTests;
        result.failedTests = this->failedTests + other.failedTests;
        result.successfulTests = this->successfulTests + other.successfulTests;
        return result;
    }

    // Überladen des += Operators
    TestStatistics& operator+=(const TestStatistics& other) {
        this->skippedTests += other.skippedTests;
        this->failedTests += other.failedTests;
        this->successfulTests += other.successfulTests;
        return *this;
    }

private:
    CounterType skippedTests;
    CounterType failedTests;
    CounterType successfulTests;
};