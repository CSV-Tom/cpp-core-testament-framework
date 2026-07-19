#include "Testament/Testament.hpp"

#include <string>

int main() {
    bool nestedTrace{};
    bool poppedInner{};
    bool poppedAll{};
    {
        Testament::Trace outer{"outer context"};
        {
            Testament::Trace inner{"inner context"};
            try {
                Testament::Asserts::assertTrue(false);
            } catch (const Testament::AssertionFailure& failure) {
                const std::string message{failure.what()};
                nestedTrace = message.contains("trace:")
                    && message.contains("inner context")
                    && message.contains("outer context");
            }
        }
        try {
            Testament::Asserts::assertTrue(false);
        } catch (const Testament::AssertionFailure& failure) {
            const std::string message{failure.what()};
            poppedInner = message.contains("outer context")
                && !message.contains("inner context");
        }
    }
    try {
        Testament::Asserts::assertTrue(false);
    } catch (const Testament::AssertionFailure& failure) {
        poppedAll = !std::string{failure.what()}.contains("trace:");
    }

    return nestedTrace && poppedInner && poppedAll ? 0 : 1;
}
