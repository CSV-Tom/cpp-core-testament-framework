#ifndef TESTAMENT_DETAIL_RUNTIMEBRIDGE_HPP
#define TESTAMENT_DETAIL_RUNTIMEBRIDGE_HPP

#include "Testament/SuiteRegistration.hpp"
#include "Testament/detail/TestHandle.hpp"

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <typeindex>
#include <vector>

namespace Testament {

class LifecycleSuite;
class SuiteOptions;
class TestOptions;

namespace detail {

class RuntimeBridge {
public:
    static TestHandle makeTest(std::string_view name, TestOptions options,
                               std::move_only_function<void()> function);
    static TestHandle makeTest(std::string_view name, TestOptions options,
                               std::type_index fixtureType,
                               std::move_only_function<void(LifecycleSuite&)> function);

    static SuiteRegistration registerSuite(std::string_view name, SuiteOptions options,
                                           std::vector<TestHandle> tests);
    static SuiteRegistration registerSuite(std::string_view name,
                                           std::type_index fixtureType,
                                           std::move_only_function<
                                               std::unique_ptr<LifecycleSuite>()
                                           > fixtureFactory,
                                           SuiteOptions options,
                                           std::vector<TestHandle> tests);
    static SuiteRegistration invalidRegistration();
    static void recordConfigurationError(std::string message);
};

}
}

#endif
