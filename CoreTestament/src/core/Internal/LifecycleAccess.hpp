#ifndef TESTAMENT_DETAIL_LIFECYCLEACCESS_HPP
#define TESTAMENT_DETAIL_LIFECYCLEACCESS_HPP

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

#endif
