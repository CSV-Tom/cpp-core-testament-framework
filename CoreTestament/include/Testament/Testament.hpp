#ifndef TESTFRAMEWORK_TESTAMENT_HPP
#define TESTFRAMEWORK_TESTAMENT_HPP

#include "Testament/Asserts.hpp"
#include "Testament/Runner.hpp"
#include "Testament/Suite.hpp"
#include "Testament/LifecycleSuite.hpp"
#include "Testament/Test.hpp"

#include <concepts>

namespace Testament {


template <typename... Tests>
requires (std::same_as<std::remove_cvref_t<Tests>, std::shared_ptr<Test>> && ...)
std::shared_ptr<Suite> makeSuite(const std::string& name, Tests&&... cases) {
    auto suite = Suite::create(name);
    (suite->addTest(std::forward<Tests>(cases)), ...);
    return suite;
}


template <typename T, typename... Tests>
requires (std::derived_from<T, LifecycleSuite> && (std::same_as<std::remove_cvref_t<Tests>, std::shared_ptr<Test>> && ...))
std::shared_ptr<T> makeSuite(const std::string& name, Tests&&... cases) {
    auto suite = LifecycleSuite::create(name, std::make_shared<T>());
    (suite->addTest(std::forward<Tests>(cases)), ...);
    return std::static_pointer_cast<T>(suite);
}


template <typename Callable>
requires std::invocable<Callable>
std::shared_ptr<Test> makeTest(const std::string& name, Callable&& testFunction) {
    return Test::create(name, FunctionVariant{std::function<void()>(std::forward<Callable>(testFunction))});
}

template <typename SuiteType, typename Callable>
requires std::derived_from<SuiteType, Suite> && std::invocable<Callable, SuiteType&>
std::shared_ptr<Test> makeTest(const std::string& name, Callable&& testFunction) {
    return Test::create(name, FunctionVariant{
        [testFunction = std::forward<Callable>(testFunction)](Suite& suite) {
            testFunction(static_cast<SuiteType&>(suite)); // Explizite Umwandlung zu Suite
        }
    });
}

template <typename Callable, typename... Args>
requires std::invocable<Callable, Args...>
std::shared_ptr<Test> makeParameterizedTest(const std::string& name, Callable&& testFunction, std::vector<std::tuple<Args...>> parameters) {
    return Test::create(name, FunctionVariant{[testFunction = std::forward<Callable>(testFunction), parameters]() {
        for (const auto& params : parameters) {
            std::apply(testFunction, params);
        }
    }});
}

template <typename Suite, typename Callable, typename... Args>
requires std::invocable<Callable, Suite&, Args...>
std::shared_ptr<Test> makeParameterizedTest(const std::string& name, Callable&& testFunction, std::vector<std::tuple<Args...>> parameters) {
    return Test::create(name, FunctionVariant{
        [testFunction = std::forward<Callable>(testFunction), parameters](Suite& suite) {
            for (const auto& params : parameters) {
                std::apply([&suite, &testFunction](Args... args) {
                    testFunction(suite, args...); // Parameter entpacken und Funktion ausführen
                }, params);
            }
        }
    });
}

}

#endif
