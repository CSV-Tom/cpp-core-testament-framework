#ifndef TESTFRAMEWORK_TESTAMENT_HPP
#define TESTFRAMEWORK_TESTAMENT_HPP

#include "Testament/Asserts.hpp"
#include "Testament/LifecycleSuite.hpp"
#include "Testament/Options.hpp"
#include "Testament/Reporters.hpp"
#include "Testament/Runner.hpp"
#include "Testament/Suite.hpp"
#include "Testament/Test.hpp"

#include <concepts>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace Testament {

namespace detail {

template <typename... Types>
concept TestHandles = (std::same_as<std::remove_cvref_t<Types>, Test> && ...);

template <typename Type>
concept FixtureSelection = std::same_as<Type, void> || std::derived_from<Type, LifecycleSuite>;

template <typename SuiteType, typename Callable, typename... Args>
concept CompatibleCallable = FixtureSelection<SuiteType>
    && ((std::same_as<SuiteType, void> && std::invocable<Callable, Args...>)
        || std::invocable<Callable, SuiteType&, Args...>);

template <TestHandles... Tests>
std::vector<Test> collectTests(Tests&&... cases) {
    std::vector<Test> result;
    result.reserve(sizeof...(Tests));
    (result.emplace_back(std::forward<Tests>(cases)), ...);
    return result;
}

template <std::derived_from<LifecycleSuite> SuiteType>
SuiteType& checkedFixture(LifecycleSuite& fixture) {
    if (auto* typed = dynamic_cast<SuiteType*>(&fixture)) return *typed;
    throw std::logic_error("Test fixture type does not match suite fixture type");
}

}

template <typename SuiteType = void, typename... Tests>
requires detail::FixtureSelection<SuiteType> && detail::TestHandles<Tests...>
[[nodiscard]] Suite makeSuite(std::string_view name, SuiteOptions options, Tests&&... cases) {
    auto tests = detail::collectTests(std::forward<Tests>(cases)...);
    if constexpr (std::same_as<SuiteType, void>) {
        return detail::makeSuite(name, std::move(options), std::move(tests));
    } else {
        return detail::makeSuite(name, std::make_unique<SuiteType>(), std::move(options), std::move(tests));
    }
}

template <typename SuiteType = void, typename... Tests>
requires detail::FixtureSelection<SuiteType> && detail::TestHandles<Tests...>
[[nodiscard]] Suite makeSuite(std::string_view name, Tests&&... cases) {
    return makeSuite<SuiteType>(name, SuiteOptions{}, std::forward<Tests>(cases)...);
}

template <typename SuiteType = void, typename Callable>
requires detail::CompatibleCallable<SuiteType, Callable>
[[nodiscard]] Test makeTest(std::string_view name, Callable&& function, TestOptions options = {}) {
    if constexpr (std::same_as<SuiteType, void>) {
        return detail::makeTest(name, std::move(options), std::function<void()>{std::forward<Callable>(function)});
    } else {
        return detail::makeTest(name, std::move(options), std::function<void(LifecycleSuite&)>{
            [function = std::forward<Callable>(function)](LifecycleSuite& fixture) mutable {
                std::invoke(function, detail::checkedFixture<SuiteType>(fixture));
            }
        });
    }
}

template <typename SuiteType = void, typename Callable, typename... Args>
requires detail::CompatibleCallable<SuiteType, Callable, Args...>
[[nodiscard]] Test makeParameterizedTest(std::string_view name, Callable&& function,
                                         std::vector<std::tuple<Args...>> parameters, TestOptions options = {}) {
    if constexpr (std::same_as<SuiteType, void>) {
        return makeTest(name, [function = std::forward<Callable>(function), parameters = std::move(parameters)]() mutable {
            for (const auto& values : parameters) std::apply(function, values);
        }, std::move(options));
    } else {
        return makeTest<SuiteType>(name, [function = std::forward<Callable>(function),
                                         parameters = std::move(parameters)](SuiteType& fixture) mutable {
            for (const auto& values : parameters) {
                std::apply([&](Args... args) { std::invoke(function, fixture, args...); }, values);
            }
        }, std::move(options));
    }
}

}

#endif
