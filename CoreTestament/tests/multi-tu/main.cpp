#include "Testament/Testament.hpp"

#include <memory>

namespace {

class CountingHandler final : public Testament::TestEventHandler {
public:
    void onSuiteStart(const SuiteInfo&) override { ++suites; }
    void onTestPassed(const SuiteInfo&, const TestInfo&) override { ++tests; }

    unsigned int suites{};
    unsigned int tests{};
};

}

int main() {
    auto handler = std::make_unique<CountingHandler>();
    auto* result = handler.get();
    Testament::Runner runner;
    runner.addHandler(std::move(handler));
    return runner.run(0, nullptr) == 0 && result->suites == 2 && result->tests == 2 ? 0 : 1;
}
