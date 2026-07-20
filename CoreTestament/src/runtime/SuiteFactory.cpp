#include "SuiteFactory.hpp"

#include "SuiteRegistry.hpp"
#include "SuiteInstance.hpp"

#include "Testament/LifecycleSuite.hpp"
#include "Testament/SuiteOptions.hpp"
#include "Testament/detail/TestHandle.hpp"

#include <utility>

namespace Testament::detail {

std::shared_ptr<SuiteInstance> SuiteFactory::create(
    std::string name, std::source_location location, std::type_index fixtureType,
    std::move_only_function<std::unique_ptr<LifecycleSuite>()> fixtureFactory,
    SuiteOptions options, std::vector<TestHandle> tests
) {
    auto suite = fixtureFactory
        ? std::make_shared<SuiteInstance>(std::move(name), location, fixtureType,
                                          std::move(fixtureFactory), std::move(options))
        : std::make_shared<SuiteInstance>(std::move(name), location, std::move(options));
    for (auto& test : tests) suite->addTest(std::move(test));
    return SuiteRegistry::instance().registerSuite(std::move(suite));
}

}
