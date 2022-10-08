#pragma once
#ifndef CATA_SRC_STRING_UTILS_H
#define CATA_SRC_STRING_UTILS_H

#include <string>
#include <vector>

class translation;

/**
 * Perform case insensitive search for a query string inside a subject string.
 *
 * Searches for string given by qry inside a subject string given by str.
 *
 * @param str Subject to search for occurrence of the query string.
 * @param qry Query string to search for in str
 *
 * @return true if the query string is found at least once within the subject
 *         string, otherwise returns false
 */
auto lcmatch( const std::string &str, const std::string &qry ) -> bool;
auto lcmatch( const translation &str, const std::string &qry ) -> bool;

/** Perform case insensitive comparison of 2 strings. */
auto lcequal( const std::string &str1, const std::string &str2 ) -> bool;

/**
 * Matches text case insensitive with the include/exclude rules of the filter
 *
 * Multiple includes/excludes are possible
 *
 * Examle: bank,-house,tank,-car
 * Will match text containing tank or bank while not containing house or car
 *
 * @param text String to be matched
 * @param filter String with include/exclude rules
 *
 * @return true if include/exclude rules pass. See Example.
 */
auto match_include_exclude( const std::string &text, std::string filter ) -> bool;

/**
 * \brief Returns true if s1 starts with s2
 *
 * TODO: Switch to starts_with method of std::string when we move to C++20
 */
auto string_starts_with( const std::string &s1, const std::string &s2 ) -> bool;

/**
 * Returns true if s1 starts with s2.
 * This version accepts constant string literals and is ≈1.5 times faster than std::string version.
 * Note: N is (size+1) for null-terminated strings.
 *
 * TODO: Maybe switch to std::string::starts_with + std::string_view when we move to C++20
 */
template <std::size_t N>
inline auto string_starts_with( const std::string &s1, const char( &s2 )[N] ) -> bool
{
    return s1.compare( 0, N - 1, s2, N - 1 ) == 0;
}

/**
 * \brief Returns true if s1 ends with s2
 *
 * TODO: Switch to ends_with method of std::string when we move to C++20
 */
auto string_ends_with( const std::string &s1, const std::string &s2 ) -> bool;

/**
 *  Returns true iff s1 ends with s2.
 *  This version accepts constant string literals and is ≈1.5 times faster than std::string version.
 *  Note: N is (size+1) for null-terminated strings.
 *
 * TODO: Maybe switch to std::string::ends_with + std::string_view when we move to C++20
 */
template <std::size_t N>
inline auto string_ends_with( const std::string &s1, const char( &s2 )[N] ) -> bool
{
    return s1.size() >= N - 1 && s1.compare( s1.size() - ( N - 1 ), std::string::npos, s2, N - 1 ) == 0;
}

/**
 * Joins a vector of `std::string`s into a single string with a delimiter/joiner
 */
auto join( const std::vector<std::string> &strings, const std::string &joiner ) -> std::string;

/**
 * Split string by delimiter
 */
auto string_split( const std::string &text_in, char delim ) -> std::vector<std::string>;

/**
 * Find position of str2 within str1 (case-insensitive)
 */
auto ci_find_substr( const std::string &str1, const std::string &str2 ) -> int;

/**
 * Match text containing wildcards (*)
 * @param text_in Text to check
 * @param pattern_in Pattern to check text_in against
 * Case insensitive search
 * Possible patterns:
 * *
 * wooD
 * wood*
 * *wood
 * Wood*aRrOW
 * wood*arrow*
 * *wood*arrow
 * *wood*hard* *x*y*z*arrow*
 */
auto wildcard_match( const std::string &text_in, const std::string &pattern_in ) -> bool;

/**
 * Remove excessive '*' in wildcard rule.
 */
auto wildcard_trim_rule( const std::string &pattern_in ) -> std::string;

/**
 * Remove spaces from the start and the end of a string.
 */
auto trim( const std::string &s ) -> std::string;

/**
 * Removes punctuation marks from the start and the end of a string.
 */
auto trim_punctuation_marks( const std::string &s ) -> std::string;

/**
 * Converts the string to upper case.
 */
auto to_upper_case( const std::string &s ) -> std::string;

/**
 * Converts the string to lower case.
 */
auto to_lower_case( const std::string &s ) -> std::string;

/**
 * Replace name tags with actual names.
 */
void replace_name_tags( std::string &input );

/**
 * Replace city tag with given name.
 */
void replace_city_tag( std::string &input, const std::string &name );

/**
 * Replace first occurence of 'what' within 'input' with 'with'.
 */
void replace_first( std::string &input, const std::string &what, const std::string &with );

/**
 * Replace all occurences of 'what' within 'input' with 'with'.
 */
auto replace_all( std::string input, const std::string &what, const std::string &with ) -> std::string;

/**
 * Replace special color tags (e.g. info, bold, bad) with actual color tags.
 */
auto replace_colors( std::string text ) -> std::string;

/**
 * Capitalize nth ASCII letter. Don't use for Unicode strings!
 */
auto capitalize_letter( std::string &str, size_t n = 0 ) -> std::string &;

/**
 * Remove leading and trailing whitespaces.
 */
auto trim_whitespaces( const std::string &str ) -> std::string;

#endif // CATA_SRC_STRING_UTILS_H
