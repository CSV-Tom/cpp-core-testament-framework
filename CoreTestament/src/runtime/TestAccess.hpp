#pragma once

#include <memory>

namespace Testament {

namespace detail {
class TestInstance;
class TestHandle;

class TestAccess {
public:
    static std::unique_ptr<TestInstance> release(TestHandle&& test);
};

}
}
