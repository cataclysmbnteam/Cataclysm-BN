#include "string_utils_fuzzy.h"

/// @see https://en.wikipedia.org/wiki/Korean_language_and_computers#Hangul_syllables_block
///
/// given a character '감'...
/// ㄱ = choseong (초성, initial consonant)
/// ㅏ = jungseong (중성, medial vowel)
/// ㅁ = jongseong (종성, final consonant)
///
/// index = (choseong * choseong_offset + jungseong * jungseong_offset + jongseong) + unicode_offset
/// 감 = (ㄱ * 588 + ㅏ * 28 + ㅁ) + 44032

namespace
{

// algorithm translated from https://taegon.kim/archives/9919
namespace korean
{

constexpr int offset = L'가';
constexpr int choseong_offset = 588;
constexpr int jungseong_offset = 28;

const auto con2syl = std::map<wchar_t, int>
{
    {L'ㄱ', L'가'},
    {L'ㄲ', L'까'},
    {L'ㄴ', L'나'},
    {L'ㄷ', L'다'},
    {L'ㄸ', L'따'},
    {L'ㄹ', L'라'},
    {L'ㅁ', L'마'},
    {L'ㅂ', L'바'},
    {L'ㅃ', L'빠'},
    {L'ㅅ', L'사'}
};

/// check if the character is a korean syllable in unicode.
auto is_syllable( const wchar_t c ) -> bool
{
    return L'가' <= c && c <= L'힣';
}

/// check if the character is a korean consonant(자음) in unicode.
auto is_consonant( const wchar_t c ) -> bool
{
    return L'ㄱ' <= c && c <= L'ㅎ';
}

/// check if the character is a korean jongseon(종성, final consonant) in unicode.
///
/// @param ch_code the character code with offset (`가`) subtracted.
auto is_jongseong( const wchar_t ch_code ) -> bool
{
    return ch_code % jungseong_offset > 0;
};

auto syllable_pattern( const wchar_t c ) -> std::wstring
{
    const int ch_code = c - offset;

    if( is_jongseong( ch_code ) ) {
        return std::wstring( 1, c );
    }

    /// the first character that has same choseong and jungseong with the given character.
    const int begin = ( ch_code / jungseong_offset ) * jungseong_offset + offset;
    /// the last character that has same choseong and jungseong with the given character.
    const int end = begin + ( jungseong_offset - 1 );

    return L"[" + std::wstring( 1, begin ) + L"-" + std::wstring( 1, end ) + L"]";
}

/// finds all characters that has same choseong with the given character.
auto consonant_pattern( const wchar_t c ) -> std::wstring
{
    const int begin = con2syl.count( c ) ? con2syl.at( c )
                      : ( L'사' + ( c - L'ㅅ' ) * choseong_offset );

    const int end = begin + ( choseong_offset - 1 );

    return L"["
           + std::wstring( 1, c ) + std::wstring( 1, begin )
           + L"-"
           + std::wstring( 1, end )
           + L"]";
}

} // namespace korean

// algorithm translated from https://github.com/lodash/lodash/blob/main/src/escapeRegExp.ts
auto escape_regex( const wchar_t c ) -> std::wstring
{
    constexpr auto escape = std::wstring_view{LR"(\^$.|?*+()[]{})"};

    if( escape.find( c ) == std::wstring::npos ) {
        return std::wstring( 1, c );
    }
    return LR"(\)" + std::wstring( 1, c );
}

auto into_pattern( const wchar_t c ) -> std::wstring
{
    if( korean::is_syllable( c ) ) {
        return korean::syllable_pattern( c );
    } else if( korean::is_consonant( c ) ) {
        return korean::consonant_pattern( c );
    } else {
        return escape_regex( c );
    }
}

auto fuzzy_pattern( const std::wstring_view s ) -> std::wstring
{
    auto xs = std::vector<std::wstring> {};
    for( const auto &c : s ) {
        xs.emplace_back( L"(" + into_pattern( c ) + L")" );
    }

    return wjoin( xs, LR"(.*?)" );
}

} // namespace

auto wjoin( const std::vector<std::wstring> &xs, const std::wstring_view sep ) -> std::wstring
{
    std::wstringstream result;

    for( auto it = xs.begin(); it != prev( xs.end() ); ++it ) {
        result << *it << sep;
    }
    result << xs.back();

    return result.str();
}

auto fuzzy_search( const std::wstring_view s ) -> cata::WRegExpMatcher
{
    const auto pattern = fuzzy_pattern( s );
    const auto regex = std::wregex( pattern, std::regex_constants::icase );

    return { pattern, regex };
}
