#ifndef TESTFRAMEWORK_TESTAMENT_HPP
#define TESTFRAMEWORK_TESTAMENT_HPP

#include "Testament/Registry.hpp"
#include "Testament/Runner.hpp"
#include "Testament/Suite.hpp"
#include "Testament/Test.hpp"

#include <concepts>

namespace Testament {

template <typename Callable>
requires std::invocable<Callable>
std::shared_ptr<Test> makeTest(const std::string& name, Callable&& testFunction) {
    return Registry::registerTest(name, FunctionVariant{std::function<void()>(std::forward<Callable>(testFunction))});
}

template <typename... Tests>
requires (std::same_as<std::remove_cvref_t<Tests>, std::shared_ptr<Test>> && ...)
std::shared_ptr<Suite> makeSuite(const std::string& name, Tests&&... cases) {
    auto suite = std::make_shared<Suite>(name);
    (suite->addTest(std::forward<Tests>(cases)), ...);    
    return Registry::registerSuite(suite);
}

template <typename T, typename... Tests>
requires (std::derived_from<T, Suite> && (std::same_as<std::remove_cvref_t<Tests>, std::shared_ptr<Test>> && ...))
std::shared_ptr<T> makeSuite(const std::string& name, Tests&&... cases) {
    auto suite = std::make_shared<T>(name);
    (suite->addTest(std::forward<Tests>(cases)), ...);
    return Registry::registerSuite(suite);
}

}

#endif 
