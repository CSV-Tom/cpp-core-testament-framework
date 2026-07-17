#include "Testament/Testament.hpp"

#include <iostream>
#include <string>

namespace Testament {


static auto suite = makeSuite("Parameterized Suite",
                              makeParameterizedTest("Division Tests",
[](int a, int b, int expected) {
    std::cout << "Executing Division Test with a = " << a << ", b = " << b << "\n";

    if (b == 0) {
        throw std::logic_error("Division by zero");
    }

    if (a / b != expected) {
        throw std::logic_error("Division test failed");
    }
},
std::vector<std::tuple<int, int, int>> {
    {10, 2, 5},
    {20, 4, 5},
    {30, 6, 5}
},
TestOptions{}.tag("parameterized")
                                                   )
                             );

}
