#include "Testament/Testament.hpp"

#include <stdexcept>

namespace {

class EmptyNameLifecycleSuite : public Testament::LifecycleSuite {};

}

int main() {
    try {
        static_cast<void>(Testament::makeSuite<EmptyNameLifecycleSuite>(""));
    } catch (const std::logic_error&) {
        return 0;
    }

    return 1;
}
