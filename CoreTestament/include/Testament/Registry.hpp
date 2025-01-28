#ifndef TESTFRAMEWORK_TESTAMENT_REGISTRY_HPP
#define TESTFRAMEWORK_TESTAMENT_REGISTRY_HPP

#include <string>
#include <memory>

#include "Testament/FunctionVariant.hpp"

namespace Testament {

class Suite;
class Test;

class Registry {
public:
    static std::shared_ptr<Test> registerTest(const std::string& name, FunctionVariant function);
    static std::shared_ptr<Suite> registerSuite(std::shared_ptr<Suite> suite);
};
  
}

#endif
