#ifndef TESTAMENT_DETAIL_NAMEDTESTCASE_HPP
#define TESTAMENT_DETAIL_NAMEDTESTCASE_HPP

#include <string>
#include <tuple>
#include <utility>

namespace Testament::detail {

template <typename... Args>
class NamedTestCase {
public:
    NamedTestCase(std::string name, std::tuple<Args...> values)
        : mName(std::move(name)), mValues(std::move(values)) {}

    [[nodiscard]] const std::string& name() const noexcept { return mName; }
    std::tuple<Args...> releaseValues() && { return std::move(mValues); }

private:
    std::string mName;
    std::tuple<Args...> mValues;
};

}

#endif
