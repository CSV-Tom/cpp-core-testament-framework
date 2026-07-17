#ifndef TESTFRAMEWORK_TESTAMENT_HPP
#define TESTFRAMEWORK_TESTAMENT_HPP

#include "Testament/Asserts.hpp"
#include "Testament/Runner.hpp"
#include "Testament/Reporters.hpp"
#include "Testament/Suite.hpp"
#include "Testament/LifecycleSuite.hpp"
#include "Testament/Options.hpp"
#include "Testament/Test.hpp"

#include <concepts>
#include <functional>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

namespace Testament {


template <typename... Tests>
requires (std::same_as<std::remove_cvref_t<Tests>, Test> && ...)
Suite makeSuite(const std::string& name, Tests&&... cases) {
    std::vector<Test> tests;
    tests.reserve(sizeof...(Tests));
    (tests.emplace_back(std::forward<Tests>(cases)), ...);
    return detail::makeSuite(name, SuiteOptions{}, std::move(tests));
}

template <typename... Tests>
requires (std::same_as<std::remove_cvref_t<Tests>, Test> && ...)
Suite makeSuite(const std::string& name, SuiteOptions options, Tests&&... cases) {
    std::vector<Test> tests;
    tests.reserve(sizeof...(Tests));
    (tests.emplace_back(std::forward<Tests>(cases)), ...);
    return detail::makeSuite(name, std::move(options), std::move(tests));
}


template <typename T, typename... Tests>
requires (std::derived_from<T, LifecycleSuite> && (std::same_as<std::remove_cvref_t<Tests>, Test> && ...))
Suite makeSuite(const std::string& name, Tests&&... cases) {
    std::vector<Test> tests;
    tests.reserve(sizeof...(Tests));
    (tests.emplace_back(std::forward<Tests>(cases)), ...);
    return detail::makeSuite(name, std::make_unique<T>(), SuiteOptions{}, std::move(tests));
}

template <typename T, typename... Tests>
requires (std::derived_from<T, LifecycleSuite> && (std::same_as<std::remove_cvref_t<Tests>, Test> && ...))
Suite makeSuite(const std::string& name, SuiteOptions options, Tests&&... cases) {
    std::vector<Test> tests;
    tests.reserve(sizeof...(Tests));
    (tests.emplace_back(std::forward<Tests>(cases)), ...);
    return detail::makeSuite(name, std::make_unique<T>(), std::move(options), std::move(tests));
}


template <typename Callable>
requires std::invocable<Callable>
Test makeTest(const std::string& name, Callable&& testFunction) {
    return detail::makeTest(name, TestOptions{},
                            std::function<void()>(std::forward<Callable>(testFunction)));
}

template <typename Callable>
requires std::invocable<Callable>
Test makeTest(const std::string& name, TestOptions options, Callable&& testFunction) {
    return detail::makeTest(name, std::move(options),
                            std::function<void()>(std::forward<Callable>(testFunction)));
}

template <typename SuiteType, typename Callable>
requires std::derived_from<SuiteType, LifecycleSuite> && std::invocable<Callable, SuiteType&>
Test makeTest(const std::string& name, Callable&& testFunction) {
    return detail::makeTest(name, TestOptions{}, std::function<void(LifecycleSuite&)>{
        [testFunction = std::forward<Callable>(testFunction)](LifecycleSuite& suite) {
            auto* typedSuite = dynamic_cast<SuiteType*>(&suite);
            if (!typedSuite) {
                throw std::logic_error("Test fixture type does not match suite fixture type");
            }
            std::invoke(testFunction, *typedSuite);
        }
    });
}

template <typename SuiteType, typename Callable>
requires std::derived_from<SuiteType, LifecycleSuite> && std::invocable<Callable, SuiteType&>
Test makeTest(const std::string& name, TestOptions options, Callable&& testFunction) {
    return detail::makeTest(name, std::move(options), std::function<void(LifecycleSuite&)>{
        [testFunction = std::forward<Callable>(testFunction)](LifecycleSuite& suite) {
            auto* typedSuite = dynamic_cast<SuiteType*>(&suite);
            if (!typedSuite) {
                throw std::logic_error("Test fixture type does not match suite fixture type");
            }
            std::invoke(testFunction, *typedSuite);
        }
    });
}

template <typename Callable, typename... Args>
requires std::invocable<Callable, Args...>
Test makeParameterizedTest(const std::string& name, Callable&& testFunction, std::vector<std::tuple<Args...>> parameters) {
    return detail::makeTest(name, TestOptions{}, std::function<void()>{[testFunction = std::forward<Callable>(testFunction), parameters]() {
        for (const auto& params : parameters) {
            std::apply(testFunction, params);
        }
    }});
}

template <typename Callable, typename... Args>
requires std::invocable<Callable, Args...>
Test makeParameterizedTest(const std::string& name, TestOptions options, Callable&& testFunction,
                           std::vector<std::tuple<Args...>> parameters) {
    return detail::makeTest(name, std::move(options), std::function<void()>{
        [testFunction = std::forward<Callable>(testFunction), parameters]() {
            for (const auto& params : parameters) {
                std::apply(testFunction, params);
            }
        }
    });
}

template <typename SuiteType, typename Callable, typename... Args>
requires std::derived_from<SuiteType, LifecycleSuite> && std::invocable<Callable, SuiteType&, Args...>
Test makeParameterizedTest(const std::string& name, Callable&& testFunction, std::vector<std::tuple<Args...>> parameters) {
    return detail::makeTest(name, TestOptions{}, std::function<void(LifecycleSuite&)>{
        [testFunction = std::forward<Callable>(testFunction), parameters](LifecycleSuite& suite) {
            auto* typedSuite = dynamic_cast<SuiteType*>(&suite);
            if (!typedSuite) {
                throw std::logic_error("Test fixture type does not match suite fixture type");
            }
            for (const auto& params : parameters) {
                std::apply([typedSuite, &testFunction](Args... args) {
                    std::invoke(testFunction, *typedSuite, args...);
                }, params);
            }
        }
    });
}

template <typename SuiteType, typename Callable, typename... Args>
requires std::derived_from<SuiteType, LifecycleSuite> && std::invocable<Callable, SuiteType&, Args...>
Test makeParameterizedTest(const std::string& name, TestOptions options, Callable&& testFunction,
                           std::vector<std::tuple<Args...>> parameters) {
    return detail::makeTest(name, std::move(options), std::function<void(LifecycleSuite&)>{
        [testFunction = std::forward<Callable>(testFunction), parameters](LifecycleSuite& suite) {
            auto* typedSuite = dynamic_cast<SuiteType*>(&suite);
            if (!typedSuite) {
                throw std::logic_error("Test fixture type does not match suite fixture type");
            }
            for (const auto& params : parameters) {
                std::apply([typedSuite, &testFunction](Args... args) {
                    std::invoke(testFunction, *typedSuite, args...);
                }, params);
            }
        }
    });
}

}

#endif
