#include "Testament/Suite.hpp"

#include "Internal/InternalSuite.hpp"
#include "Internal/InternalRegistry.hpp"

namespace Testament {

std::shared_ptr<Suite> Suite::create(const std::string& name)
{
    return InternalRegistry::getInstance().registerSuite(std::make_unique<InternalSuite>(name));
}

void Suite::addTest(const std::shared_ptr<Test>& test)
{
    if (auto internalSuite = dynamic_cast<InternalSuite*>(this)) {
        internalSuite->addTest(test);
    }
    else {
        throw std::logic_error("addTest called on a non-InternalSuite instance");
    }
}

Suite::Suite() = default;
Suite::Suite(Suite&&) noexcept = default;
Suite& Suite::operator=(Suite&&) noexcept = default;
Suite::~Suite() = default;

}
