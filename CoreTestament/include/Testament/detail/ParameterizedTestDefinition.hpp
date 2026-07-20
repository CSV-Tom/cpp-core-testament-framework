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
        explicit SynchronizedCallable(Callable function) : callable(std::move(function)) {}

        Callable callable;
        std::mutex mutex;
    };

public:
    ParameterizedTestDefinition(std::string name, std::source_location location,
                                TestOptions options,
                                CaseSet<Args...> cases, Callable callable);

    template <FixtureSelection Fixture>
    requires ParameterBodyCompatible<Fixture, Callable, Args...>::value
    std::vector<TestHandle> materialize() &&;

private:
    std::string mName;
    std::source_location mLocation;
    TestOptions mOptions;
    CaseSet<Args...> mCases;
    Callable mCallable;
};

}

#include "Testament/detail/ParameterizedTestDefinition.tpp"

#endif
