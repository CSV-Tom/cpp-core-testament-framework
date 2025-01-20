#include <concepts>
#include <memory>

#include "TestCase.hpp"

// Factory-Funktion für TestCases
template <typename Callable>
requires std::invocable<Callable>
std::shared_ptr<TestCase> makeTest(const std::string& name, Callable&& testFunction) {
    return std::make_shared<TestCase>(name, FunctionVariant{std::function<void()>(std::forward<Callable>(testFunction))});
}

template <typename Suite, typename Callable>
requires std::invocable<Callable, Suite&>
std::shared_ptr<TestCase> makeTest(const std::string& name, Callable&& testFunction) {
    return std::make_shared<TestCase>(name, FunctionVariant{
        [testFunction = std::forward<Callable>(testFunction)](TestSuite& suite) {
            testFunction(static_cast<Suite&>(suite)); // Explizite Umwandlung zu Suite
        }
    });
}


template <typename Callable, typename... Args>
requires std::invocable<Callable, Args...>
std::shared_ptr<TestCase> makeParameterizedTest(const std::string& name, Callable&& testFunction, std::vector<std::tuple<Args...>> parameters) {
    return std::make_shared<TestCase>(name, FunctionVariant{[testFunction = std::forward<Callable>(testFunction), parameters]() {
        for (const auto& params : parameters) {
            std::apply(testFunction, params); // Test mit Parametern ausführen
        }
    }});
}

template <typename Suite, typename Callable, typename... Args>
requires std::invocable<Callable, Suite&, Args...>
std::shared_ptr<TestCase> makeParameterizedTest(const std::string& name, Callable&& testFunction, std::vector<std::tuple<Args...>> parameters) {
    return std::make_shared<TestCase>(name, FunctionVariant{
        [testFunction = std::forward<Callable>(testFunction), parameters](TestSuite& baseSuite) {
            auto& suite = static_cast<Suite&>(baseSuite); // Typumwandlung zu Suite
            for (const auto& params : parameters) {
                std::apply([&suite, &testFunction](Args... args) {
                    testFunction(suite, args...); // Parameter entpacken und Funktion ausführen
                }, params);
            }
        }
    });
}

// Factory-Funktion für TestSuites
template <typename... TestCases>
requires (std::same_as<std::remove_cvref_t<TestCases>, std::shared_ptr<TestCase>> && ...)
std::shared_ptr<TestSuite> makeSuite(const std::string& name, TestCases&&... cases) {
    auto suite = std::make_shared<TestSuite>(name);
    (suite->addTest(std::forward<TestCases>(cases)), ...);
    TestRegistry::instance().addSuite(suite);
    return suite;
}

template <typename T, typename... TestCases>
requires (std::derived_from<T, TestSuite> && (std::same_as<std::remove_cvref_t<TestCases>, std::shared_ptr<TestCase>> && ...))
std::shared_ptr<T> makeSuite(const std::string& name, TestCases&&... cases) {
    auto suite = std::make_shared<T>(name);
    (suite->addTest(std::forward<TestCases>(cases)), ...);
    TestRegistry::instance().addSuite(suite);
    return suite;
}