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
        : name_(std::move(name)), values_(std::move(values)) {}

    [[nodiscard]] const std::string& name() const noexcept { return name_; }
    std::tuple<Args...> releaseValues() && { return std::move(values_); }

private:
    std::string name_;
    std::tuple<Args...> values_;
};

}

#endif
