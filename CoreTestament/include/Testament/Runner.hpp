#ifndef TESTFRAMEWORK_TESTAMENT_RUNNER_HPP
#define TESTFRAMEWORK_TESTAMENT_RUNNER_HPP

#include "Testament/TestEventHandler.hpp"

#include <memory>

namespace Testament {

class Runner {
public:
    Runner();
    ~Runner();

    Runner(Runner&&) noexcept;
    Runner& operator=(Runner&&) noexcept;

    Runner(const Runner&) = delete;
    Runner& operator=(const Runner&) = delete;

    Runner& addHandler(std::unique_ptr<TestEventHandler> handler);
    [[nodiscard("propagate the test exit code, e.g. return runner.run(argc, argv)")]]
    int run(int argc, char** argv);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

[[nodiscard("propagate the test exit code, e.g. return Testament::run(argc, argv)")]]
int run(int argc, char** argv);

}

#endif
