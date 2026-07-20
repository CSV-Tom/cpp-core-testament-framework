#pragma once

#include "Testament/detail/MoveOnlyFunction.hpp"

#include <variant>

namespace Testament {

class LifecycleSuite;

namespace detail {

using TestFunction = std::variant<
    MoveOnlyFunction<void()>,
    MoveOnlyFunction<void(LifecycleSuite&)>
>;

}
}
