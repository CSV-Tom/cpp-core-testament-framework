#ifndef TESTFRAMEWORK_TESTAMENT_SUITE_HPP
#define TESTFRAMEWORK_TESTAMENT_SUITE_HPP

#include <memory>
#include <string>

namespace Testament {

class Test;

class Suite {
public:
    explicit Suite(const std::string& name);
    virtual ~Suite();

    virtual void beforeAll();
    virtual void beforeEach();
    virtual void afterEach();
    virtual void afterAll();

    void addTest(const std::shared_ptr<Test>& test);
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

}

#endif