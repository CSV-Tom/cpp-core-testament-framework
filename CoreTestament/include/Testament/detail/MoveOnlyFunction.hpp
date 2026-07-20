#ifndef TESTAMENT_DETAIL_MOVEONLYFUNCTION_HPP
#define TESTAMENT_DETAIL_MOVEONLYFUNCTION_HPP

#include <concepts>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

namespace Testament::detail {

#if !defined(TESTAMENT_FORCE_MOVE_ONLY_FUNCTION_FALLBACK) \
    && defined(__cpp_lib_move_only_function) && __cpp_lib_move_only_function >= 202110L

template <typename Signature>
using MoveOnlyFunction = std::move_only_function<Signature>;

#else

template <typename Signature>
class MoveOnlyFunction;

template <typename Return, typename... Arguments>
class MoveOnlyFunction<Return(Arguments...)> {
public:
    MoveOnlyFunction() noexcept = default;

    template <typename Callable>
    requires (!std::same_as<std::remove_cvref_t<Callable>, MoveOnlyFunction>)
        && std::invocable<Callable&, Arguments...>
    MoveOnlyFunction(Callable&& callable)
        : mTarget(std::make_unique<Model<std::remove_cvref_t<Callable>>>(
              std::forward<Callable>(callable)
          )) {}

    MoveOnlyFunction(MoveOnlyFunction&&) noexcept = default;
    MoveOnlyFunction& operator=(MoveOnlyFunction&&) noexcept = default;

    MoveOnlyFunction(const MoveOnlyFunction&) = delete;
    MoveOnlyFunction& operator=(const MoveOnlyFunction&) = delete;

    [[nodiscard]] explicit operator bool() const noexcept {
        return static_cast<bool>(mTarget);
    }

    Return operator()(Arguments... arguments) {
        if (!mTarget) throw std::bad_function_call{};
        if constexpr (std::is_void_v<Return>) {
            mTarget->invoke(std::forward<Arguments>(arguments)...);
        } else {
            return mTarget->invoke(std::forward<Arguments>(arguments)...);
        }
    }

private:
    class Target {
    public:
        virtual ~Target() = default;
        virtual Return invoke(Arguments... arguments) = 0;
    };

    template <typename Callable>
    class Model final : public Target {
    public:
        explicit Model(Callable callable) : mCallable(std::move(callable)) {}

        Return invoke(Arguments... arguments) override {
            if constexpr (std::is_void_v<Return>) {
                std::invoke(mCallable, std::forward<Arguments>(arguments)...);
            } else {
                return std::invoke(mCallable, std::forward<Arguments>(arguments)...);
            }
        }

    private:
        Callable mCallable;
    };

    std::unique_ptr<Target> mTarget;
};

#endif

}

#endif
