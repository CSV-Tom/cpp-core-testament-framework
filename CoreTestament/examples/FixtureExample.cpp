// A LifecycleSuite fixture with all four hooks. See docs/guide.md for the full guide.
#include <Testament/Testament.hpp>

using namespace Testament;

namespace {

class DatabaseFixture : public LifecycleSuite {
protected:
    void beforeAll() override { connected = true; }
    void afterAll() override { connected = false; }
    void beforeEach() override { ++transactionCount; }
    void afterEach() override { --transactionCount; }

public:
    bool connected{false};
    int transactionCount{0};
};

inline const auto fixtureSuite = Suite<DatabaseFixture>(
    "Fixture Example",
    SuiteOptions{}.tag("example"),

    Test("fixture is connected before the test body runs", [](DatabaseFixture& fixture) {
        Asserts::assertTrue(fixture.connected, "beforeAll should have connected");
    }),

    Test("beforeEach runs once per attempt", [](DatabaseFixture& fixture) {
        Asserts::assertEquals(1, fixture.transactionCount);
    })
);

}
