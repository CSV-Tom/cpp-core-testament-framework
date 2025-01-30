#include "Testament/Test.hpp"

#include "Internal/InternalTest.hpp"

namespace Testament {

std::shared_ptr<Test> Test::create(const std::string& name, FunctionVariant function)
{
    return std::make_shared<InternalTest>(name, std::move(function));
}

Test::~Test() = default;
Test::Test(Test&&) noexcept = default;
Test& Test::operator=(Test&&) noexcept = default;
Test::Test() = default;

}