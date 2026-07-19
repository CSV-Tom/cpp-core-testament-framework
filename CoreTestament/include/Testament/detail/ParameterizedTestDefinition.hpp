#ifndef TESTAMENT_DETAIL_PARAMETERIZEDTESTDEFINITION_HPP
#define TESTAMENT_DETAIL_PARAMETERIZEDTESTDEFINITION_HPP

#include "Testament/LifecycleSuite.hpp"
#include "Testament/TestOptions.hpp"
#include "Testament/detail/CaseSet.hpp"
#include "Testament/detail/Concepts.hpp"
#include "Testament/detail/RuntimeBridge.hpp"

#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <source_location>
#include <string>
#include <tuple>
#include <typeindex>
#include <utility>
#include <vector>

namespace Testament::detail {

template <typename Callable, typename... Args>
class [[nodiscard("the parameterized test definition must be passed to Suite")]]
ParameterizedTestDefinition {
    struct SynchronizedCallable {
        explicit SynchronizedCallable(Callable callable_) : callable(std::move(callable_)) {}

        Callable callable;
        std::mutex mutex;
    };

public:
    ParameterizedTestDefinition(std::string name, std::source_location location,
                                TestOptions options,
                                CaseSet<Args...> cases, Callable callable)
        : name_(std::move(name)), location_(location), options_(std::move(options)),
          cases_(std::move(cases)), callable_(std::move(callable)) {}

    template <FixtureSelection Fixture>
    requires ParameterBodyCompatible<Fixture, Callable, Args...>::value
    std::vector<TestHandle> materialize() && {
        auto cases = std::move(cases_).release();
        std::shared_ptr<SynchronizedCallable> synchronizedCallable;
        if constexpr (!std::copy_constructible<Callable>) {
            synchronizedCallable = std::make_shared<SynchronizedCallable>(std::move(callable_));
        }
        std::vector<TestHandle> tests;
        tests.reserve(cases.size());
        for (auto& testCase : cases) {
            if (testCase.name().empty()) throw std::invalid_argument("Test case name cannot be empty");
            const auto testName = name_ + " / " + testCase.name();
            auto values = std::make_shared<const std::tuple<Args...>>(
                std::move(testCase).releaseValues()
            );
            if constexpr (std::same_as<Fixture, void>) {
                if constexpr (std::copy_constructible<Callable>) {
                    tests.push_back(RuntimeBridge::makeTest(
                        testName, options_, std::move_only_function<void()>{
                            [callable = callable_, values = std::move(values)] mutable {
                                std::apply(callable, *values);
                            }
                        }, location_
                    ));
                } else {
                    tests.push_back(RuntimeBridge::makeTest(
                        testName, options_, std::move_only_function<void()>{
                            [callable = synchronizedCallable, values = std::move(values)] {
                                std::scoped_lock lock(callable->mutex);
                                std::apply(callable->callable, *values);
                            }
                        }, location_
                    ));
                }
            } else if constexpr (std::copy_constructible<Callable>) {
                tests.push_back(RuntimeBridge::makeTest(
                    testName, options_, std::type_index(typeid(Fixture)),
                    std::move_only_function<void(LifecycleSuite&)>{
                        [callable = callable_, values = std::move(values)]
                        (LifecycleSuite& fixture) mutable {
                            auto* typedFixture = dynamic_cast<Fixture*>(&fixture);
                            if (!typedFixture) throw std::logic_error("Internal fixture type mismatch");
                            std::apply([&](const Args&... args) {
                                std::invoke(callable, *typedFixture, args...);
                            }, *values);
                        }
                    }, location_
                ));
            } else {
                tests.push_back(RuntimeBridge::makeTest(
                    testName, options_, std::type_index(typeid(Fixture)),
                    std::move_only_function<void(LifecycleSuite&)>{
                        [callable = synchronizedCallable, values = std::move(values)]
                        (LifecycleSuite& fixture) mutable {
                            auto* typedFixture = dynamic_cast<Fixture*>(&fixture);
                            if (!typedFixture) throw std::logic_error("Internal fixture type mismatch");
                            std::scoped_lock lock(callable->mutex);
                            std::apply([&](const Args&... args) {
                                std::invoke(callable->callable, *typedFixture, args...);
                            }, *values);
                        }
                    }, location_
                ));
            }
        }
        return tests;
    }

private:
    std::string name_;
    std::source_location location_;
    TestOptions options_;
    CaseSet<Args...> cases_;
    Callable callable_;
};

}

#endif
