#ifndef TESTAMENT_DETAIL_PARAMETERIZEDTESTDEFINITION_HPP
#define TESTAMENT_DETAIL_PARAMETERIZEDTESTDEFINITION_HPP

#include "Testament/LifecycleSuite.hpp"
#include "Testament/TestOptions.hpp"
#include "Testament/detail/CaseSet.hpp"
#include "Testament/detail/Concepts.hpp"
#include "Testament/detail/RuntimeBridge.hpp"

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <typeindex>
#include <utility>
#include <vector>

namespace Testament::detail {

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
            if (testCase.name().empty()) throw std::invalid_argument("Test case name cannot be empty");
            const auto testName = name_ + " / " + testCase.name();
            auto values = std::make_shared<const std::tuple<Args...>>(
                std::move(testCase).releaseValues()
            );
            auto callable = callable_;
            if constexpr (std::same_as<Fixture, void>) {
                tests.push_back(RuntimeBridge::makeTest(
                    testName, options_, std::function<void()>{
                        [callable = std::move(callable), values = std::move(values)]() mutable {
                            std::apply(callable, *values);
                        }
                    }
                ));
            } else {
                tests.push_back(RuntimeBridge::makeTest(
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

}

#endif
