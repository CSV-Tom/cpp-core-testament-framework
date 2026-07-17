#ifndef TESTFRAMEWORK_TESTAMENT_LIFECYCLESUITE_HPP
#define TESTFRAMEWORK_TESTAMENT_LIFECYCLESUITE_HPP

namespace Testament {

class LifecycleSuite {
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
    friend class InternalSuite;
};

}

#endif
