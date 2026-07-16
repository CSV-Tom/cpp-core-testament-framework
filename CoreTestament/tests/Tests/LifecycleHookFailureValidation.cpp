#include "Testament/Test.hpp"
#include "Testament/Runner.hpp"

#include "core/Internal/InternalRegistry.hpp"
#include "core/Internal/InternalSuite.hpp"

#include <memory>
#include <stdexcept>

int main() {
    auto suite = Testament::InternalRegistry::getInstance().registerSuite(
        std::make_shared<Testament::InternalSuite>("failing lifecycle hook")
    );
    bool testExecuted = false;

    suite->setBeforeSuite([] {
        throw std::runtime_error("expected hook failure");
    });
    suite->addTest(Testament::Test::create("must not run", [&testExecuted] {
        testExecuted = true;
    }));

    const int exitCode = Testament::Runner::run(0, nullptr);
    return exitCode == 1 && !testExecuted && suite->getStatistics().getTotalTests() == 0
        ? 0
        : 1;
}
