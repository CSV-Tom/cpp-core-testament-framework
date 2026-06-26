#ifndef TESTAMENT_TESTREGISTRY_HPP
#define TESTAMENT_TESTREGISTRY_HPP

#include <memory>

class TestSuite;
class TestEventHandler;

class TestRegistry {
public:
    static TestRegistry& instance();

    void addSuite(const std::shared_ptr<TestSuite>& suite);

    void addGlobalEventHandler(const std::shared_ptr<TestEventHandler>& handler);

    int run(int argc, char** argv);
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

#endif
