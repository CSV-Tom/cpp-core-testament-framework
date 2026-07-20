#ifndef TESTFRAMEWORK_TESTAMENT_RUNNER_HPP
#define TESTFRAMEWORK_TESTAMENT_RUNNER_HPP

#include "Testament/Export.hpp"

#include <memory>
#include <cstddef>
#include <string_view>

namespace Testament {

class TestEventHandler;
class GlobalEnvironment;

class TESTAMENT_EXPORT Runner {
public:
    Runner();
    ~Runner();

    Runner(Runner&&) noexcept;
    Runner& operator=(Runner&&) noexcept;

    Runner(const Runner&) = delete;
    Runner& operator=(const Runner&) = delete;

    // Moving transfers all handlers. A moved-from Runner remains reusable as an empty Runner.
    Runner& addHandler(std::unique_ptr<TestEventHandler> handler);
    Runner& addEnvironment(std::unique_ptr<GlobalEnvironment> environment);
    // Filters accept glob patterns (* and ?); a leading '-' negates a pattern.
    Runner& filterSuite(std::string_view name);
    Runner& filterTest(std::string_view name);
    Runner& clearFilters() noexcept;
    // Fixture-less suites and tests may execute concurrently. Lifecycle suites stay serial.
    Runner& maxParallelSuites(std::size_t count);
    Runner& maxParallelTests(std::size_t count);
    // Concurrent calls are serialized. Complete suite registration before starting a run.
    [[nodiscard("propagate the test exit code, e.g. return runner.run(argc, argv)")]]
    int run(int argc, char** argv);

private:
    class TESTAMENT_NO_EXPORT Impl;
    std::unique_ptr<Impl> pImpl;
};

[[nodiscard("propagate the test exit code, e.g. return Testament::run(argc, argv)")]]
TESTAMENT_EXPORT int run(int argc, char** argv);

}

#endif
