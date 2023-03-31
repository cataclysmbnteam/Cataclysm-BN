#include "catch/catch.hpp"

#include "decision_table_utils.h"

TEST_CASE( "Decision Table Inclusive" )
{
    auto value_color = decision_table<int, std::string>( {
        {25, "Red"},
        {75, "Green"},
        {50, "Yellow"},
    }, "Magenta" );

    SECTION( "Boundary Values" ) {
        REQUIRE( value_color( 76 ) == "Green" );
        REQUIRE( value_color( 75 ) == "Green" );
        REQUIRE( value_color( 74 ) == "Yellow" );
        REQUIRE( value_color( 51 ) == "Yellow" );
        REQUIRE( value_color( 50 ) == "Yellow" );
        REQUIRE( value_color( 49 ) == "Red" );
        REQUIRE( value_color( 26 ) == "Red" );
        REQUIRE( value_color( 25 ) == "Red" );
        REQUIRE( value_color( 24 ) == "Magenta" );
    }

    SECTION( "Value outside table ranges" ) {
        REQUIRE( value_color( 80000 ) == "Green" );
        REQUIRE( value_color( 10 ) == "Magenta" );
        REQUIRE( value_color( -112341 ) == "Magenta" );
    }
}
