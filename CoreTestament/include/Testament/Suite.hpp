#ifndef TESTAMENT_SUITE_HPP
#define TESTAMENT_SUITE_HPP

#include "Testament/SuiteOptions.hpp"
#include "Testament/SuiteRegistration.hpp"
#include "Testament/detail/Concepts.hpp"
#include "Testament/detail/RuntimeBridge.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace Testament::detail {

template <typename... Definitions>
std::vector<TestHandle> materializeDefinitions(Definitions&&... definitions) {
    std::vector<TestHandle> tests;
    auto append = [&tests](auto&& materialized) {
        tests.reserve(tests.size() + materialized.size());
        for (auto& test : materialized) tests.push_back(std::move(test));
    };
    (append(std::forward<Definitions>(definitions)), ...);
    return tests;
}

}

namespace Testament {

template <typename Fixture = void, typename... Definitions>
requires detail::FixtureSelection<Fixture>
    && (detail::DefinitionFor<Fixture, Definitions> && ...)
[[nodiscard]] SuiteRegistration Suite(std::string_view name, SuiteOptions options,
                                      Definitions&&... definitions) {
    try {
        auto tests = detail::materializeDefinitions(
            std::forward<Definitions>(definitions).template materialize<Fixture>()...
        );
        if constexpr (std::same_as<Fixture, void>) {
            return detail::RuntimeBridge::registerSuite(
                name, std::move(options), std::move(tests)
            );
        } else {
            return detail::RuntimeBridge::registerSuite(
                name, std::make_unique<Fixture>(), std::move(options), std::move(tests)
            );
        }
    } catch (const std::logic_error& error) {
        detail::RuntimeBridge::recordConfigurationError(
            std::string{name} + ": " + error.what()
        );
        return detail::RuntimeBridge::invalidRegistration();
    }
}

template <typename Fixture = void, typename... Definitions>
requires detail::FixtureSelection<Fixture>
    && (detail::DefinitionFor<Fixture, Definitions> && ...)
[[nodiscard]] SuiteRegistration Suite(std::string_view name, Definitions&&... definitions) {
    return Suite<Fixture>(name, SuiteOptions{}, std::forward<Definitions>(definitions)...);
}

}

#endif
