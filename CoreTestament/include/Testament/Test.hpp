#ifndef TESTFRAMEWORK_TESTAMENT_TEST_HPP
#define TESTFRAMEWORK_TESTAMENT_TEST_HPP

#include <functional>
#include <memory>
#include <string>

namespace Testament {

class Suite;
class Test;

namespace detail {

class TestAccess;
Test makeTest(std::string name, std::function<void()> function);
Test makeTest(std::string name, std::function<void(Suite&)> function);

}

class Test {
public:
    ~Test();

    Test(Test&&) noexcept;
    Test& operator=(Test&&) noexcept;

    Test(const Test&) = delete;
    Test& operator=(const Test&) = delete;

private:
    class Impl;

    explicit Test(std::unique_ptr<Impl> impl);

    friend Test detail::makeTest(std::string, std::function<void()>);
    friend Test detail::makeTest(std::string, std::function<void(Suite&)>);
    friend class detail::TestAccess;

    std::unique_ptr<Impl> impl;
};

}

#endif
