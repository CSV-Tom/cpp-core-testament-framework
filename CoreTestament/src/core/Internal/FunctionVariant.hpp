#ifndef TESTAMENT_INTERNAL_FUNCTIONVARIANT_HPP
#define TESTAMENT_INTERNAL_FUNCTIONVARIANT_HPP

#include <functional>
#include <variant>

namespace Testament {

class Suite;

using FunctionVariant = std::variant<std::function<void()>, std::function<void(Suite&)>>;

}

#endif
