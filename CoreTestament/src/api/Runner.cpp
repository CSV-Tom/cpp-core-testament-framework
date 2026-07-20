#include "Testament/Runner.hpp"

#include "Testament/GlobalEnvironment.hpp"
#include "Testament/TestEventHandler.hpp"

#include "reporting/CompositeTestEventHandler.hpp"
#include "configuration/CommandLineOptions.hpp"
#include "runtime/EnvironmentSession.hpp"
#include "runtime/SuiteRegistry.hpp"
#include "runtime/SuiteScheduler.hpp"
#include "runtime/SuiteCatalog.hpp"
#include "runtime/TestCounts.hpp"

#include <cstdint>
#include <iostream>
#include <mutex>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Testament {

namespace {

std::mutex testRunMutex;

}

class Runner::Impl {
public:
    std::vector<std::unique_ptr<TestEventHandler>> handlers;
    std::vector<std::unique_ptr<GlobalEnvironment>> environments;
    std::optional<std::string> suiteFilter;
    std::optional<std::string> testFilter;
    std::size_t maxParallelSuites{1};
    std::size_t maxParallelTests{1};
};

Runner::Runner() : pImpl(std::make_unique<Impl>()) {}

Runner::~Runner() = default;

Runner::Runner(Runner&&) noexcept = default;

Runner& Runner::operator=(Runner&&) noexcept = default;

Runner& Runner::addHandler(std::unique_ptr<TestEventHandler> handler) {
    if (handler) {
        if (!pImpl) pImpl = std::make_unique<Impl>();
        pImpl->handlers.push_back(std::move(handler));
    }
    return *this;
}

Runner& Runner::addEnvironment(std::unique_ptr<GlobalEnvironment> environment) {
    if (environment) {
        if (!pImpl) pImpl = std::make_unique<Impl>();
        pImpl->environments.push_back(std::move(environment));
    }
    return *this;
}

Runner& Runner::filterSuite(std::string_view name) {
    if (name.empty()) throw std::invalid_argument("Suite filter cannot be empty");
    if (!pImpl) pImpl = std::make_unique<Impl>();
    pImpl->suiteFilter = name;
    return *this;
}

Runner& Runner::filterTest(std::string_view name) {
    if (name.empty()) throw std::invalid_argument("Test filter cannot be empty");
    if (!pImpl) pImpl = std::make_unique<Impl>();
    pImpl->testFilter = name;
    return *this;
}

Runner& Runner::clearFilters() noexcept {
    if (pImpl) {
        pImpl->suiteFilter.reset();
        pImpl->testFilter.reset();
    }
    return *this;
}

Runner& Runner::maxParallelSuites(std::size_t count) {
    if (count == 0) throw std::invalid_argument("Maximum parallel suites must be greater than zero");
    if (!pImpl) pImpl = std::make_unique<Impl>();
    pImpl->maxParallelSuites = count;
    return *this;
}

Runner& Runner::maxParallelTests(std::size_t count) {
    if (count == 0) throw std::invalid_argument("Maximum parallel tests must be greater than zero");
    if (!pImpl) pImpl = std::make_unique<Impl>();
    pImpl->maxParallelTests = count;
    return *this;
}

int Runner::run(int argc, char** argv) {
    const auto commandLine = detail::CommandLineOptions::parse(argc, argv);
    if (!commandLine) {
        std::cerr << commandLine.error() << '\n';
        return 2;
    }

    const std::scoped_lock runLock(testRunMutex);
    if (!pImpl) pImpl = std::make_unique<Impl>();

    auto& registry = detail::SuiteRegistry::instance();
    if (const auto errors = registry.configurationErrors(); !errors.empty()) {
        for (const auto& error : errors) std::cerr << "Test configuration error: " << error << '\n';
        return 2;
    }
    const detail::SuiteCatalog catalog{
        registry.suites(),
        pImpl->suiteFilter
            ? std::optional<std::string_view>{*pImpl->suiteFilter}
            : std::nullopt
    };
    const std::string testFilter = pImpl->testFilter.value_or("");
    const std::string cliFilter = commandLine->filter.value_or("");
    if (commandLine->listTests) {
        catalog.list(std::cout, testFilter, cliFilter);
        return 0;
    }

    detail::CompositeTestEventHandler chain;
    for (const auto& handler : pImpl->handlers) chain.add(handler.get());
    if (const auto result = chain.configure(commandLine->arguments); !result) {
        std::cerr << result.error() << '\n';
        return 2;
    }

    detail::EnvironmentSession environmentSession{pImpl->environments, chain};
    if (!environmentSession.start()) return 1;

    std::uint64_t baseSeed{};
    if (commandLine->seed) {
        baseSeed = *commandLine->seed;
    } else if (commandLine->shuffle) {
        baseSeed = (static_cast<std::uint64_t>(std::random_device{}()) << 32)
            ^ std::random_device{}();
    }

    bool allRunsSucceeded = true;
    detail::TestCounts<unsigned int> allStatistics;
    chain.onStartReport(static_cast<unsigned int>(catalog.suites().size() * commandLine->repeat));
    for (std::size_t repetition = 0; repetition < commandLine->repeat; ++repetition) {
        const auto runSeed = baseSeed + repetition;
        const auto shuffleSeed = commandLine->shuffle
            ? std::optional<std::uint64_t>{runSeed}
            : std::nullopt;
        if (shuffleSeed) std::cout << "[SEED] " << *shuffleSeed << '\n';

        const auto suites = catalog.forRun(shuffleSeed);
        const auto result = detail::SuiteScheduler::run(
            suites, chain,
            {testFilter, cliFilter, pImpl->maxParallelSuites,
             pImpl->maxParallelTests, shuffleSeed}
        );
        allStatistics += result.statistics;
        allRunsSucceeded = result.succeeded() && allRunsSucceeded;
    }

    const bool environmentsSucceeded = environmentSession.finish();

    chain.onFinalReport(RunSummary{
        static_cast<unsigned int>(catalog.suites().size() * commandLine->repeat),
        allStatistics.passedTests(),
        allStatistics.failedTests(),
        allStatistics.skippedTests(),
        allStatistics.errors(),
        environmentsSucceeded ? 0U : 1U,
        static_cast<unsigned int>(commandLine->repeat)
    });

    const auto handlerError = chain.errorMessage();
    if (!handlerError.empty()) {
        std::cerr << handlerError << '\n';
    }

    return allRunsSucceeded && environmentsSucceeded && handlerError.empty()
        ? 0
        : 1;
}

}
