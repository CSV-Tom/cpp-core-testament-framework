#include <Testament/Testament.hpp>

#include <expected>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace {

class ConsumerHandler final : public Testament::TestEventHandler {
public:
    std::string configure(Arguments arguments) override {
        configured = arguments.size() == 1 && arguments.front() == "--consumer";
        return {};
    }

    void onTestPassed(const SuiteInfo&, const TestInfo&) override {
        ++passed;
    }

    void onTestStart(const SuiteInfo& suite, const TestInfo& test) override {
        metadataReceived = suite.options.attribute("component") == "consumer"
            && test.options.attribute("operation") == "install-check";
    }

    int passed{};
    bool configured{};
    bool metadataReceived{};
};

}

int main() {
    const std::expected<int, int> cxx23Value{23};
    auto test = Testament::makeTest(
        "installed consumer test",
        Testament::TestOptions{}.attribute("operation", "install-check"),
        [] {}
    );
    auto suite = Testament::makeSuite(
        "installed consumer suite",
        Testament::SuiteOptions{}.attribute("component", "consumer"),
        std::move(test)
    );

    auto handler = std::make_unique<ConsumerHandler>();
    auto* handlerResult = handler.get();
    Testament::Runner runner;
    runner.addHandler(std::move(handler));

    std::string executable = "CoreTestamentConsumer";
    std::string option = "--consumer";
    char* arguments[]{executable.data(), option.data()};

    return cxx23Value.value() == 23
        && suite
        && runner.run(2, arguments) == 0
        && handlerResult->configured
        && handlerResult->metadataReceived
        && handlerResult->passed == 1
        ? 0
        : 1;
}
