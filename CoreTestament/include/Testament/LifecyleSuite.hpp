#ifndef TESTFRAMEWORK_TESTAMENT_LIFECYCLESUITE_HPP
#define TESTFRAMEWORK_TESTAMENT_LIFECYCLESUITE_HPP

#include "Testament/Suite.hpp"

namespace Testament {

class Test;

class LifecycleSuite : public Suite {
public:    
    static std::shared_ptr<Suite> create(const std::string& name, std::shared_ptr<LifecycleSuite> lifecycleSuite);
    
    LifecycleSuite();
    ~LifecycleSuite() override;

    LifecycleSuite(LifecycleSuite&&) noexcept;
    LifecycleSuite& operator=(LifecycleSuite&&) noexcept;

    LifecycleSuite(const LifecycleSuite&) = delete;
    LifecycleSuite& operator=(const LifecycleSuite&) = delete;
    
protected:    
    virtual void beforeAll();
    virtual void beforeEach();
    virtual void afterEach();
    virtual void afterAll();
};

}

#endif