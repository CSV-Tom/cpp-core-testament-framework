#include "Testament/Suite.hpp"

#include <stdexcept>
#include <vector>
#include <string>

namespace Testament {

class Suite::Impl {
public:
    explicit Impl(const std::string& name) : name(name) {}

    void addTest(const std::shared_ptr<Test>& test) {
        if (!test) {
            throw std::logic_error("Attempted to add a null test to the Suite");
        }
        tests.push_back(test);
    }

    void beforeAll() {}
    void beforeEach() {}
    void afterEach() {}
    void afterAll() {}

private:
    std::string name;
    std::vector<std::shared_ptr<Test>> tests;
};

}
