#ifndef TESTAMENT_GLOBALENVIRONMENT_HPP
#define TESTAMENT_GLOBALENVIRONMENT_HPP

namespace Testament {

class GlobalEnvironment {
public:
    virtual ~GlobalEnvironment() = default;
    virtual void setUp() {}
    virtual void tearDown() {}
};

}

#endif
