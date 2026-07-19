#ifndef TESTAMENT_PARAMETERIZEDTEST_HPP
#define TESTAMENT_PARAMETERIZEDTEST_HPP

#include "Testament/TestOptions.hpp"
#include "Testament/detail/LocatedName.hpp"
#include "Testament/detail/ParameterizedTestDefinition.hpp"

#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace Testament {

template <typename... Args, typename Callable>
[[nodiscard]] auto ParameterizedTest(detail::LocatedName name, detail::CaseSet<Args...> cases,
                                     Callable&& body) {
    return detail::ParameterizedTestDefinition<std::decay_t<Callable>, Args...>{
        name.value(), name.location(), TestOptions{}, std::move(cases),
        std::forward<Callable>(body)
    };
}

template <typename... Args, typename Callable>
[[nodiscard]] auto ParameterizedTest(detail::LocatedName name, TestOptions options,
                                     detail::CaseSet<Args...> cases, Callable&& body) {
    return detail::ParameterizedTestDefinition<std::decay_t<Callable>, Args...>{
        name.value(), name.location(), std::move(options), std::move(cases),
        std::forward<Callable>(body)
    };
}

}

#endif
