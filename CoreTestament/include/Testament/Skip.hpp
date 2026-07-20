#ifndef TESTAMENT_SKIP_HPP
#define TESTAMENT_SKIP_HPP

#include "Testament/Export.hpp"

#include <string_view>

namespace Testament {

[[noreturn]] TESTAMENT_EXPORT void skip(std::string_view reason = {});

}

#endif
