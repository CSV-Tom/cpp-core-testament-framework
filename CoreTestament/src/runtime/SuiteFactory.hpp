#pragma once

#include <memory>
#include <source_location>
#include <functional>
#include <string>
#include <typeindex>
#include <vector>

namespace Testament {
class LifecycleSuite;
class SuiteOptions;

namespace detail {
class SuiteInstance;
class TestHandle;

class SuiteFactory {
public:
    [[nodiscard]] static std::shared_ptr<SuiteInstance> create(
        std::string name, std::source_location location, std::type_index fixtureType,
        std::move_only_function<std::unique_ptr<LifecycleSuite>()> fixtureFactory,
        SuiteOptions options, std::vector<TestHandle> tests
    );
};

}
}
