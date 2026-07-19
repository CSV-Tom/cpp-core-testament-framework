#ifndef TESTAMENT_INTERNAL_ASSERTIONCOLLECTION_HPP
#define TESTAMENT_INTERNAL_ASSERTIONCOLLECTION_HPP

#include <exception>

namespace Testament::Asserts::detail {

void beginAssertionCollection();
[[nodiscard]] std::exception_ptr finishAssertionCollection(
    std::exception_ptr terminalFailure = {}
);

}

#endif
