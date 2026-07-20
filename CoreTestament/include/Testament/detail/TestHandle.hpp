#ifndef TESTAMENT_DETAIL_TESTHANDLE_HPP
#define TESTAMENT_DETAIL_TESTHANDLE_HPP

#include "Testament/Export.hpp"

#include <memory>

namespace Testament::detail {

class TestInstance;
class TestHandleAccess;

class TESTAMENT_EXPORT TestHandle {
public:
    ~TestHandle();

    TestHandle(TestHandle&&) noexcept;
    TestHandle& operator=(TestHandle&&) noexcept;

    TestHandle(const TestHandle&) = delete;
    TestHandle& operator=(const TestHandle&) = delete;

private:
    class TESTAMENT_NO_EXPORT Impl;
    TESTAMENT_NO_EXPORT explicit TestHandle(std::unique_ptr<Impl> implementation);

    friend class RuntimeBridge;
    friend class TestHandleAccess;

    std::unique_ptr<Impl> pImpl;
};

}

#endif
