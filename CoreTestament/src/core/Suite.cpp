
#include "Internal/InternalSuite.hpp"
#include "Internal/InternalRegistry.hpp"

namespace Testament {

std::shared_ptr<Suite> Suite::create(const std::string& name) {
    return InternalRegistry::getInstance().registerSuite(std::make_unique<InternalSuite>(name));
}


 void Suite::addTest(const std::shared_ptr<Test>& test) {   
    if (auto internalSuite = dynamic_cast<InternalSuite*>(this)) {
        internalSuite->addTest(test);
    } else {
        throw std::logic_error("addTest called on a non-InternalSuite instance");
    }
 }
 
Suite::Suite() = default;
Suite::Suite(Suite&&) noexcept = default;
Suite& Suite::operator=(Suite&&) noexcept = default;
Suite::~Suite() = default;


std::shared_ptr<Suite> LifecycleSuite::create(const std::string& name, std::shared_ptr<LifecycleSuite> lifecycleSuite) {   
    auto suite = InternalRegistry::getInstance().registerSuite(std::make_unique<InternalSuite>(name));

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
