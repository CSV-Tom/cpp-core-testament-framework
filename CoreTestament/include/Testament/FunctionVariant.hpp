#ifndef TESTAMENT_FUNCTIONVARIANT_HPP
#define TESTAMENT_FUNCTIONVARIANT_HPP

#include <variant>
#include <functional>

namespace Testament {

class Suite;

using FunctionVariant = std::variant<std::function<void()>, std::function<void(Suite&)>>;
  
}

#endif
