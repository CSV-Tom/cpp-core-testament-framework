#include "SuiteAssembler.hpp"

#include "InternalRegistry.hpp"
#include "InternalSuite.hpp"

#include "Testament/LifecycleSuite.hpp"
#include "Testament/SuiteOptions.hpp"
#include "Testament/detail/TestHandle.hpp"

#include <utility>

namespace Testament {

std::shared_ptr<InternalSuite> SuiteAssembler::assemble(
    std::string name, std::source_location location, std::type_index fixtureType,
    std::move_only_function<std::unique_ptr<LifecycleSuite>()> fixtureFactory,
    SuiteOptions options, std::vector<detail::TestHandle> tests
) {
    auto suite = fixtureFactory
        ? std::make_shared<InternalSuite>(std::move(name), location, fixtureType,
                                          std::move(fixtureFactory), std::move(options))
        : std::make_shared<InternalSuite>(std::move(name), location, std::move(options));
    for (auto& test : tests) suite->addTest(std::move(test));
    return InternalRegistry::getInstance().registerSuite(std::move(suite));
}

}
