#include <Testament/Testament.hpp>

#include <expected>
#include <memory>

namespace {

class ConsumerHandler final : public Testament::TestEventHandler {
public:
    void onTestPassed(const SuiteInfo&, const TestInfo&) override {
        ++passed;
    }

    int passed{};
};

}

int main() {
    const std::expected<int, int> cxx23Value{23};
    auto test = Testament::makeTest("installed consumer test", [] {});
    auto suite = Testament::makeSuite("installed consumer suite", test);

    auto handler = std::make_unique<ConsumerHandler>();
    auto* handlerResult = handler.get();
    Testament::Runner runner;
    runner.addHandler(std::move(handler));

    return cxx23Value.value() == 23
        && suite
        && runner.run(0, nullptr) == 0
        && handlerResult->passed == 1
        ? 0
        : 1;
}
