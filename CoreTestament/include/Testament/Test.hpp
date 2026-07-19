#ifndef TESTAMENT_TEST_HPP
#define TESTAMENT_TEST_HPP

#include "Testament/TestOptions.hpp"
#include "Testament/detail/LocatedName.hpp"
#include "Testament/detail/TestDefinition.hpp"

#include <type_traits>
#include <utility>

namespace Testament {

template <typename Callable>
[[nodiscard]] auto Test(detail::LocatedName name, Callable&& body) {
    return detail::TestDefinition<std::decay_t<Callable>>{
        name.value(), name.location(), TestOptions{}, std::forward<Callable>(body)
    };
}

template <typename Callable>
[[nodiscard]] auto Test(detail::LocatedName name, TestOptions options, Callable&& body) {
    return detail::TestDefinition<std::decay_t<Callable>>{
        name.value(), name.location(), std::move(options), std::forward<Callable>(body)
    };
}

}

#endif
