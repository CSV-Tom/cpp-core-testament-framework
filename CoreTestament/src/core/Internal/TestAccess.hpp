#ifndef TESTAMENT_INTERNAL_TESTACCESS_HPP
#define TESTAMENT_INTERNAL_TESTACCESS_HPP

#include <memory>

namespace Testament {

class InternalTest;
namespace detail {
class TestHandle;
}

namespace detail {

class TestAccess {
public:
    static std::unique_ptr<InternalTest> release(TestHandle&& test);
};

}

}

#endif
