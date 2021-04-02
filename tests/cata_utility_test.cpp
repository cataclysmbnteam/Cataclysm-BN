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

struct repl_test_data {
    size_t serial;
    std::string input;
    std::string what;
    std::string with;
    std::string expected;
};

TEST_CASE( "replace_all", "[utility]" )
{
    static const std::vector<repl_test_data> data = {{
            {0, "aaaaaaa", "aa", "aaab", "aaabaaabaaaba"},
            {1, "aaaaaaa", "bb", "aa", "aaaaaaa"},
            {2, "", "", "", ""},
            {3, "a", "a", "", ""},
            {4, "", "a", "a", ""},
            {5, "", "", "a", ""},
            {6, "a", "", "a", "a"},
        }
    };

    for( const repl_test_data &it : data ) {
        CAPTURE( it.serial );
        std::string res = replace_all( it.input, it.what, it.with );
        CHECK( res == it.expected );
    }
}

TEST_CASE( "replace_first", "[utility]" )
{
    static const std::vector<repl_test_data> data = {{
            {0, "aaaaaaa", "aa", "aaab", "aaabaaaaa"},
            {1, "aaaaaaa", "bb", "aa", "aaaaaaa"},
            {2, "", "", "", ""},
            {3, "a", "a", "", ""},
            {4, "", "a", "a", ""},
            {5, "", "", "a", ""},
            {6, "a", "", "a", "a"},
        }
    };

    for( const repl_test_data &it : data ) {
        CAPTURE( it.serial );
        std::string text = it.input;
        replace_first( text, it.what, it.with );
        CHECK( text == it.expected );
    }
}
