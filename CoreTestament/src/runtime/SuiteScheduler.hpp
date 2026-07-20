#pragma once

#include "TestCounts.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

namespace Testament {
class TestEventHandler;
}

namespace Testament::detail {

class SuiteInstance;

class SuiteScheduler {
public:
    struct Configuration {
        std::string_view testFilter;
        std::string_view expression;
        std::size_t maxParallelSuites{1};
        std::size_t maxParallelTests{1};
        std::optional<std::uint64_t> shuffleSeed;
    };

    struct Result {
        TestCounts<unsigned int> statistics;
        bool hooksSucceeded{true};

        [[nodiscard]] bool succeeded() const noexcept;
    };

    [[nodiscard]] static Result run(
        const std::vector<std::shared_ptr<SuiteInstance>>& suites,
        TestEventHandler& handler, Configuration configuration
    );
};

}
