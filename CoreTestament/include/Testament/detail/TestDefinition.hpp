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
        : mName(std::move(name)), mLocation(location), mOptions(std::move(options)),
          mCallable(std::move(callable)) {}

    template <FixtureSelection Fixture>
    requires TestBodyCompatible<Fixture, Callable>::value
    std::vector<TestHandle> materialize() && {
        std::vector<TestHandle> tests;
        tests.reserve(1);
        if constexpr (std::same_as<Fixture, void>) {
            tests.push_back(RuntimeBridge::makeTest(
                mName, std::move(mOptions),
                std::move_only_function<void()>{std::move(mCallable)}, mLocation
            ));
        } else {
            tests.push_back(RuntimeBridge::makeTest(
                mName, std::move(mOptions), std::type_index(typeid(Fixture)),
                std::move_only_function<void(LifecycleSuite&)>{
                    [callable = std::move(mCallable)](LifecycleSuite& fixture) mutable {
                        auto* typedFixture = dynamic_cast<Fixture*>(&fixture);
                        if (!typedFixture) throw std::logic_error("Internal fixture type mismatch");
                        std::invoke(callable, *typedFixture);
                    }
                }, mLocation
            ));
        }
        return tests;
    }

private:
    std::string mName;
    std::source_location mLocation;
    TestOptions mOptions;
    Callable mCallable;
};

}

#endif
