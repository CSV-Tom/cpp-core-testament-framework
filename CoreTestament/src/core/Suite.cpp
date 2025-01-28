
#include "SuiteImpl.hpp"

namespace Testament {

Suite::Suite(const std::string& name) : pImpl(std::make_unique<Impl>(name)) {}
Suite::~Suite() = default;

void Suite::beforeAll() {
    pImpl->beforeAll();    
}

void Suite::beforeEach() {
    pImpl->beforeEach();
}

void Suite::afterEach() {
    pImpl->afterEach();
}

void Suite::afterAll() {
    pImpl->afterAll();
}

void Suite::addTest(const std::shared_ptr<Test>& test) {
    pImpl->addTest(test);
}

}
