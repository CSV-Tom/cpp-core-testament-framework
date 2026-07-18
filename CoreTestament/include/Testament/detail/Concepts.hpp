#ifndef TESTAMENT_DETAIL_CONCEPTS_HPP
#define TESTAMENT_DETAIL_CONCEPTS_HPP

#include "Testament/LifecycleSuite.hpp"
#include "Testament/detail/TestHandle.hpp"

#include <concepts>
#include <type_traits>
#include <utility>
#include <vector>

namespace Testament::detail {

template <typename Fixture, typename Callable>
struct TestBodyCompatible : std::bool_constant<std::invocable<Callable&, Fixture&>> {};

template <typename Callable>
struct TestBodyCompatible<void, Callable> : std::bool_constant<std::invocable<Callable&>> {};

template <typename Fixture, typename Callable, typename... Args>
struct ParameterBodyCompatible
    : std::bool_constant<std::invocable<Callable&, Fixture&, const Args&...>> {};

template <typename Callable, typename... Args>
struct ParameterBodyCompatible<void, Callable, Args...>
    : std::bool_constant<std::invocable<Callable&, const Args&...>> {};

template <typename Fixture>
concept FixtureSelection = std::same_as<Fixture, void>
    || (std::derived_from<Fixture, LifecycleSuite> && std::default_initializable<Fixture>);

template <typename Fixture, typename Definition>
concept DefinitionFor = requires(Definition&& definition) {
    { std::forward<Definition>(definition).template materialize<Fixture>() }
        -> std::same_as<std::vector<TestHandle>>;
};

}

#endif
