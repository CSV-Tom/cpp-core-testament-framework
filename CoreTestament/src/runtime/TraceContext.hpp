#pragma once

#include <source_location>
#include <string>
#include <string_view>

namespace Testament::detail {

void pushTrace(std::string_view message, std::source_location location);
void popTrace() noexcept;
[[nodiscard]] std::string currentTrace();

}
