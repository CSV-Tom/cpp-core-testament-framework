#include "Testament/Runner.hpp"

#include "Testament/GlobalEnvironment.hpp"
#include "Testament/Reporters.hpp"
#include "Testament/TestEventHandler.hpp"

#include "EventHandlers/ConsoleTestEventHandler.hpp"
#include "EventHandlers/JUnitTestEventHandler.hpp"
#include "EventHandlers/TestEventHandlerChain.hpp"
#include "Internal/InternalRegistry.hpp"
#include "Internal/InternalSuite.hpp"
#include "Internal/InternalTest.hpp"
#include "Internal/FilterPattern.hpp"
#include "Internal/utils/TestStatistics.hpp"

#include <algorithm>
#include <charconv>
#include <cstdint>
#include <filesystem>
#include <future>
#include <iostream>
#include <limits>
#include <mutex>
#include <optional>
#include <random>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Testament {

namespace {

std::mutex testRunMutex;

std::optional<std::vector<std::string_view>> commandLineArguments(int argc, char** argv) {
    if (argc < 0 || (argc > 0 && !argv)) {
        return std::nullopt;
    }

    std::vector<std::string_view> arguments;
    arguments.reserve(argc > 1 ? static_cast<std::size_t>(argc - 1) : 0U);
    for (int index = 1; index < argc; ++index) {
        if (!argv[index]) {
            return std::nullopt;
        }
        arguments.emplace_back(argv[index]);
    }
    return arguments;
}

void printTags(std::span<const std::string> tags) {
    std::cout << "tags:";
    if (tags.empty()) {
        std::cout << " -";
    } else {
        for (const auto& tag : tags) std::cout << ' ' << tag;
    }
}

std::optional<std::uint64_t> parseUnsigned(std::string_view value) {
    std::uint64_t result{};
    const auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), result);
    if (error != std::errc{} || end != value.data() + value.size()) return std::nullopt;
    return result;
}

class EnvironmentSession {
public:
    EnvironmentSession(
        const std::vector<std::unique_ptr<GlobalEnvironment>>& registeredEnvironments,
        TestEventHandler& eventHandler
    ) : environments(registeredEnvironments), handler(eventHandler) {}

    ~EnvironmentSession() { (void)finish(); }

    [[nodiscard]] bool start() noexcept {
        try {
            for (const auto& environment : environments) {
                environment->setUp();
                ++active;
            }
            return true;
        } catch (const std::exception& error) {
            handler.onEnvironmentError("setup", error.what());
        } catch (...) {
            handler.onEnvironmentError("setup", "unknown non-standard exception");
        }
        (void)finish();
        return false;
    }

    [[nodiscard]] bool finish() noexcept {
        while (active > 0) {
            try {
                environments[--active]->tearDown();
            } catch (const std::exception& error) {
                handler.onEnvironmentError("teardown", error.what());
                succeeded = false;
            } catch (...) {
                handler.onEnvironmentError("teardown", "unknown non-standard exception");
                succeeded = false;
            }
        }
        return succeeded;
    }

private:
    const std::vector<std::unique_ptr<GlobalEnvironment>>& environments;
    TestEventHandler& handler;
    std::size_t active{};
    bool succeeded{true};
};

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

Runner::Runner() : impl(std::make_unique<Impl>()) {}

Runner::~Runner() = default;

Runner::Runner(Runner&&) noexcept = default;

Runner& Runner::operator=(Runner&&) noexcept = default;

Runner& Runner::addHandler(std::unique_ptr<TestEventHandler> handler) {
    if (handler) {
        if (!impl) impl = std::make_unique<Impl>();
        impl->handlers.push_back(std::move(handler));
    }
    return *this;
}

Runner& Runner::addEnvironment(std::unique_ptr<GlobalEnvironment> environment) {
    if (environment) {
        if (!impl) impl = std::make_unique<Impl>();
        impl->environments.push_back(std::move(environment));
    }
    return *this;
}

Runner& Runner::filterSuite(std::string_view name) {
    if (name.empty()) throw std::invalid_argument("Suite filter cannot be empty");
    if (!impl) impl = std::make_unique<Impl>();
    impl->suiteFilter = name;
    return *this;
}

Runner& Runner::filterTest(std::string_view name) {
    if (name.empty()) throw std::invalid_argument("Test filter cannot be empty");
    if (!impl) impl = std::make_unique<Impl>();
    impl->testFilter = name;
    return *this;
}

Runner& Runner::clearFilters() noexcept {
    if (impl) {
        impl->suiteFilter.reset();
        impl->testFilter.reset();
    }
    return *this;
}

Runner& Runner::maxParallelSuites(std::size_t count) {
    if (count == 0) throw std::invalid_argument("Maximum parallel suites must be greater than zero");
    if (!impl) impl = std::make_unique<Impl>();
    impl->maxParallelSuites = count;
    return *this;
}

Runner& Runner::maxParallelTests(std::size_t count) {
    if (count == 0) throw std::invalid_argument("Maximum parallel tests must be greater than zero");
    if (!impl) impl = std::make_unique<Impl>();
    impl->maxParallelTests = count;
    return *this;
}

int Runner::run(int argc, char** argv) {
    const auto arguments = commandLineArguments(argc, argv);
    if (!arguments) {
        std::cerr << "Invalid command-line arguments\n";
        return 2;
    }
    std::optional<std::string> commandLineFilter;
    bool listTests{};
    std::size_t repeat{1};
    bool shuffle{};
    std::optional<std::uint64_t> requestedSeed;
    for (const auto argument : *arguments) {
        if (argument == "--list-tests") {
            listTests = true;
            continue;
        }
        if (argument == "--shuffle") {
            shuffle = true;
            continue;
        }
        if (argument.starts_with("--repeat=")) {
            const auto parsed = parseUnsigned(argument.substr(9));
            if (!parsed || *parsed == 0
                || *parsed > std::numeric_limits<std::size_t>::max()) {
                std::cerr << "--repeat requires an integer greater than zero\n";
                return 2;
            }
            repeat = static_cast<std::size_t>(*parsed);
            continue;
        }
        if (argument.starts_with("--seed=")) {
            requestedSeed = parseUnsigned(argument.substr(7));
            if (!requestedSeed) {
                std::cerr << "--seed requires an unsigned integer\n";
                return 2;
            }
            shuffle = true;
            continue;
        }
        if (!argument.starts_with("--filter=")) continue;
        if (commandLineFilter) {
            std::cerr << "--filter may only be specified once\n";
            return 2;
        }
        const auto value = argument.substr(std::string_view{"--filter="}.size());
        if (value.empty() || value == "-" || value == "tag:" || value == "-tag:") {
            std::cerr << "--filter requires a non-empty pattern\n";
            return 2;
        }
        commandLineFilter = value;
    }

    const std::scoped_lock runLock(testRunMutex);
    if (!impl) impl = std::make_unique<Impl>();

    auto& registry = InternalRegistry::getInstance();
    if (const auto errors = registry.getConfigurationErrors(); !errors.empty()) {
        for (const auto& error : errors) std::cerr << "Test configuration error: " << error << '\n';
        return 2;
    }
    auto suites = registry.getAllSuites();
    if (impl->suiteFilter) {
        std::erase_if(suites, [this](const auto& suite) {
            return !detail::matchesNameFilter(suite->getName(), *impl->suiteFilter);
        });
    }
    std::ranges::sort(suites, [](const auto& left, const auto& right) {
        const auto leftOrder = left->getOptions().order().value_or(0);
        const auto rightOrder = right->getOptions().order().value_or(0);
        if (leftOrder != rightOrder) {
            return leftOrder < rightOrder;
        }
        return left->getName() < right->getName();
    });

    const std::string testFilter = impl->testFilter.value_or("");
    const std::string cliFilter = commandLineFilter.value_or("");
    if (listTests) {
        for (const auto& suite : suites) {
            std::cout << suite->getName() << " [";
            printTags(suite->getOptions().tags());
            std::cout << "]\n";
            for (const auto& test : suite->getTests()) {
                if ((!testFilter.empty()
                     && !detail::matchesNameFilter(test->getName(), testFilter))
                    || (!cliFilter.empty()
                        && !detail::matchesTestFilter(
                            suite->getName(), suite->getOptions().tags(),
                            test->getName(), test->getOptions().tags(), cliFilter
                        ))) {
                    continue;
                }
                std::cout << "  " << test->getName() << " [";
                printTags(test->getOptions().tags());
                std::cout << ", "
                          << (test->getOptions().isDisabled() ? "disabled" : "enabled")
                          << "]\n";
            }
        }
        return 0;
    }

    detail::TestEventHandlerChain chain;
    for (const auto& handler : impl->handlers) chain.add(handler.get());
    if (const auto result = chain.configure(*arguments); !result) {
        std::cerr << result.error() << '\n';
        return 2;
    }

    EnvironmentSession environmentSession{impl->environments, chain};
    if (!environmentSession.start()) return 1;

    const auto registeredSuites = suites;
    std::uint64_t baseSeed{};
    if (requestedSeed) {
        baseSeed = *requestedSeed;
    } else if (shuffle) {
        baseSeed = (static_cast<std::uint64_t>(std::random_device{}()) << 32)
            ^ std::random_device{}();
    }
    bool allRunsSucceeded = true;
    TestStatistics<unsigned int> allStatistics;
    bool allHooksSucceeded = true;
    chain.onStartReport(static_cast<unsigned int>(suites.size() * repeat));
    for (std::size_t repetition = 0; repetition < repeat; ++repetition) {
    suites = registeredSuites;
    const auto runSeed = baseSeed + repetition;
    if (shuffle) {
        std::cout << "[SEED] " << runSeed << '\n';
        std::mt19937_64 random{runSeed};
        std::ranges::shuffle(suites, random);
        std::ranges::stable_sort(suites, [](const auto& left, const auto& right) {
            return left->getOptions().order().value_or(0)
                < right->getOptions().order().value_or(0);
        });
    }

    struct SuiteResult {
        bool hooksSucceeded;
        TestStatistics<unsigned int> statistics;
    };
    const auto executeSuite = [this, &chain, &testFilter, &cliFilter, shuffle, runSeed](
        const auto& suite
    ) {
        SuiteResult result;
        result.hooksSucceeded = suite->run(
            &chain,
            InternalSuite::RunConfiguration{
                testFilter, cliFilter, impl->maxParallelTests,
                shuffle ? std::optional<std::uint64_t>{runSeed} : std::nullopt
            }
        );
        result.statistics = suite->getStatistics();
        return result;
    };

    TestStatistics<unsigned int> total;
    bool hooksSucceeded = true;
    const auto consume = [&hooksSucceeded, &total](SuiteResult result) {
        hooksSucceeded = result.hooksSucceeded && hooksSucceeded;
        total += result.statistics;
    };

    std::size_t index{};
    while (index < suites.size()) {
        if (suites[index]->getOptions().execution() == Execution::Serial) {
            consume(executeSuite(suites[index++]));
            continue;
        }

        const auto concurrentEnd = std::ranges::find_if(
            suites.begin() + static_cast<std::ptrdiff_t>(index), suites.end(),
            [](const auto& suite) {
                return suite->getOptions().execution() == Execution::Serial;
            }
        );
        const auto endIndex = static_cast<std::size_t>(concurrentEnd - suites.begin());
        while (index < endIndex) {
            const auto count = std::min(impl->maxParallelSuites, endIndex - index);
            std::vector<std::future<SuiteResult>> running;
            running.reserve(count);
            for (std::size_t offset = 0; offset < count; ++offset) {
                running.push_back(std::async(
                    std::launch::async, executeSuite, suites[index + offset]
                ));
            }
            for (auto& future : running) consume(future.get());
            index += count;
        }
    }

    allStatistics += total;
    allHooksSucceeded = hooksSucceeded && allHooksSucceeded;
    allRunsSucceeded = total.getFailedTests() == 0 && total.getErrors() == 0
        && hooksSucceeded && allRunsSucceeded;
    }

    const bool environmentsSucceeded = environmentSession.finish();

    chain.onFinalReport(RunSummary{
        static_cast<unsigned int>(registeredSuites.size() * repeat),
        allStatistics.getPassedTests(),
        allStatistics.getFailedTests(),
        allStatistics.getSkippedTests(),
        allStatistics.getErrors(),
        environmentsSucceeded ? 0U : 1U,
        static_cast<unsigned int>(repeat)
    });

    const auto handlerError = chain.errorMessage();
    if (!handlerError.empty()) {
        std::cerr << handlerError << '\n';
    }

    return allRunsSucceeded && allHooksSucceeded && environmentsSucceeded && handlerError.empty() ? 0 : 1;
}

std::unique_ptr<TestEventHandler> makeConsoleHandler() {
    return std::make_unique<detail::ConsoleTestEventHandler>();
}

std::unique_ptr<TestEventHandler> makeJUnitHandler() {
    return std::make_unique<detail::JUnitTestEventHandler>();
}

std::unique_ptr<TestEventHandler> makeJUnitHandler(std::filesystem::path outputPath) {
    return std::make_unique<detail::JUnitTestEventHandler>(std::move(outputPath));
}

int run(int argc, char** argv) {
    Runner runner;
    runner.addHandler(makeConsoleHandler());
    runner.addHandler(makeJUnitHandler());
    return runner.run(argc, argv);
}

}
