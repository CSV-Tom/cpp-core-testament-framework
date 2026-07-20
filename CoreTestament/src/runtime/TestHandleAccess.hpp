#pragma once

#include <memory>

namespace Testament {

namespace detail {
class TestInstance;
class TestHandle;

class TestHandleAccess {
public:
    static std::unique_ptr<TestInstance> release(TestHandle&& test);
};

}
}
