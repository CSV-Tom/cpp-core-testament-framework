#include "Testament/LifecyleSuite.hpp"

#include "Internal/InternalSuite.hpp"
#include "Internal/InternalRegistry.hpp"

namespace Testament {

std::shared_ptr<Suite> LifecycleSuite::create(const std::string& name, std::shared_ptr<LifecycleSuite> lifecycleSuite) {
    auto suite = InternalRegistry::getInstance().registerSuite(std::make_unique<InternalSuite>(name, lifecycleSuite));

    suite->setBeforeSuite([lifecycleSuite]() { lifecycleSuite->beforeAll(); });
    suite->setBeforeEach([lifecycleSuite]() { lifecycleSuite->beforeEach(); });
    suite->setAfterEach([lifecycleSuite]() { lifecycleSuite->afterEach(); });
    suite->setAfterSuite([lifecycleSuite]() { lifecycleSuite->afterAll(); });

    return suite;
}

void LifecycleSuite::beforeAll() {}
void LifecycleSuite::beforeEach() {}
void LifecycleSuite::afterEach() {}
void LifecycleSuite::afterAll() {}

LifecycleSuite::LifecycleSuite() = default;
LifecycleSuite::LifecycleSuite(LifecycleSuite&&) noexcept = default;
LifecycleSuite& LifecycleSuite::operator=(LifecycleSuite&&) noexcept = default;
LifecycleSuite::~LifecycleSuite() = default;

}
