#ifndef TESTAMENT_INTERNAL_TESTACCESS_HPP
#define TESTAMENT_INTERNAL_TESTACCESS_HPP

#include <memory>

namespace Testament {

class InternalTest;
class Test;

namespace detail {

class TestAccess {
public:
    static std::unique_ptr<InternalTest> release(Test&& test);
};

}

}

#endif
