#ifndef TESTAMENT_SKIP_HPP
#define TESTAMENT_SKIP_HPP

#include <string_view>

namespace Testament {

[[noreturn]] void skip(std::string_view reason = {});

}

#endif
