#ifndef TESTAMENT_INTERNAL_COMMANDLINEOPTIONS_HPP
#define TESTAMENT_INTERNAL_COMMANDLINEOPTIONS_HPP

#include <cstdint>
#include <cstddef>
#include <expected>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace Testament::detail {

struct CommandLineOptions {
    std::vector<std::string_view> arguments;
    std::optional<std::string> filter;
    std::optional<std::uint64_t> seed;
    std::size_t repeat{1};
    bool listTests{};
    bool shuffle{};

    [[nodiscard]] static std::expected<CommandLineOptions, std::string> parse(
        int argc, char** argv
    );
};

}

#endif
