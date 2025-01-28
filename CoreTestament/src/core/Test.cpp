#include "TestImpl.hpp"

namespace Testament {

Test::Test(const std::string& name, FunctionVariant function) : pImpl(std::make_unique<Impl>(name, std::move(function))) {}
Test::~Test() = default;

}