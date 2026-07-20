#ifndef TESTAMENT_DETAIL_TESTHANDLE_HPP
#define TESTAMENT_DETAIL_TESTHANDLE_HPP

#include <memory>

namespace Testament::detail {

class TestInstance;
class TestAccess;

class TestHandle {
public:
    ~TestHandle();

    TestHandle(TestHandle&&) noexcept;
    TestHandle& operator=(TestHandle&&) noexcept;

    TestHandle(const TestHandle&) = delete;
    TestHandle& operator=(const TestHandle&) = delete;

private:
    class Impl;
    explicit TestHandle(std::unique_ptr<Impl> implementation);

    friend class RuntimeBridge;
    friend class TestAccess;

    std::unique_ptr<Impl> pImpl;
};

}

#endif
