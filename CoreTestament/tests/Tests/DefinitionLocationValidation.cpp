#include "Testament/Testament.hpp"

#include <cstdint>
#include <memory>
#include <source_location>
#include <string_view>
#include <utility>

namespace {

class RecordingHandler final : public Testament::TestEventHandler {
public:
    void onSuiteStart(const SuiteInfo& suite) override {
        suiteLocation = suite.location;
    }

    void onTestStart(const SuiteInfo&, const TestInfo& test) override {
        testLocation = test.location;
    }

    std::source_location suiteLocation;
    std::source_location testLocation;
};

}

int main() {
    const auto testLine = static_cast<std::uint_least32_t>(__LINE__ + 1);
    auto test = Testament::Test("located test", [] {});
    const auto suiteLine = static_cast<std::uint_least32_t>(__LINE__ + 1);
    auto suite = Testament::Suite("located suite", std::move(test));

    auto handler = std::make_unique<RecordingHandler>();
    auto* result = handler.get();
    Testament::Runner runner;
    runner.addHandler(std::move(handler));

    return suite
        && runner.run(0, nullptr) == 0
        && std::string_view{result->testLocation.file_name()} == __FILE__
        && result->testLocation.line() == testLine
        && std::string_view{result->suiteLocation.file_name()} == __FILE__
        && result->suiteLocation.line() == suiteLine
        ? 0
        : 1;
}
