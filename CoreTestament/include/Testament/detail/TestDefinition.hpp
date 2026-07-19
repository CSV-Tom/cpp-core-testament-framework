#ifndef TESTAMENT_DETAIL_TESTDEFINITION_HPP
#define TESTAMENT_DETAIL_TESTDEFINITION_HPP

#include "Testament/LifecycleSuite.hpp"
#include "Testament/TestOptions.hpp"
#include "Testament/detail/Concepts.hpp"
#include "Testament/detail/RuntimeBridge.hpp"

#include <functional>
#include <stdexcept>
#include <source_location>
#include <string>
#include <typeindex>
#include <utility>
#include <vector>

namespace Testament::detail {

template <typename Callable>
class [[nodiscard("the test definition must be passed to Suite")]] TestDefinition {
public:
    TestDefinition(std::string name, std::source_location location, TestOptions options,
                   Callable callable)
        : name_(std::move(name)), location_(location), options_(std::move(options)),
          callable_(std::move(callable)) {}

    template <FixtureSelection Fixture>
    requires TestBodyCompatible<Fixture, Callable>::value
    std::vector<TestHandle> materialize() && {
        std::vector<TestHandle> tests;
        tests.reserve(1);
        if constexpr (std::same_as<Fixture, void>) {
            tests.push_back(RuntimeBridge::makeTest(
                name_, std::move(options_),
                std::move_only_function<void()>{std::move(callable_)}, location_
            ));
        } else {
            tests.push_back(RuntimeBridge::makeTest(
                name_, std::move(options_), std::type_index(typeid(Fixture)),
                std::move_only_function<void(LifecycleSuite&)>{
                    [callable = std::move(callable_)](LifecycleSuite& fixture) mutable {
                        auto* typedFixture = dynamic_cast<Fixture*>(&fixture);
                        if (!typedFixture) throw std::logic_error("Internal fixture type mismatch");
                        std::invoke(callable, *typedFixture);
                    }
                }, location_
            ));
        }
        return tests;
    }

private:
    std::string name_;
    std::source_location location_;
    TestOptions options_;
    Callable callable_;
};

}

#endif
