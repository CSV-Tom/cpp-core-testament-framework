#ifndef TESTFRAMEWORK_TESTAMENT_TEST_HPP
#define TESTFRAMEWORK_TESTAMENT_TEST_HPP

#include <memory>
#include <string>

#include "Testament/FunctionVariant.hpp"

namespace Testament {

class Test {
public:

    static std::shared_ptr<Test> create(const std::string& name, FunctionVariant function);

protected:

    Test();
    virtual ~Test();

    Test(Test&&) noexcept;
    Test& operator=(Test&&) noexcept;

    Test(const Test&) = delete;
    Test& operator=(const Test&) = delete;
};

}

#endif
