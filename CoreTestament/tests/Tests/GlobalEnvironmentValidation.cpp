#include "Testament/Testament.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

class Environment final : public Testament::GlobalEnvironment {
public:
    Environment(std::string name_, std::vector<std::string>& events_, bool fail = false)
        : name(std::move(name_)), events(events_), failSetup(fail) {}

    void setUp() override {
        events.push_back("setup " + name);
        if (failSetup) throw std::runtime_error("expected setup failure");
    }

    void tearDown() override {
        events.push_back("teardown " + name);
    }

private:
    std::string name;
    std::vector<std::string>& events;
    bool failSetup;
};

}

int main() {
    std::vector<std::string> events;
    auto suite = Testament::Suite(
        "environment suite",
        Testament::Test("environment test", [&events] { events.push_back("test"); })
    );

    Testament::Runner runner;
    runner.addEnvironment(std::make_unique<Environment>("first", events))
        .addEnvironment(std::make_unique<Environment>("second", events));
    const auto successfulRun = runner.run(0, nullptr);

    std::vector<std::string> failedEvents;
    Testament::Runner failingRunner;
    failingRunner.addEnvironment(std::make_unique<Environment>("started", failedEvents))
        .addEnvironment(std::make_unique<Environment>("failing", failedEvents, true));
    const auto failedRun = failingRunner.run(0, nullptr);

    return suite
        && successfulRun == 0
        && failedRun == 1
        && events == std::vector<std::string>{
            "setup first", "setup second", "test", "teardown second", "teardown first"
        }
        && failedEvents == std::vector<std::string>{
            "setup started", "setup failing", "teardown started"
        }
        ? 0
        : 1;
}
