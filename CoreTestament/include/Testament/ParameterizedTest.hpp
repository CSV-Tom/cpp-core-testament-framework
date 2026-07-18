#ifndef TESTAMENT_PARAMETERIZEDTEST_HPP
#define TESTAMENT_PARAMETERIZEDTEST_HPP

#include "Testament/TestOptions.hpp"
#include "Testament/detail/ParameterizedTestDefinition.hpp"

#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace Testament {

template <typename... Args, typename Callable>
[[nodiscard]] auto ParameterizedTest(std::string_view name, detail::CaseSet<Args...> cases,
                                     Callable&& body) {
    return detail::ParameterizedTestDefinition<std::decay_t<Callable>, Args...>{
        std::string{name}, TestOptions{}, std::move(cases), std::forward<Callable>(body)
    };
}

template <typename... Args, typename Callable>
[[nodiscard]] auto ParameterizedTest(std::string_view name, TestOptions options,
                                     detail::CaseSet<Args...> cases, Callable&& body) {
    return detail::ParameterizedTestDefinition<std::decay_t<Callable>, Args...>{
        std::string{name}, std::move(options), std::move(cases), std::forward<Callable>(body)
    };
}

}

#endif
