#include "Testament/LifecycleSuite.hpp"

namespace Testament {

void LifecycleSuite::beforeAll() {}
void LifecycleSuite::beforeEach() {}
void LifecycleSuite::afterEach() {}
void LifecycleSuite::afterAll() {}

LifecycleSuite::LifecycleSuite() = default;
LifecycleSuite::LifecycleSuite(LifecycleSuite&&) noexcept = default;
LifecycleSuite& LifecycleSuite::operator=(LifecycleSuite&&) noexcept = default;
LifecycleSuite::~LifecycleSuite() = default;

}
