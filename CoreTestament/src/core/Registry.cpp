#include "Testament/Registry.hpp"

#include "Testament/Suite.hpp"
//#include "Testament/Test.hpp"

#include "Internal/InternalTest.hpp"
#include "TestImpl.hpp"

#include <vector>

namespace Testament {

std::shared_ptr<Test> Registry::registerTest(const std::string& name, FunctionVariant function) {
    return std::make_shared<InternalTest>(name, function);    
  /*  auto test = std::make_shared<Test>(name, function);
    tests.emplace_back(test);
    return test;*/
}


std::shared_ptr<Suite> Registry::registerSuite(std::shared_ptr<Suite> suite) {
    static std::vector<std::shared_ptr<Suite>> suites;
    suites.emplace_back(suite);
    return suite;
}

}
