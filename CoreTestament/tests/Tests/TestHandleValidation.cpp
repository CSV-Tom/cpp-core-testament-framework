#include "Testament/Testament.hpp"

#include <stdexcept>
#include <utility>

int main() {
    auto source = Testament::makeTest("test", [] {});
    if (!source) {
        return 1;
    }

    auto destination = std::move(source);
    if (source || !destination) {
        return 1;
    }

    try {
        auto suite = Testament::makeSuite("suite", std::move(source));
        static_cast<void>(suite);
    } catch (const std::invalid_argument&) {
        return destination ? 0 : 1;
    } catch (...) {
        return 1;
    }

    return 1;
}
