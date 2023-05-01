#include "catch/catch.hpp"

#include "assertion_helpers.h"
#include "cata_utility.h"
#include "string_utils.h"
#include "units_utility.h"
#include "units.h"

// tests both variants of string_starts_with
template <std::size_t N>
bool test_string_starts_with( const std::string &s1, const char( &s2 )[N] )
{
    CAPTURE( s1, s2, N );
    bool r1 =  string_starts_with( s1, s2 );
    bool r2 =  string_starts_with( s1, std::string( s2 ) );
    CHECK( r1 == r2 );
    return r1;
}

// tests both variants of string_ends_with
template <std::size_t N>
bool test_string_ends_with( const std::string &s1, const char( &s2 )[N] )
{
    CAPTURE( s1, s2, N );
    bool r1 =  string_ends_with( s1, s2 );
    bool r2 =  string_ends_with( s1, std::string( s2 ) );
    CHECK( r1 == r2 );
    return r1;
}

TEST_CASE( "string_starts_with", "[utility]" )
{
    CHECK( test_string_starts_with( "", "" ) );
    CHECK( test_string_starts_with( "a", "" ) );
    CHECK_FALSE( test_string_starts_with( "", "a" ) );
    CHECK( test_string_starts_with( "ab", "a" ) );
    CHECK_FALSE( test_string_starts_with( "ab", "b" ) );
    CHECK_FALSE( test_string_starts_with( "a", "ab" ) );
}

TEST_CASE( "string_ends_with", "[utility]" )
{
    CHECK( test_string_ends_with( "", "" ) );
    CHECK( test_string_ends_with( "a", "" ) );
    CHECK_FALSE( test_string_ends_with( "", "a" ) );
    CHECK( test_string_ends_with( "ba", "a" ) );
    CHECK_FALSE( test_string_ends_with( "ba", "b" ) );
    CHECK_FALSE( test_string_ends_with( "a", "ba" ) );
}

TEST_CASE( "string_ends_with_benchmark", "[.][utility][benchmark]" )
{
    const std::string s1 = "long_string_with_suffix";

    BENCHMARK( "old version" ) {
        return string_ends_with( s1, std::string( "_suffix" ) );
    };
    BENCHMARK( "new version" ) {
        return string_ends_with( s1, "_suffix" );
    };
}

TEST_CASE( "string_ends_with_season_suffix", "[utility]" )
{
    constexpr size_t suffix_len = 15;
    constexpr char season_suffix[4][suffix_len] = {
        "_season_spring", "_season_summer", "_season_autumn", "_season_winter"
    };

    CHECK( test_string_ends_with( "t_tile_season_spring", season_suffix[0] ) );
    CHECK_FALSE( test_string_ends_with( "t_tile", season_suffix[0] ) );
    CHECK_FALSE( test_string_ends_with( "t_tile_season_summer", season_suffix[0] ) );
    CHECK_FALSE( test_string_ends_with( "t_tile_season_spring1", season_suffix[0] ) );
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

TEST_CASE( "trim_whitespaces", "[utility]" )
{
    CHECK( trim_whitespaces( "abc" ) == "abc" );
    CHECK( trim_whitespaces( "  abc" ) == "abc" );
    CHECK( trim_whitespaces( "abc  " ) == "abc" );
    CHECK( trim_whitespaces( "  abc  " ) == "abc" );
    // NOLINTNEXTLINE(cata-text-style)
    CHECK( trim_whitespaces( "  \t  \t  abc\t\t  \t\t\t  " ) == "abc" );
    // NOLINTNEXTLINE(cata-text-style)
    CHECK( trim_whitespaces( "abc  \t   def" ) == "abc  \t   def" );
    // NOLINTNEXTLINE(cata-text-style)
    CHECK( trim_whitespaces( "  abc  \t   def \t" ) == "abc  \t   def" );
    // NOLINTNEXTLINE(readability-container-size-empty)
    CHECK( trim_whitespaces( "   " ) == "" );
    // NOLINTNEXTLINE(readability-container-size-empty)
    CHECK( trim_whitespaces( " " ) == "" );
    // NOLINTNEXTLINE(readability-container-size-empty, cata-text-style)
    CHECK( trim_whitespaces( "\t" ) == "" );
    // NOLINTNEXTLINE(readability-container-size-empty)
    CHECK( trim_whitespaces( "" ) == "" );
}

struct split_test_data {
    size_t serial;
    std::string input;
    char delim;
    std::vector<std::string> expected;
};

TEST_CASE( "string_split", "[utility]" )
{
    static const std::vector<split_test_data> test_data = {{
            { 0, "", ' ', {} },
            { 1, "a", ' ', {"a"} },
            { 2, "ab", ' ', {"ab"} },
            { 3, "a b", ' ', {"a", "b"} },
            { 4, "a b c", ' ', {"a", "b", "c"} },
            { 5, " a", ' ', { "", "a"} },
            { 6, "a ", ' ', { "a", ""} },
            { 7, " a ", ' ', { "", "a", ""} },
            { 8, " ", ' ', { "", ""} },
            { 9, "  ", ' ', {"", "", ""} },
            {
                10,
                "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.",
                ' ', {
                    "Lorem", "ipsum", "dolor", "sit",
                    "amet,", "consectetur", "adipiscing", "elit,",
                    "sed", "do", "eiusmod", "tempor",
                    "incididunt", "ut", "labore", "et",
                    "dolore", "magna", "aliqua."
                }
            },
        }
    };

    SECTION( "std::string" ) {
        for( const split_test_data &it : test_data ) {
            CAPTURE( it.serial );
            std::vector<std::string> got = string_split( it.input, it.delim );
            CHECK( got == it.expected );
        }
    }

    SECTION( "std::string_view" ) {
        for( const split_test_data &it : test_data ) {
            CAPTURE( it.serial );
            std::vector<std::string_view> got = string_split_sv( it.input, it.delim );
            std::vector<std::string> got_s;
            for( std::string_view sv : got ) {
                got_s.emplace_back( sv );
            }
            CHECK( got_s == it.expected );
        }
    }
}

struct kb_test_data {
    size_t serial;
    std::string input;
    std::string expected;
};

TEST_CASE( "replace_keybind_tags", "[utility]" )
{
    static const std::vector<kb_test_data> test_data = {{
            {
                0,
                "Press <key:DEFAULTMODE:messages> to view message log.",
                "Press [<color_c_yellow>P</color>] to view message log."
            },
            {
                1,
                "Press <key::HELP_KEYBINDINGS> to view keybindings.",
                "Press [<color_c_yellow>?</color>] to view keybindings."
            },
            {
                2,
                "Press <key:DEFAULT:BROKEN_TAG:YELLOW> to use broken key.",
                "Press <key:DEFAULT:BROKEN_TAG:YELLOW> to use broken key.",
            },
            {
                3,
                "Press <color_c_yellow>KEY</color> to use broken key.",
                "Press <color_c_yellow>KEY</color> to use broken key.",
            },
            {
                4,
                "Press <key:DEFAULTMODE:messages to use broken key.",
                "Press <key:DEFAULTMODE:messages to use broken key.",
            }
        }
    };

    for( const kb_test_data &it : test_data ) {
        CAPTURE( it.serial );
        std::string got = replace_keybind_tags( it.input );
        CHECK( got == it.expected );
    }
}
