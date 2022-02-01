#include <string>
#include <vector>

#include "catch/catch.hpp"
#include "point.h"

TEST_CASE( "rectangle_containment", "[point]" )
{
    // NOLINTNEXTLINE(cata-use-named-point-constants)
    half_open_rectangle r1( point( 0, 0 ), point( 2, 2 ) );
    CHECK( !r1.contains( point( 0, -1 ) ) ); // NOLINT(cata-use-named-point-constants)
    CHECK( r1.contains( point( 0, 0 ) ) ); // NOLINT(cata-use-named-point-constants)
    CHECK( r1.contains( point( 0, 1 ) ) ); // NOLINT(cata-use-named-point-constants)
    CHECK( !r1.contains( point( 0, 2 ) ) );
    CHECK( !r1.contains( point( 0, 3 ) ) );

    // NOLINTNEXTLINE(cata-use-named-point-constants)
    inclusive_rectangle r2( point( 0, 0 ), point( 2, 2 ) );
    CHECK( !r2.contains( point( 0, -1 ) ) ); // NOLINT(cata-use-named-point-constants)
    CHECK( r2.contains( point( 0, 0 ) ) ); // NOLINT(cata-use-named-point-constants)
    CHECK( r2.contains( point( 0, 1 ) ) ); // NOLINT(cata-use-named-point-constants)
    CHECK( r2.contains( point( 0, 2 ) ) );
    CHECK( !r2.contains( point( 0, 3 ) ) );
}

TEST_CASE( "rectangle_overlapping", "[point]" )
{
    SECTION( "inclusive" ) {
        // NOLINTNEXTLINE(cata-use-named-point-constants)
        inclusive_rectangle r1( point( 0, 0 ), point( 2, 2 ) );
        inclusive_rectangle r2( point( 2, 2 ), point( 3, 3 ) );
        // NOLINTNEXTLINE(cata-use-named-point-constants)
        inclusive_rectangle r3( point( 0, 0 ), point( 2, 1 ) );
        inclusive_rectangle r4( point( -2, -4 ), point( 4, -1 ) );
        inclusive_rectangle r5( point( -1, -3 ), point( 0, -2 ) );

        CHECK( r1.overlaps_inclusive( r1 ) );
        CHECK( r1.overlaps_inclusive( r2 ) );
        CHECK( r1.overlaps_inclusive( r3 ) );
        CHECK( !r1.overlaps_inclusive( r4 ) );
        CHECK( !r1.overlaps_inclusive( r5 ) );

        CHECK( r2.overlaps_inclusive( r1 ) );
        CHECK( r2.overlaps_inclusive( r2 ) );
        CHECK( !r2.overlaps_inclusive( r3 ) );
        CHECK( !r2.overlaps_inclusive( r4 ) );
        CHECK( !r2.overlaps_inclusive( r5 ) );

        CHECK( r3.overlaps_inclusive( r1 ) );
        CHECK( !r3.overlaps_inclusive( r2 ) );
        CHECK( r3.overlaps_inclusive( r3 ) );
        CHECK( !r3.overlaps_inclusive( r4 ) );
        CHECK( !r3.overlaps_inclusive( r5 ) );

        CHECK( !r4.overlaps_inclusive( r1 ) );
        CHECK( !r4.overlaps_inclusive( r2 ) );
        CHECK( !r4.overlaps_inclusive( r3 ) );
        CHECK( r4.overlaps_inclusive( r4 ) );
        CHECK( r4.overlaps_inclusive( r5 ) );
        CHECK( r5.overlaps_inclusive( r4 ) );
    }

    SECTION( "half_open" ) {
        // NOLINTNEXTLINE(cata-use-named-point-constants)
        half_open_rectangle r1( point( 0, 0 ), point( 2, 2 ) );
        half_open_rectangle r2( point( 2, 2 ), point( 3, 3 ) );
        // NOLINTNEXTLINE(cata-use-named-point-constants)
        half_open_rectangle r3( point( 0, 0 ), point( 2, 1 ) );
        half_open_rectangle r4( point( -2, -4 ), point( 4, -1 ) );
        half_open_rectangle r5( point( -1, -3 ), point( 0, -2 ) );

        CHECK( r1.overlaps_half_open( r1 ) );
        CHECK( !r1.overlaps_half_open( r2 ) );
        CHECK( r1.overlaps_half_open( r3 ) );
        CHECK( !r1.overlaps_half_open( r4 ) );
        CHECK( !r1.overlaps_half_open( r5 ) );

        CHECK( !r2.overlaps_half_open( r1 ) );
        CHECK( r2.overlaps_half_open( r2 ) );
        CHECK( !r2.overlaps_half_open( r3 ) );
        CHECK( !r2.overlaps_half_open( r4 ) );
        CHECK( !r2.overlaps_half_open( r5 ) );

        CHECK( r3.overlaps_half_open( r1 ) );
        CHECK( !r3.overlaps_half_open( r2 ) );
        CHECK( r3.overlaps_half_open( r3 ) );
        CHECK( !r3.overlaps_half_open( r4 ) );
        CHECK( !r3.overlaps_half_open( r5 ) );

        CHECK( !r4.overlaps_half_open( r1 ) );
        CHECK( !r4.overlaps_half_open( r2 ) );
        CHECK( !r4.overlaps_half_open( r3 ) );
        CHECK( r4.overlaps_half_open( r4 ) );
        CHECK( r4.overlaps_half_open( r5 ) );
        CHECK( r5.overlaps_half_open( r4 ) );
    }
}

TEST_CASE( "box_shrinks", "[point]" )
{
    half_open_box b( tripoint_zero, tripoint( 3, 3, 3 ) );
    CAPTURE( b );
    CHECK( b.contains( tripoint( 1, 0, 0 ) ) ); // NOLINT(cata-use-named-point-constants)
    CHECK( b.contains( tripoint( 2, 1, 2 ) ) );
    b.shrink( tripoint( 1, 1, 0 ) ); // NOLINT(cata-use-named-point-constants)
    CAPTURE( b );
    // Shrank in the x and y directions
    // NOLINTNEXTLINE(cata-use-named-point-constants)
    CHECK( !b.contains( tripoint( 1, 0, 0 ) ) );
    CHECK( !b.contains( tripoint( 2, 1, 2 ) ) );
    // Didn't shrink in the z direction
    // NOLINTNEXTLINE(cata-use-named-point-constants)
    CHECK( b.contains( tripoint( 1, 1, 0 ) ) );
    CHECK( b.contains( tripoint( 1, 1, 2 ) ) );
}

TEST_CASE( "point_to_string", "[point]" )
{
    CHECK( point_south.to_string() == "(0,1)" );
    CHECK( tripoint( -1, 0, 1 ).to_string() == "(-1,0,1)" );
}

TEST_CASE( "tripoint_xy", "[point]" )
{
    tripoint p( 1, 2, 3 );
    CHECK( p.xy() == point( 1, 2 ) );
}

TEST_CASE( "closest_tripoints_first", "[point]" )
{
    const tripoint center = { 1, -1, 2 };

    GIVEN( "min_dist > max_dist" ) {
        const std::vector<tripoint> result = closest_tripoints_first( center, 1, 0 );

        CHECK( result.empty() );
    }

    GIVEN( "min_dist = max_dist = 0" ) {
        const std::vector<tripoint> result = closest_tripoints_first( center, 0, 0 );

        CHECK( result.size() == 1 );
        CHECK( result[0] == tripoint{ 1, -1, 2 } );
    }

    GIVEN( "min_dist = 0, max_dist = 1" ) {
        const std::vector<tripoint> result = closest_tripoints_first( center, 0, 1 );

        CHECK( result.size() == 9 );
        CHECK( result[0] == tripoint{  1, -1, 2 } );
        CHECK( result[1] == tripoint{  2, -1, 2 } );
        CHECK( result[2] == tripoint{  2,  0, 2 } );
        CHECK( result[3] == tripoint{  1,  0, 2 } );
        CHECK( result[4] == tripoint{  0,  0, 2 } );
        CHECK( result[5] == tripoint{  0, -1, 2 } );
        CHECK( result[6] == tripoint{  0, -2, 2 } );
        CHECK( result[7] == tripoint{  1, -2, 2 } );
        CHECK( result[8] == tripoint{  2, -2, 2 } );
    }

    GIVEN( "min_dist = 2, max_dist = 2" ) {
        const std::vector<tripoint> result = closest_tripoints_first( center, 2, 2 );

        CHECK( result.size() == 16 );

        CHECK( result[0]  == tripoint{  3, -2, 2 } );
        CHECK( result[1]  == tripoint{  3, -1, 2 } );
        CHECK( result[2]  == tripoint{  3,  0, 2 } );
        CHECK( result[3]  == tripoint{  3,  1, 2 } );
        CHECK( result[4]  == tripoint{  2,  1, 2 } );
        CHECK( result[5]  == tripoint{  1,  1, 2 } );
        CHECK( result[6]  == tripoint{  0,  1, 2 } );
        CHECK( result[7]  == tripoint{ -1,  1, 2 } );
        CHECK( result[8]  == tripoint{ -1,  0, 2 } );
        CHECK( result[9]  == tripoint{ -1, -1, 2 } );
        CHECK( result[10] == tripoint{ -1, -2, 2 } );
        CHECK( result[11] == tripoint{ -1, -3, 2 } );
        CHECK( result[12] == tripoint{  0, -3, 2 } );
        CHECK( result[13] == tripoint{  1, -3, 2 } );
        CHECK( result[14] == tripoint{  2, -3, 2 } );
        CHECK( result[15] == tripoint{  3, -3, 2 } );
    }
}
