#pragma once

namespace Testament {

class LifecycleSuite;

namespace detail {

class LifecycleAccess {
public:
    static void beforeAll(LifecycleSuite& suite);
    static void beforeEach(LifecycleSuite& suite);
    static void afterEach(LifecycleSuite& suite);
    static void afterAll(LifecycleSuite& suite);
};

}
}
