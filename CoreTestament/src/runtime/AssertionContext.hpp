#pragma once

#include <exception>

namespace Testament::Asserts::detail {

void beginAssertionContext();
[[nodiscard]] std::exception_ptr finishAssertionContext(
    std::exception_ptr terminalFailure = {}
);

}
