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
        mCases.reserve(sizeof...(Cases));
        (mCases.emplace_back(std::forward<Cases>(cases)), ...);
    }

    std::vector<NamedTestCase<Args...>> release() && { return std::move(mCases); }

private:
    std::vector<NamedTestCase<Args...>> mCases;
};

}

#endif
