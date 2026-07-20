#ifndef TESTAMENT_DETAIL_RUNTIMEBRIDGE_HPP
#define TESTAMENT_DETAIL_RUNTIMEBRIDGE_HPP

#include "Testament/Export.hpp"
#include "Testament/SuiteRegistration.hpp"
#include "Testament/detail/TestHandle.hpp"

#include <functional>
#include <memory>
#include <source_location>
#include <string>
#include <string_view>
#include <typeindex>
#include <vector>

namespace Testament {

class LifecycleSuite;
class SuiteOptions;
class TestOptions;

namespace detail {

class TESTAMENT_EXPORT RuntimeBridge {
public:
    static TestHandle makeTest(
        std::string_view name, TestOptions options,
        std::move_only_function<void()> function,
        std::source_location location = std::source_location::current()
    );
    static TestHandle makeTest(std::string_view name, TestOptions options,
                               std::type_index fixtureType,
                               std::move_only_function<void(LifecycleSuite&)> function,
                               std::source_location location = std::source_location::current());

    static SuiteRegistration registerSuite(std::string_view name,
                                           std::source_location location,
                                           SuiteOptions options,
                                           std::vector<TestHandle> tests);
    static SuiteRegistration registerSuite(std::string_view name,
                                           std::source_location location,
                                           std::type_index fixtureType,
                                           std::move_only_function<
                                               std::unique_ptr<LifecycleSuite>()
                                           > fixtureFactory,
                                           SuiteOptions options,
                                           std::vector<TestHandle> tests);
    static SuiteRegistration configurationError(std::string message);
};

}
}

#endif
