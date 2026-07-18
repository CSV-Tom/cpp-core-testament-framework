#ifndef TESTAMENT_INTERNAL_FUNCTIONVARIANT_HPP
#define TESTAMENT_INTERNAL_FUNCTIONVARIANT_HPP

#include <functional>
#include <variant>

namespace Testament {

class LifecycleSuite;

using FunctionVariant = std::variant<
    std::move_only_function<void()>,
    std::move_only_function<void(LifecycleSuite&)>
>;

}

#endif
