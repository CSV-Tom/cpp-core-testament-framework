#include "SuiteAssembler.hpp"

#include "InternalRegistry.hpp"
#include "InternalSuite.hpp"

#include "Testament/LifecycleSuite.hpp"
#include "Testament/SuiteOptions.hpp"
#include "Testament/detail/TestHandle.hpp"

#include <utility>

namespace Testament {

std::shared_ptr<InternalSuite> SuiteAssembler::assemble(
    std::string name, std::unique_ptr<LifecycleSuite> fixture,
    SuiteOptions options, std::vector<detail::TestHandle> tests
) {
    auto suite = fixture
        ? std::make_shared<InternalSuite>(std::move(name), std::move(fixture), std::move(options))
        : std::make_shared<InternalSuite>(std::move(name), std::move(options));
    for (auto& test : tests) suite->addTest(std::move(test));
    return InternalRegistry::getInstance().registerSuite(std::move(suite));
}

}
