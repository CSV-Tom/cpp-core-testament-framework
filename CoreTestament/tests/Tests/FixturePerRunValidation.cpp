#include "Testament/Testament.hpp"

#include <stdexcept>

namespace {

inline bool throwOnConstruction = false;
inline unsigned int constructionAttempts = 0;
inline unsigned int destructions = 0;
inline unsigned int executions = 0;

class Fixture final : public Testament::LifecycleSuite {
public:
    Fixture() {
        ++constructionAttempts;
        if (throwOnConstruction) throw std::runtime_error("expected constructor failure");
    }

    ~Fixture() override {
        ++destructions;
    }

    unsigned int state{};
};

inline const auto suite = Testament::Suite<Fixture>(
    "fixture per run",
    Testament::Test("fresh state", [](Fixture& fixture) {
        Testament::Asserts::assertEquals(0U, fixture.state);
        ++fixture.state;
        ++executions;
    })
);

}

int main() {
    if (!suite || constructionAttempts != 0) return 1;

    Testament::Runner runner;
    throwOnConstruction = true;
    const auto failedRun = runner.run(0, nullptr);

    throwOnConstruction = false;
    const auto firstSuccessfulRun = runner.run(0, nullptr);
    const auto secondSuccessfulRun = runner.run(0, nullptr);

    return failedRun == 1
        && firstSuccessfulRun == 0
        && secondSuccessfulRun == 0
        && constructionAttempts == 3
        && destructions == 2
        && executions == 2
        ? 0
        : 1;
}
