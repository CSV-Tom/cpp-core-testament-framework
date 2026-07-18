#ifndef TESTAMENT_TEST_HPP
#define TESTAMENT_TEST_HPP

#include "Testament/TestOptions.hpp"
#include "Testament/detail/TestDefinition.hpp"

#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace Testament {

template <typename Callable>
[[nodiscard]] auto Test(std::string_view name, Callable&& body) {
    return detail::TestDefinition<std::decay_t<Callable>>{
        std::string{name}, TestOptions{}, std::forward<Callable>(body)
    };
}

template <typename Callable>
[[nodiscard]] auto Test(std::string_view name, TestOptions options, Callable&& body) {
    return detail::TestDefinition<std::decay_t<Callable>>{
        std::string{name}, std::move(options), std::forward<Callable>(body)
    };
}

}

#endif
