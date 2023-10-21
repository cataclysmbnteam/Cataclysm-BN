#pragma once
#ifndef CATA_SRC_STRING_UTILS_FUZZY_H
#define CATA_SRC_STRING_UTILS_FUZZY_H

#include <regex>
#include <string_view>

/// join a vector of `std::wstring_view`s into a single string with a delimiter/joiner.
auto wjoin( const std::vector<std::wstring> &xs, const std::wstring_view sep ) -> std::wstring;

namespace cata
{

struct WRegExpMatcher {
    /// text representation of the pattern
    std::wstring pattern;

    /// case-insensitive regex matcher
    std::wregex regex;
};

} // namespace cata

/// creates a fuzzy search regex pattern for a given string.
/// supports english and korean.
auto fuzzy_search( const std::wstring_view s ) -> cata::WRegExpMatcher;

#endif // CATA_SRC_STRING_UTILS_FUZZY_H

