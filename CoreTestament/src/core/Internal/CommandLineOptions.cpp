#include "CommandLineOptions.hpp"

#include <charconv>
#include <limits>

namespace Testament::detail {

namespace {

std::optional<std::uint64_t> parseUnsigned(std::string_view value) {
    std::uint64_t result{};
    const auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), result);
    if (error != std::errc{} || end != value.data() + value.size()) return std::nullopt;
    return result;
}

}

std::expected<CommandLineOptions, std::string> CommandLineOptions::parse(int argc, char** argv) {
    if (argc < 0 || (argc > 0 && !argv)) {
        return std::unexpected("Invalid command-line arguments");
    }

    CommandLineOptions result;
    result.arguments.reserve(argc > 1 ? static_cast<std::size_t>(argc - 1) : 0U);
    for (int index = 1; index < argc; ++index) {
        if (!argv[index]) return std::unexpected("Invalid command-line arguments");
        result.arguments.emplace_back(argv[index]);
    }

    for (const auto argument : result.arguments) {
        if (argument == "--list-tests") {
            result.listTests = true;
        } else if (argument == "--shuffle") {
            result.shuffle = true;
        } else if (argument.starts_with("--repeat=")) {
            const auto parsed = parseUnsigned(argument.substr(9));
            if (!parsed || *parsed == 0 || *parsed > std::numeric_limits<std::size_t>::max()) {
                return std::unexpected("--repeat requires an integer greater than zero");
            }
            result.repeat = static_cast<std::size_t>(*parsed);
        } else if (argument.starts_with("--seed=")) {
            result.seed = parseUnsigned(argument.substr(7));
            if (!result.seed) return std::unexpected("--seed requires an unsigned integer");
            result.shuffle = true;
        } else if (argument.starts_with("--filter=")) {
            if (result.filter) return std::unexpected("--filter may only be specified once");
            const auto value = argument.substr(std::string_view{"--filter="}.size());
            if (value.empty() || value == "-" || value == "tag:" || value == "-tag:") {
                return std::unexpected("--filter requires a non-empty pattern");
            }
            result.filter = value;
        }
    }
    return result;
}

}
