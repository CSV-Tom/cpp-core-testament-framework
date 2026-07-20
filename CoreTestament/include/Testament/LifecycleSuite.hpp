#ifndef TESTFRAMEWORK_TESTAMENT_LIFECYCLESUITE_HPP
#define TESTFRAMEWORK_TESTAMENT_LIFECYCLESUITE_HPP

#include "Testament/Export.hpp"

namespace Testament {

namespace detail {
class LifecycleHookAccess;
}

class TESTAMENT_EXPORT LifecycleSuite {
public:
    LifecycleSuite();
    virtual ~LifecycleSuite();

    LifecycleSuite(LifecycleSuite&&) noexcept;
    LifecycleSuite& operator=(LifecycleSuite&&) noexcept;

    LifecycleSuite(const LifecycleSuite&) = delete;
    LifecycleSuite& operator=(const LifecycleSuite&) = delete;

protected:
    virtual void beforeAll();
    virtual void beforeEach();
    virtual void afterEach();
    virtual void afterAll();

private:
    friend class detail::LifecycleHookAccess;
};

}

#endif
