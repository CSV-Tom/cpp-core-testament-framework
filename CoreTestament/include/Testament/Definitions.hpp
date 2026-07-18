#ifndef TESTAMENT_DEFINITIONS_HPP
#define TESTAMENT_DEFINITIONS_HPP

#include "Testament/LifecycleSuite.hpp"
#include "Testament/Options.hpp"
#include "Testament/SuiteRegistration.hpp"

#include <concepts>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <utility>
#include <vector>

namespace Testament {

namespace detail {

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

template <typename Callable>
class [[nodiscard("the test definition must be passed to Suite")]] TestDefinition {
public:
    TestDefinition(std::string name, TestOptions options, Callable callable)
        : name_(std::move(name)), options_(std::move(options)), callable_(std::move(callable)) {}

    template <FixtureSelection Fixture>
    requires TestBodyCompatible<Fixture, Callable>::value && std::copy_constructible<Callable>
    std::vector<TestHandle> materialize() && {
        std::vector<TestHandle> tests;
        tests.reserve(1);
        if constexpr (std::same_as<Fixture, void>) {
            tests.push_back(makeRuntimeTest(
                name_, std::move(options_), std::function<void()>{std::move(callable_)}
            ));
        } else {
            tests.push_back(makeRuntimeTest(
                name_, std::move(options_), std::type_index(typeid(Fixture)),
                std::function<void(LifecycleSuite&)>{
                    [callable = std::move(callable_)](LifecycleSuite& fixture) mutable {
                        auto* typedFixture = dynamic_cast<Fixture*>(&fixture);
                        if (!typedFixture) throw std::logic_error("Internal fixture type mismatch");
                        std::invoke(callable, *typedFixture);
                    }
                }
            ));
        }
        return tests;
    }

private:
    std::string name_;
    TestOptions options_;
    Callable callable_;
};

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

template <typename... Args>
class CaseSet {
public:
    template <typename... Cases>
    requires (std::same_as<std::remove_cvref_t<Cases>, NamedTestCase<Args...>> && ...)
    explicit CaseSet(Cases&&... cases) {
        cases_.reserve(sizeof...(Cases));
        (cases_.emplace_back(std::forward<Cases>(cases)), ...);
    }

    std::vector<NamedTestCase<Args...>> release() && { return std::move(cases_); }

private:
    std::vector<NamedTestCase<Args...>> cases_;
};

template <typename Callable, typename... Args>
class [[nodiscard("the parameterized test definition must be passed to Suite")]]
ParameterizedTestDefinition {
public:
    ParameterizedTestDefinition(std::string name, TestOptions options,
                                CaseSet<Args...> cases, Callable callable)
        : name_(std::move(name)), options_(std::move(options)), cases_(std::move(cases)),
          callable_(std::move(callable)) {}

    template <FixtureSelection Fixture>
    requires ParameterBodyCompatible<Fixture, Callable, Args...>::value
        && std::copy_constructible<Callable>
    std::vector<TestHandle> materialize() && {
        auto cases = std::move(cases_).release();
        std::vector<TestHandle> tests;
        tests.reserve(cases.size());
        for (auto& testCase : cases) {
            if (testCase.name().empty()) {
                throw std::invalid_argument("Test case name cannot be empty");
            }
            auto values = std::make_shared<const std::tuple<Args...>>(
                std::move(testCase).releaseValues()
            );
            const auto testName = name_ + " / " + testCase.name();
            auto callable = callable_;
            if constexpr (std::same_as<Fixture, void>) {
                tests.push_back(makeRuntimeTest(
                    testName, options_, std::function<void()>{
                        [callable = std::move(callable), values = std::move(values)]() mutable {
                            std::apply(callable, *values);
                        }
                    }
                ));
            } else {
                tests.push_back(makeRuntimeTest(
                    testName, options_, std::type_index(typeid(Fixture)),
                    std::function<void(LifecycleSuite&)>{
                        [callable = std::move(callable), values = std::move(values)]
                        (LifecycleSuite& fixture) mutable {
                            auto* typedFixture = dynamic_cast<Fixture*>(&fixture);
                            if (!typedFixture) throw std::logic_error("Internal fixture type mismatch");
                            std::apply([&](const Args&... args) {
                                std::invoke(callable, *typedFixture, args...);
                            }, *values);
                        }
                    }
                ));
            }
        }
        return tests;
    }

private:
    std::string name_;
    TestOptions options_;
    CaseSet<Args...> cases_;
    Callable callable_;
};

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

template <typename Callable>
[[nodiscard]] auto Test(std::string_view name, Callable&& body) {
    return detail::TestDefinition<std::decay_t<Callable>>{
        std::string{name}, TestOptions{}, std::forward<Callable>(body)
    };
}

template <typename Callable>
[[nodiscard]] auto Test(std::string_view name, TestOptions options, Callable&& body) {
    return detail::TestDefinition<std::decay_t<Callable>>{
        std::string{name}, std::move(options), std::forward<Callable>(body)
    };
}

template <typename... Args>
[[nodiscard]] auto TestCase(std::string_view name, Args&&... values) {
    return detail::NamedTestCase<std::decay_t<Args>...>{
        std::string{name}, std::tuple<std::decay_t<Args>...>{std::forward<Args>(values)...}
    };
}

template <typename... Args, typename... Remaining>
requires (std::same_as<std::remove_cvref_t<Remaining>, detail::NamedTestCase<Args...>> && ...)
[[nodiscard]] auto Cases(detail::NamedTestCase<Args...> first, Remaining&&... remaining) {
    return detail::CaseSet<Args...>{std::move(first), std::forward<Remaining>(remaining)...};
}

template <typename... Args, typename Callable>
[[nodiscard]] auto ParameterizedTest(std::string_view name, detail::CaseSet<Args...> cases,
                                     Callable&& body) {
    return detail::ParameterizedTestDefinition<std::decay_t<Callable>, Args...>{
        std::string{name}, TestOptions{}, std::move(cases), std::forward<Callable>(body)
    };
}

template <typename... Args, typename Callable>
[[nodiscard]] auto ParameterizedTest(std::string_view name, TestOptions options,
                                     detail::CaseSet<Args...> cases, Callable&& body) {
    return detail::ParameterizedTestDefinition<std::decay_t<Callable>, Args...>{
        std::string{name}, std::move(options), std::move(cases), std::forward<Callable>(body)
    };
}

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
            return detail::registerSuite(name, std::move(options), std::move(tests));
        } else {
            return detail::registerSuite(name, std::make_unique<Fixture>(),
                                         std::move(options), std::move(tests));
        }
    } catch (const std::logic_error& error) {
        detail::recordConfigurationError(std::string{name} + ": " + error.what());
        return detail::invalidSuiteRegistration();
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
