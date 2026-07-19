#ifndef TESTAMENT_INTERNAL_TRACECONTEXT_HPP
#define TESTAMENT_INTERNAL_TRACECONTEXT_HPP

#include <source_location>
#include <string>
#include <string_view>

namespace Testament::detail {

void pushTrace(std::string_view message, std::source_location location);
void popTrace() noexcept;
[[nodiscard]] std::string currentTrace();

}

#endif
