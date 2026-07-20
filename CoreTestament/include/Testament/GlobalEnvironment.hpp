#ifndef TESTAMENT_GLOBALENVIRONMENT_HPP
#define TESTAMENT_GLOBALENVIRONMENT_HPP

#include "Testament/Export.hpp"

namespace Testament {

class TESTAMENT_EXPORT GlobalEnvironment {
public:
    virtual ~GlobalEnvironment() = default;
    virtual void setUp() {}
    virtual void tearDown() {}
};

}

#endif
