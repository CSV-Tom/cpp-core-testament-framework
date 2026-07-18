#include "Testament/Testament.hpp"

#include <array>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

inline std::array<unsigned int, 2> attempts{};
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

inline const auto suite = Testament::Suite<Fixture>(
    "parameter behavior",
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
        && beforeAllCalls == 1
        && afterAllCalls == 1
        && beforeEachCalls == 3
        && afterEachCalls == 3
        && result->names == std::vector<std::string>{
            "retry cases / flaky", "retry cases / stable"
        }
        ? 0
        : 1;
}
