#ifndef TESTFRAMEWORK_TESTAMENT_HPP
#define TESTFRAMEWORK_TESTAMENT_HPP

#include "Testament/Asserts.hpp"
#include "Testament/Runner.hpp"
#include "Testament/Reporters.hpp"
#include "Testament/Suite.hpp"
#include "Testament/LifecycleSuite.hpp"
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
std::shared_ptr<Suite> makeSuite(const std::string& name, Tests&&... cases) {
    auto suite = Suite::create(name);
    (suite->addTest(std::forward<Tests>(cases)), ...);
    return suite;
}


template <typename T, typename... Tests>
requires (std::derived_from<T, LifecycleSuite> && (std::same_as<std::remove_cvref_t<Tests>, Test> && ...))
std::shared_ptr<Suite> makeSuite(const std::string& name, Tests&&... cases) {
    auto suite = LifecycleSuite::create(name, std::make_shared<T>());
    (suite->addTest(std::forward<Tests>(cases)), ...);
    return suite;
}


template <typename Callable>
requires std::invocable<Callable>
Test makeTest(const std::string& name, Callable&& testFunction) {
    return detail::makeTest(name, std::function<void()>(std::forward<Callable>(testFunction)));
}

template <typename SuiteType, typename Callable>
requires std::derived_from<SuiteType, Suite> && std::invocable<Callable, SuiteType&>
Test makeTest(const std::string& name, Callable&& testFunction) {
    return detail::makeTest(name, std::function<void(Suite&)>{
        [testFunction = std::forward<Callable>(testFunction)](Suite& suite) {
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
    return detail::makeTest(name, std::function<void()>{[testFunction = std::forward<Callable>(testFunction), parameters]() {
        for (const auto& params : parameters) {
            std::apply(testFunction, params);
        }
    }});
}

template <typename SuiteType, typename Callable, typename... Args>
requires std::derived_from<SuiteType, Suite> && std::invocable<Callable, SuiteType&, Args...>
Test makeParameterizedTest(const std::string& name, Callable&& testFunction, std::vector<std::tuple<Args...>> parameters) {
    return detail::makeTest(name, std::function<void(Suite&)>{
        [testFunction = std::forward<Callable>(testFunction), parameters](Suite& suite) {
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
