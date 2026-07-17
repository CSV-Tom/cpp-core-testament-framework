#include "Testament/Suite.hpp"
#include "Testament/Test.hpp"

#include "Internal/InternalSuite.hpp"
#include "Internal/InternalRegistry.hpp"

namespace Testament {

std::shared_ptr<Suite> Suite::create(const std::string& name)
{
    return InternalRegistry::getInstance().registerSuite(std::make_unique<InternalSuite>(name));
}

void Suite::addTest(Test test)
{
    if (auto internalSuite = dynamic_cast<InternalSuite*>(this)) {
        internalSuite->addTest(std::move(test));
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
