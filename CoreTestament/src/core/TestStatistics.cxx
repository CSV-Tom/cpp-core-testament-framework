module;

#include <memory>

module TestFramework.TestStatistics;

template <typename CounterType>
class TestStatistics<CounterType>::Impl {
public:
    CounterType skippedTests = 0;
    CounterType failedTests = 0;
    CounterType successfulTests = 0;

    CounterType getTotalTests() const {
        return skippedTests + failedTests + successfulTests;
    }
};

// Konstruktor und Destruktor
template <typename CounterType>
TestStatistics<CounterType>::TestStatistics() : pImpl(std::make_unique<Impl>()) {}

template <typename CounterType>
TestStatistics<CounterType>::~TestStatistics() = default;

// Getter-Implementierungen
template <typename CounterType>
CounterType TestStatistics<CounterType>::getSkippedTests() const {
    return pImpl->skippedTests;
}

template <typename CounterType>
CounterType TestStatistics<CounterType>::getFailedTests() const {
    return pImpl->failedTests;
}

template <typename CounterType>
CounterType TestStatistics<CounterType>::getPassedTests() const {
    return pImpl->successfulTests;
}

template <typename CounterType>
CounterType TestStatistics<CounterType>::getTotalTests() const {
    return pImpl->getTotalTests();
}

// Setter-Implementierungen
template <typename CounterType>
void TestStatistics<CounterType>::incrementSkippedTests() {
    ++pImpl->skippedTests;
}

template <typename CounterType>
void TestStatistics<CounterType>::incrementFailedTests() {
    ++pImpl->failedTests;
}

template <typename CounterType>
void TestStatistics<CounterType>::incrementPassedTests() {
    ++pImpl->successfulTests;
}

template <typename CounterType>
void TestStatistics<CounterType>::reset() {
    pImpl->skippedTests = 0;
    pImpl->failedTests = 0;
    pImpl->successfulTests = 0;
}

// Operator-Implementierungen
template <typename CounterType>
TestStatistics<CounterType> TestStatistics<CounterType>::operator+(const TestStatistics& other) const {
    TestStatistics result;
    result.pImpl->skippedTests = this->pImpl->skippedTests + other.pImpl->skippedTests;
    result.pImpl->failedTests = this->pImpl->failedTests + other.pImpl->failedTests;
    result.pImpl->successfulTests = this->pImpl->successfulTests + other.pImpl->successfulTests;
    return result;
}

template <typename CounterType>
TestStatistics<CounterType>& TestStatistics<CounterType>::operator+=(const TestStatistics& other) {
    this->pImpl->skippedTests += other.pImpl->skippedTests;
    this->pImpl->failedTests += other.pImpl->failedTests;
    this->pImpl->successfulTests += other.pImpl->successfulTests;
    return *this;
}
