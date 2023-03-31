#include "catch/catch.hpp"

#include <optional>
#include <utility>


TEST_CASE( "optional_assignment_works", "[optional]" )
{
    std::optional<int> a( 1 );
    REQUIRE( a );
    CHECK( *a == 1 );

    std::optional<int> b( 2 );
    std::optional<int> unset;
    a = b;
    REQUIRE( a );
    CHECK( *a == 2 );
    a = unset;
    CHECK( !a );
    a = std::move( b );
    REQUIRE( a );
    CHECK( *a == 2 );
    a = std::move( unset );
    CHECK( !a );

    const std::optional<int> c( 3 );
    a = c;
    REQUIRE( a );
    CHECK( *a == 3 );

    int d = 4;
    a = d;
    REQUIRE( a );
    CHECK( *a == 4 );
    a = d;
    REQUIRE( a );
    CHECK( *a == 4 );

    const int e = 5;
    a = e;
    REQUIRE( a );
    CHECK( *a == 5 );
}
