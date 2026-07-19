#ifndef TESTAMENT_INTERNAL_FILTERPATTERN_HPP
#define TESTAMENT_INTERNAL_FILTERPATTERN_HPP

#include <span>
#include <string>
#include <string_view>

namespace Testament::detail {

inline bool globMatches(std::string_view value, std::string_view pattern) {
    std::size_t valueIndex{};
    std::size_t patternIndex{};
    std::size_t star = std::string_view::npos;
    std::size_t retryValue{};

    while (valueIndex < value.size()) {
        if (patternIndex < pattern.size()
            && (pattern[patternIndex] == '?' || pattern[patternIndex] == value[valueIndex])) {
            ++valueIndex;
            ++patternIndex;
        } else if (patternIndex < pattern.size() && pattern[patternIndex] == '*') {
            star = patternIndex++;
            retryValue = valueIndex;
        } else if (star != std::string_view::npos) {
            patternIndex = star + 1;
            valueIndex = ++retryValue;
        } else {
            return false;
        }
    }
    while (patternIndex < pattern.size() && pattern[patternIndex] == '*') ++patternIndex;
    return patternIndex == pattern.size();
}

inline bool anyTagMatches(std::span<const std::string> tags, std::string_view pattern) {
    for (const auto& tag : tags) {
        if (globMatches(tag, pattern)) return true;
    }
    return false;
}

inline bool matchesNameFilter(std::string_view name, std::string_view expression) {
    const bool negated = expression.starts_with('-');
    if (negated) expression.remove_prefix(1);
    const bool matched = globMatches(name, expression);
    return negated ? !matched : matched;
}

inline bool matchesTestFilter(std::string_view suiteName,
                              std::span<const std::string> suiteTags,
                              std::string_view testName,
                              std::span<const std::string> testTags,
                              std::string_view expression) {
    const bool negated = expression.starts_with('-');
    if (negated) expression.remove_prefix(1);

    bool matched{};
    if (expression.starts_with("tag:")) {
        expression.remove_prefix(4);
        matched = anyTagMatches(suiteTags, expression) || anyTagMatches(testTags, expression);
    } else {
        const auto fullName = std::string{suiteName} + '/' + std::string{testName};
        matched = globMatches(suiteName, expression)
            || globMatches(testName, expression)
            || globMatches(fullName, expression);
    }
    return negated ? !matched : matched;
}

}

#endif
