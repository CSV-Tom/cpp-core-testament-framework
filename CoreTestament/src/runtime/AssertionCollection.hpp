#pragma once

#include <exception>

namespace Testament::Asserts::detail {

void beginAssertionCollection();
[[nodiscard]] std::exception_ptr finishAssertionCollection(
    std::exception_ptr terminalFailure = {}
);

}
