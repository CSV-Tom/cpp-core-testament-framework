module;

#include <memory> // Für std::unique_ptr

export module TestFramework.TestStatistics;

export template <typename CounterType = unsigned int>
class TestStatistics {
private:
    // Pimpl-Zeiger auf die Implementierung
    class Impl;
    std::unique_ptr<Impl> pImpl;

public:
    // Konstruktor und Destruktor
    TestStatistics();
    ~TestStatistics();

    // Getter
    CounterType getSkippedTests() const;
    CounterType getFailedTests() const;
    CounterType getPassedTests() const;
    CounterType getTotalTests() const;

    // Setter
    void incrementSkippedTests();
    void incrementFailedTests();
    void incrementPassedTests();
    void reset();

    // Operatoren
    TestStatistics operator+(const TestStatistics& other) const;
    TestStatistics& operator+=(const TestStatistics& other);
};
