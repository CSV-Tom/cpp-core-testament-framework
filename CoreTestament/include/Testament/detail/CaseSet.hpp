#ifndef TESTAMENT_DETAIL_CASESET_HPP
#define TESTAMENT_DETAIL_CASESET_HPP

#include "Testament/detail/NamedTestCase.hpp"

#include <concepts>
#include <type_traits>
#include <utility>
#include <vector>

namespace Testament::detail {

template <typename... Args>
class CaseSet {
public:
    template <typename... Cases>
    requires (std::same_as<std::remove_cvref_t<Cases>, NamedTestCase<Args...>> && ...)
    explicit CaseSet(Cases&&... cases) {
        cases_.reserve(sizeof...(Cases));
        (cases_.emplace_back(std::forward<Cases>(cases)), ...);
    }

    std::vector<NamedTestCase<Args...>> release() && { return std::move(cases_); }

private:
    std::vector<NamedTestCase<Args...>> cases_;
};

}

#endif
