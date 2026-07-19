#include "Testament/Testament.hpp"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

namespace {

class UnexpectedHandler final : public Testament::TestEventHandler {
public:
    std::expected<void, std::string> configure(Arguments) override {
        configured = true;
        return std::unexpected("list mode must not configure handlers");
    }

    bool configured{};
};

}

int main() {
    unsigned int executions{};
    auto suite = Testament::Suite(
        "listed suite",
        Testament::SuiteOptions{}.tag("integration"),
        Testament::Test(
            "enabled test",
            Testament::TestOptions{}.tag("fast"),
            [&executions] { ++executions; }
        ),
        Testament::Test(
            "disabled test",
            Testament::TestOptions{}.tag("slow").disabled(),
            [&executions] { ++executions; }
        )
    );

    auto handler = std::make_unique<UnexpectedHandler>();
    auto* unexpected = handler.get();
    Testament::Runner runner;
    runner.addHandler(std::move(handler));

    std::string executable{"ListTestsValidation"};
    std::string listOption{"--list-tests"};
    char* arguments[]{executable.data(), listOption.data()};
    std::ostringstream output;
    auto* previous = std::cout.rdbuf(output.rdbuf());
    const auto exitCode = runner.run(2, arguments);
    std::cout.rdbuf(previous);

    const auto listing = output.str();
    return suite
        && exitCode == 0
        && executions == 0
        && !unexpected->configured
        && listing.contains("listed suite [tags: integration]")
        && listing.contains("enabled test [tags: fast, enabled]")
        && listing.contains("disabled test [tags: slow, disabled]")
        ? 0
        : 1;
}
