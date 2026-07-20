#include "Testament/LifecycleSuite.hpp"
#include "Testament/detail/RuntimeBridge.hpp"

namespace Testament {

void LifecycleSuite::beforeAll() {}
void LifecycleSuite::beforeEach() {}
void LifecycleSuite::afterEach() {}
void LifecycleSuite::afterAll() {}

void detail::RuntimeBridge::beforeAll(LifecycleSuite& suite) {
    suite.beforeAll();
}

void detail::RuntimeBridge::beforeEach(LifecycleSuite& suite) {
    suite.beforeEach();
}

void detail::RuntimeBridge::afterEach(LifecycleSuite& suite) {
    suite.afterEach();
}

void detail::RuntimeBridge::afterAll(LifecycleSuite& suite) {
    suite.afterAll();
}

LifecycleSuite::LifecycleSuite() = default;
LifecycleSuite::LifecycleSuite(LifecycleSuite&&) noexcept = default;
LifecycleSuite& LifecycleSuite::operator=(LifecycleSuite&&) noexcept = default;
LifecycleSuite::~LifecycleSuite() = default;

}
