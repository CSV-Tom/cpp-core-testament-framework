#include <Testament/Testament.hpp>

#include <expected>

int main() {
    const std::expected<int, int> cxx23Value{23};
    auto test = Testament::makeTest("installed consumer test", [] {});
    auto suite = Testament::makeSuite("installed consumer suite", test);

    return cxx23Value.value() == 23 && suite ? 0 : 1;
}
