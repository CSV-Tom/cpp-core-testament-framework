#include "Testament/Testament.hpp"

int main(int argc, char** argv) {

    return Testament::Runner::run(argc, argv);
}



/*import TestFramework.TestStatistics; // Modul importieren
#include <iostream>

int main() {
    // Erstelle ein TestStatistics-Objekt mit Standard-CounterType
    TestStatistics<> stats;

    // Initiale Werte ausgeben
    std::cout << "Initial skipped tests: " << stats.getSkippedTests() << "\n";
    std::cout << "Initial failed tests: " << stats.getFailedTests() << "\n";
    std::cout << "Initial passed tests: " << stats.getPassedTests() << "\n";
    std::cout << "Initial total tests: " << stats.getTotalTests() << "\n";

    // Werte ändern
    stats.incrementSkippedTests();
    stats.incrementFailedTests();
    stats.incrementPassedTests();
    stats.incrementPassedTests();

    // Geänderte Werte ausgeben
    std::cout << "Skipped tests after increment: " << stats.getSkippedTests() << "\n";
    std::cout << "Failed tests after increment: " << stats.getFailedTests() << "\n";
    std::cout << "Passed tests after increment: " << stats.getPassedTests() << "\n";
    std::cout << "Total tests after increment: " << stats.getTotalTests() << "\n";

    // Reset aufrufen
    stats.reset();
    std::cout << "After reset, total tests: " << stats.getTotalTests() << "\n";

    // Teste Operatorüberladung
    TestStatistics<> otherStats;
    otherStats.incrementSkippedTests();
    otherStats.incrementSkippedTests();
    otherStats.incrementFailedTests();

    stats += otherStats;
    std::cout << "After operator+=, skipped tests: " << stats.getSkippedTests() << "\n";
    std::cout << "After operator+=, failed tests: " << stats.getFailedTests() << "\n";

    return 0;
}*/

/*
#include "CoreTestamentFramework/CoreTestament.hpp"

static auto exampleSuiteAsserts = makeSuite("Example Suite",
    makeTest("Simple Assertion Test", []() {
        Asserts::assertTrue(true, "This should not fail");
        Asserts::assertFalse(false, "This should also not fail");
    }),
    makeTest("Equality Assertion Test", []() {
        Asserts::assertEquals(42, 42, "Equality check");
    }),
    makeTest("Range Assertion Test", []() {
        Asserts::assertInRange(5, 1, 10, "Value is within range");
        Asserts::assertInRange(15, 1, 10, "This should fail");
    })
);

class MyTestSuite : public TestSuite {
public:
    explicit MyTestSuite(const std::string& name) : TestSuite(name), counter(0) {}

    void beforeAll() override {
        std::cout << "[Setup] Running beforeAll for suite: " << getName() << "\n";
        counter = 0; // Initialisierung
    }

    void beforeEach() override {
        std::cout << "[Setup] Running beforeEach for suite: " << getName() << "\n";
        testState = "Preparing test";
    }

    void afterEach() override {
        std::cout << "[Teardown] Running afterEach for suite: " << getName() << "\n";
        testState = "";
    }

    void afterAll() override {
        std::cout << "[Teardown] Running afterAll for suite: " << getName() << "\n";
        std::cout << "Total tests executed: " << counter << "\n";
    }

    void incrementCounter() { counter++; }

    int counter;
private:

    std::string testState;
};

static auto exampleSuiteTeardown = makeSuite<MyTestSuite>("Example Suite with Setup and Teardown",
    makeTest<MyTestSuite>("Simple Test", [](MyTestSuite& suite) {
        suite.incrementCounter();
        std::cout << "Counter: " << std::to_string(suite.counter) << std::endl;
    }),
    makeTest<MyTestSuite>("Another Simple Test", [](MyTestSuite& suite) {
        suite.incrementCounter();
        std::cout << "Counter: " << std::to_string(suite.counter) << std::endl;
    })
);


static auto parameterizedSuite = makeSuite<MyTestSuite>("Parameterized Suite",
    makeParameterizedTest<MyTestSuite>("Division Tests",
        [](MyTestSuite& suite, int a, int b, int expected) {
            std::cout << "Executing Division Test with a = " << a << ", b = " << b << "\n";

            if (b == 0) {
                throw std::logic_error("Division by zero");
            }

            if (a / b != expected) {
                throw std::logic_error("Division test failed");
            }

            suite.incrementCounter();
        },
        std::vector<std::tuple<int, int, int>>{
            {10, 2, 5},
            {20, 4, 5},
            {30, 6, 5} // Dieser Test schlägt fehl
        }
    )
);


static auto booleanAssertionTests = makeSuite("Boolean Assertions Tests",
    makeTest("AssertTrue Pass Test", []() {
        Asserts::assertTrue(true, "AssertTrue: Condition is explicitly expected to be true");
    }),
    makeTest("AssertFalse Pass Test", []() {
        Asserts::assertFalse(false, "AssertFalse: Condition is explicitly expected to be false");
    })
);


int main() {
    return TestRegistry::instance().run();
}

*/