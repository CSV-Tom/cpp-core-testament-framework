#include "SuiteCatalog.hpp"

#include "configuration/FilterMatcher.hpp"
#include "SuiteInstance.hpp"
#include "TestInstance.hpp"

#include <algorithm>
#include <random>
#include <span>
#include <string>
#include <utility>

namespace Testament::detail {

namespace {

void printTags(std::ostream& output, std::span<const std::string> tags) {
    output << "tags:";
    if (tags.empty()) {
        output << " -";
    } else {
        for (const auto& tag : tags) output << ' ' << tag;
    }
}

}

SuiteCatalog::SuiteCatalog(std::vector<std::shared_ptr<SuiteInstance>> suites,
                         std::optional<std::string_view> suiteFilter)
    : mSuites(std::move(suites)) {
    if (suiteFilter) {
        std::erase_if(mSuites, [suiteFilter](const auto& suite) {
            return !matchesNameFilter(suite->name(), *suiteFilter);
        });
    }
    std::ranges::sort(mSuites, [](const auto& left, const auto& right) {
        const auto leftOrder = left->options().order().value_or(0);
        const auto rightOrder = right->options().order().value_or(0);
        if (leftOrder != rightOrder) return leftOrder < rightOrder;
        return left->name() < right->name();
    });
}

const std::vector<std::shared_ptr<SuiteInstance>>& SuiteCatalog::suites() const noexcept {
    return mSuites;
}

std::vector<std::shared_ptr<SuiteInstance>> SuiteCatalog::forRun(
    std::optional<std::uint64_t> shuffleSeed
) const {
    auto result = mSuites;
    if (!shuffleSeed) return result;
    std::mt19937_64 random{*shuffleSeed};
    std::ranges::shuffle(result, random);
    std::ranges::stable_sort(result, [](const auto& left, const auto& right) {
        return left->options().order().value_or(0)
            < right->options().order().value_or(0);
    });
    return result;
}

void SuiteCatalog::list(std::ostream& output, std::string_view testFilter,
                       std::string_view expression) const {
    for (const auto& suite : mSuites) {
        output << suite->name() << " [";
        printTags(output, suite->options().tags());
        output << "]\n";
        for (const auto& test : suite->tests()) {
            if ((!testFilter.empty() && !matchesNameFilter(test->name(), testFilter))
                || (!expression.empty() && !matchesTestFilter(
                    suite->name(), suite->options().tags(), test->name(),
                    test->options().tags(), expression
                ))) {
                continue;
            }
            output << "  " << test->name() << " [";
            printTags(output, test->options().tags());
            output << ", " << (test->options().isDisabled() ? "disabled" : "enabled")
                   << "]\n";
        }
    }
}

}
