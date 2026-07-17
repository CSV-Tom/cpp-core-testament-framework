#ifndef TESTFRAMEWORK_TESTAMENT_SUITE_HPP
#define TESTFRAMEWORK_TESTAMENT_SUITE_HPP

#include <memory>
#include <string>
#include <vector>

#include "Testament/Options.hpp"

namespace Testament {

class LifecycleSuite;
class Test;
class Suite;

namespace detail {

Suite makeSuite(std::string name, SuiteOptions options, std::vector<Test> tests);
Suite makeSuite(std::string name, std::unique_ptr<LifecycleSuite> fixture,
                SuiteOptions options, std::vector<Test> tests);

}

class [[nodiscard("the suite handle keeps its registration alive")]] Suite {
public:
    ~Suite();

    Suite(Suite&&) noexcept;
    Suite& operator=(Suite&&) noexcept;

    Suite(const Suite&) = delete;
    Suite& operator=(const Suite&) = delete;

    [[nodiscard]] explicit operator bool() const noexcept;

private:
    class Impl;

    explicit Suite(std::unique_ptr<Impl> impl);

    friend Suite detail::makeSuite(std::string, SuiteOptions, std::vector<Test>);
    friend Suite detail::makeSuite(std::string, std::unique_ptr<LifecycleSuite>,
                                  SuiteOptions, std::vector<Test>);

    std::unique_ptr<Impl> impl;
};

}

#endif
