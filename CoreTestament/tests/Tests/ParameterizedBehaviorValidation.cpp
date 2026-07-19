#include "Testament/Testament.hpp"

#include <array>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

inline std::array<unsigned int, 2> attempts{};
inline std::array<unsigned int, 2> callableState{};
inline unsigned int beforeAllCalls{};
inline unsigned int afterAllCalls{};
inline unsigned int beforeEachCalls{};
inline unsigned int afterEachCalls{};

class Fixture final : public Testament::LifecycleSuite {
    void beforeAll() override { ++beforeAllCalls; }
    void afterAll() override { ++afterAllCalls; }
    void beforeEach() override { ++beforeEachCalls; }
    void afterEach() override { ++afterEachCalls; }
};

class RecordingHandler final : public Testament::TestEventHandler {
public:
    void onTestPassed(const SuiteInfo&, const TestInfo& test) override {
        names.push_back(test.name);
    }

    std::vector<std::string> names;
};

class StatefulCallable {
public:
    void operator()(Fixture&, unsigned int index) {
        callableState[index] = ++calls;
    }

private:
    unsigned int calls{};
};

inline const auto suite = Testament::Suite<Fixture>(
    "parameter behavior",
    Testament::ParameterizedTest(
        "independent callable",
        Testament::Cases(
            Testament::TestCase("first", 0U),
            Testament::TestCase("second", 1U)
        ),
        StatefulCallable{}
    ),
    Testament::ParameterizedTest(
        "retry cases",
        Testament::TestOptions{}.maxAttempts(2),
        Testament::Cases(
            Testament::TestCase("flaky", 0U),
            Testament::TestCase("stable", 1U)
        ),
        [](Fixture&, unsigned int index) {
            ++attempts[index];
            if (index == 0 && attempts[index] == 1) throw std::runtime_error("retry");
        }
    )
);

}

int main() {
    auto handler = std::make_unique<RecordingHandler>();
    auto* result = handler.get();
    Testament::Runner runner;
    runner.addHandler(std::move(handler));

    return suite
        && runner.run(0, nullptr) == 0
        && attempts == std::array<unsigned int, 2>{2, 1}
        && callableState == std::array<unsigned int, 2>{1, 1}
        && beforeAllCalls == 1
        && afterAllCalls == 1
        && beforeEachCalls == 5
        && afterEachCalls == 5
        && result->names == std::vector<std::string>{
            "independent callable / first", "independent callable / second",
            "retry cases / flaky", "retry cases / stable"
        }
        ? 0
        : 1;
}
