#ifndef TESTFRAMEWORK_TESTAMENT_SUITE_HPP
#define TESTFRAMEWORK_TESTAMENT_SUITE_HPP

#include <memory>
#include <string>

namespace Testament {

class Test;

class Suite {
public:

    static std::shared_ptr<Suite> create(const std::string& name);
    virtual void addTest(const std::shared_ptr<Test>& test);  
      
protected:
    Suite();
    virtual ~Suite();

    Suite(Suite&&) noexcept;
    Suite& operator=(Suite&&) noexcept;

    Suite(const Suite&) = delete;
    Suite& operator=(const Suite&) = delete;        
};

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