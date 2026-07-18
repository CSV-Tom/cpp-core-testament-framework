#include "Testament/Testament.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

class ArgumentHandler final : public Testament::TestEventHandler {
public:
    explicit ArgumentHandler(bool reject_) : reject(reject_) {}

    std::expected<void, std::string> configure(Arguments arguments) override {
        receivedArguments.assign(arguments.begin(), arguments.end());
        if (reject) return std::unexpected("handler rejected arguments");
        return {};
    }

    void onTestPassed(const SuiteInfo&, const TestInfo&) override {
        ++passed;
    }

    bool reject;
    int passed{};
    std::vector<std::string_view> receivedArguments;
};

}

int main() {
    int executions = 0;
    auto suite = Testament::Suite(
        "handler arguments",
        Testament::Test("runs", [&executions] { ++executions; })
    );

    auto firstHandler = std::make_unique<ArgumentHandler>(false);
    auto secondHandler = std::make_unique<ArgumentHandler>(false);
    auto* firstResult = firstHandler.get();
    auto* secondResult = secondHandler.get();

    Testament::Runner runner;
    runner.addHandler(std::move(firstHandler)).addHandler(std::move(secondHandler));

    std::string executable = "HandlerArgumentsValidation";
    std::string option = "--custom=value";
    std::string positional = "input";
    char* arguments[]{executable.data(), option.data(), positional.data()};
    const int successfulRun = runner.run(3, arguments);

    auto rejectingHandler = std::make_unique<ArgumentHandler>(true);
    auto* rejectingResult = rejectingHandler.get();
    Testament::Runner rejectingRunner;
    rejectingRunner.addHandler(std::move(rejectingHandler));
    const int rejectedRun = rejectingRunner.run(3, arguments);

    return suite
        && successfulRun == 0
        && rejectedRun == 2
        && executions == 1
        && firstResult->passed == 1
        && secondResult->passed == 1
        && rejectingResult->passed == 0
        && firstResult->receivedArguments == std::vector<std::string_view>{option, positional}
        && secondResult->receivedArguments == firstResult->receivedArguments
        ? 0
        : 1;
}
