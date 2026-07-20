#include "Testament/LifecycleSuite.hpp"

#include "runtime/LifecycleHookAccess.hpp"

namespace Testament {

void LifecycleSuite::beforeAll() {}
void LifecycleSuite::beforeEach() {}
void LifecycleSuite::afterEach() {}
void LifecycleSuite::afterAll() {}

void detail::LifecycleHookAccess::beforeAll(LifecycleSuite& suite) {
    suite.beforeAll();
}

void detail::LifecycleHookAccess::beforeEach(LifecycleSuite& suite) {
    suite.beforeEach();
}

void detail::LifecycleHookAccess::afterEach(LifecycleSuite& suite) {
    suite.afterEach();
}

void detail::LifecycleHookAccess::afterAll(LifecycleSuite& suite) {
    suite.afterAll();
}

LifecycleSuite::LifecycleSuite() = default;
LifecycleSuite::LifecycleSuite(LifecycleSuite&&) noexcept = default;
LifecycleSuite& LifecycleSuite::operator=(LifecycleSuite&&) noexcept = default;
LifecycleSuite::~LifecycleSuite() = default;

}
