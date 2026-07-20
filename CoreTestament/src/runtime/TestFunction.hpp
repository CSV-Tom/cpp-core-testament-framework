#pragma once

#include <functional>
#include <variant>

namespace Testament {

class LifecycleSuite;

namespace detail {

using TestFunction = std::variant<
    std::move_only_function<void()>,
    std::move_only_function<void(LifecycleSuite&)>
>;

}
}
