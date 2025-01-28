#include "Testament/Test.hpp"

#include "../TestImpl.hpp"

namespace Testament {

class InternalTest : public Test {
public:
    using Test::Test;

    void xx() {
        std::cout << pImpl->name << std::endl;
    
    }
};

}