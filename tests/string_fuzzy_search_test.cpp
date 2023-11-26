#include "catch/catch.hpp"

#include <catacharset.h>
#include <clocale>
#include <string_utils_fuzzy.h>
#include <regex>

namespace
{

struct Case {
    std::wstring pattern;
    std::wstring target;
    bool expected;
};

} //namespace


TEST_CASE( "fuzzy_search" )
{
    SECTION( "korean" ) {
        ( void )std::setlocale( LC_ALL, "ko_KR.UTF-8" );

        const auto cases = std::vector<Case> {
            {L"ㅋㅁㅅ", L"크리스마스", true},
            {L"ㅋㅁㅅ", L"크리스", false},
            {L"고라", L"골라", true},
            {L"고라", L"가라", false},
            {L"군ㄱㅁ", L"군고구마", true},
            {L"군ㄱㅁ", L"궁고구마", false},

            {L"ㅋㅌㅋ", L"카타클리즘", true},
            {L"ㅋㅌㅋ", L"타클리즘", false},
            {L"바밤", L"밝밤", true},
            {L"바밤", L"붉밤", false},
            {L"좀ㅂ", L"좀비", true},
            {L"좀ㅂ", L"존비", false},
        };
        for( const auto &c : cases ) {
            const auto matcher = fuzzy_search( c.pattern );

            INFO( "pattern: " << wstr_to_utf8( c.pattern )
                  << ", target: " << wstr_to_utf8( c.target )
                  << ", regex: /" << wstr_to_utf8( matcher.pattern ) << "/" );
            CHECK( std::regex_search( c.target, matcher.regex ) == c.expected );
        }
    }
}
