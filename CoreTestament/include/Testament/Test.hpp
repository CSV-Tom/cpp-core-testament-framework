#ifndef TESTFRAMEWORK_TESTAMENT_TEST_HPP
#define TESTFRAMEWORK_TESTAMENT_TEST_HPP

#include <functional>
#include <memory>
#include <string_view>

namespace Testament {

class LifecycleSuite;
class Test;
class TestOptions;

namespace detail {

class TestAccess;
Test makeTest(std::string_view name, TestOptions options, std::function<void()> function);
Test makeTest(std::string_view name, TestOptions options,
              std::function<void(LifecycleSuite&)> function);

}

class [[nodiscard("the test handle must be added to a suite")]] Test {
public:
    ~Test();

    Test(Test&&) noexcept;
    Test& operator=(Test&&) noexcept;

    Test(const Test&) = delete;
    Test& operator=(const Test&) = delete;

    [[nodiscard]] explicit operator bool() const noexcept;

private:
    class Impl;

    explicit Test(std::unique_ptr<Impl> impl);

    friend Test detail::makeTest(std::string_view, TestOptions, std::function<void()>);
    friend Test detail::makeTest(std::string_view, TestOptions,
                                 std::function<void(LifecycleSuite&)>);
    friend class detail::TestAccess;

    std::unique_ptr<Impl> impl;
};

}

#endif
