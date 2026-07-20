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
                   Callable callable);

    template <FixtureSelection Fixture>
    requires TestBodyCompatible<Fixture, Callable>::value
    std::vector<TestHandle> materialize() &&;

private:
    std::string mName;
    std::source_location mLocation;
    TestOptions mOptions;
    Callable mCallable;
};

}

#include "Testament/detail/TestDefinition.tpp"

#endif
