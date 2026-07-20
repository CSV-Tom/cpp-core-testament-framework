#include "Testament/LifecycleSuite.hpp"

#include "Internal/LifecycleAccess.hpp"

namespace Testament {

void LifecycleSuite::beforeAll() {}
void LifecycleSuite::beforeEach() {}
void LifecycleSuite::afterEach() {}
void LifecycleSuite::afterAll() {}

void detail::LifecycleAccess::beforeAll(LifecycleSuite& suite) {
    suite.beforeAll();
}

void detail::LifecycleAccess::beforeEach(LifecycleSuite& suite) {
    suite.beforeEach();
}

void detail::LifecycleAccess::afterEach(LifecycleSuite& suite) {
    suite.afterEach();
}

void detail::LifecycleAccess::afterAll(LifecycleSuite& suite) {
    suite.afterAll();
}

LifecycleSuite::LifecycleSuite() = default;
LifecycleSuite::LifecycleSuite(LifecycleSuite&&) noexcept = default;
LifecycleSuite& LifecycleSuite::operator=(LifecycleSuite&&) noexcept = default;
LifecycleSuite::~LifecycleSuite() = default;

}
