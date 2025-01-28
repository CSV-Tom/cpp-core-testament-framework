#include "Testament/Test.hpp"

namespace Testament {

class Test::Impl {
public:
    Impl(const std::string& name, FunctionVariant function)  : name(name), function(std::move(function)) {}

private:
    std::string name;
    FunctionVariant function;
};

}