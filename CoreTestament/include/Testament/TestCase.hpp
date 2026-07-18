#ifndef TESTAMENT_TESTCASE_HPP
#define TESTAMENT_TESTCASE_HPP

#include "Testament/detail/CaseSet.hpp"

#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace Testament {

template <typename... Args>
[[nodiscard]] auto TestCase(std::string_view name, Args&&... values) {
    return detail::NamedTestCase<std::decay_t<Args>...>{
        std::string{name}, std::tuple<std::decay_t<Args>...>{std::forward<Args>(values)...}
    };
}

template <typename... Args, typename... Remaining>
requires (std::same_as<std::remove_cvref_t<Remaining>, detail::NamedTestCase<Args...>> && ...)
[[nodiscard]] auto Cases(detail::NamedTestCase<Args...> first, Remaining&&... remaining) {
    return detail::CaseSet<Args...>{std::move(first), std::forward<Remaining>(remaining)...};
}

}

#endif
