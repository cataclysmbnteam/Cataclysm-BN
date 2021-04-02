#include "catch/catch.hpp"
#include "cata_utility.h"
#include "string_utils.h"
#include "units_utility.h"
#include "units.h"

TEST_CASE( "string_starts_with", "[utility]" )
{
    CHECK( string_starts_with( "", "" ) );
    CHECK( string_starts_with( "a", "" ) );
    CHECK_FALSE( string_starts_with( "", "a" ) );
    CHECK( string_starts_with( "ab", "a" ) );
    CHECK_FALSE( string_starts_with( "ab", "b" ) );
    CHECK_FALSE( string_starts_with( "a", "ab" ) );
}

TEST_CASE( "string_ends_with", "[utility]" )
{
    CHECK( string_ends_with( "", "" ) );
    CHECK( string_ends_with( "a", "" ) );
    CHECK_FALSE( string_ends_with( "", "a" ) );
    CHECK( string_ends_with( "ba", "a" ) );
    CHECK_FALSE( string_ends_with( "ba", "b" ) );
    CHECK_FALSE( string_ends_with( "a", "ba" ) );
}

TEST_CASE( "divide_round_up", "[utility]" )
{
    CHECK( divide_round_up( 0, 5 ) == 0 );
    CHECK( divide_round_up( 1, 5 ) == 1 );
    CHECK( divide_round_up( 4, 5 ) == 1 );
    CHECK( divide_round_up( 5, 5 ) == 1 );
    CHECK( divide_round_up( 6, 5 ) == 2 );
}

TEST_CASE( "divide_round_up_units", "[utility]" )
{
    CHECK( divide_round_up( 0_ml, 5_ml ) == 0 );
    CHECK( divide_round_up( 1_ml, 5_ml ) == 1 );
    CHECK( divide_round_up( 4_ml, 5_ml ) == 1 );
    CHECK( divide_round_up( 5_ml, 5_ml ) == 1 );
    CHECK( divide_round_up( 6_ml, 5_ml ) == 2 );
}

TEST_CASE( "replace_all", "[utility]" )
{
    static const std::vector<std::array<std::string, 4>> data = {
        {"aaaaaaa", "aa", "aaab", "aaabaaabaaaba"},
        {"aaaaaaa", "bb", "aa", "aaaaaaa"},
        {"", "", "", ""},
        {"a", "a", "", ""},
        {"", "a", "a", ""},
        {"", "", "a", ""},
        {"a", "", "a", "a"},
    };

    for( size_t i = 0; i < data.size(); i++ ) {
        CAPTURE( i );
        std::string res = replace_all( data[i][0], data[i][1], data[i][2] );
        CHECK( res == data[i][3] );
    }
}

TEST_CASE( "replace_first", "[utility]" )
{
    static const std::vector<std::array<std::string, 4>> data = {
        {"aaaaaaa", "aa", "aaab", "aaabaaaaa"},
        {"aaaaaaa", "bb", "aa", "aaaaaaa"},
        {"", "", "", ""},
        {"a", "a", "", ""},
        {"", "a", "a", ""},
        {"", "", "a", ""},
        {"a", "", "a", "a"},
    };

    for( size_t i = 0; i < data.size(); i++ ) {
        CAPTURE( i );
        std::string text = data[i][0];
        replace_first( text, data[i][1], data[i][2] );
        CHECK( text == data[i][3] );
    }
}
