#ifndef TESTFRAMEWORK_TESTAMENT_TEST_HPP
#define TESTFRAMEWORK_TESTAMENT_TEST_HPP

#include <memory>
#include <string>

#include "Testament/FunctionVariant.hpp"

namespace Testament {

class Test {
public:
    Test(const std::string& name, FunctionVariant function);
    virtual ~Test();
protected:
    class Impl;
    std::unique_ptr<Impl> pImpl;    
};

}

#endif
