#include "shape_impl.h"
#include "catch/catch.hpp"

TEST_CASE( "cone_test", "[shape]" )
{
    SECTION( "90 degrees, length 1" ) {
        cone c( deg2rad( 90 ), 1.0 );
        CHECK( c.signed_distance( rl_vec3d{0.0, 0.0, 0.0} ) == Approx( 0.0 ).margin( 0.001 ) );
        CHECK( c.signed_distance( rl_vec3d{0.0, 1.0, 0.0} ) == Approx( 1.0 ).margin( 0.001 ) );
        CHECK( c.signed_distance( rl_vec3d{0.0, -1.0, 0.0} ) == Approx( 0.0 ).margin( 0.001 ) );

        CHECK( c.signed_distance( rl_vec3d{1.0, 0.0, 0.0} ) == Approx( 0.0 ).margin( 0.01 ) );
        CHECK( c.signed_distance( rl_vec3d{0.0, 0.0, 1.0} ) == Approx( 0.0 ).margin( 0.01 ) );

        CHECK( c.signed_distance( rl_vec3d{0.0, -0.5, 0.0} ) == Approx( -0.5 ).margin( 0.01 ) );
    }

    SECTION( "45 degrees, length 2" ) {
        cone c( deg2rad( 45 ), 2.0 );
        CHECK( c.signed_distance( rl_vec3d{0.0, 0.0, 0.0} ) == Approx( 0.0 ).margin( 0.001 ) );
        CHECK( c.signed_distance( rl_vec3d{0.0, 1.0, 0.0} ) == Approx( 1.0 ).margin( 0.001 ) );
        CHECK( c.signed_distance( rl_vec3d{0.0, -2.0, 0.0} ) == Approx( 0.0 ).margin( 0.001 ) );
        CHECK( c.signed_distance( rl_vec3d{0.0, -1.0, 0.0} ) == Approx( -0.707 ).margin( 0.01 ) );
    }
}

TEST_CASE( "offset_cone_test", "[shape]" )
{
    SECTION( "cone offset by (-1, 0, 0), 90 degrees, length 1" ) {
        offset_shape sh( std::make_shared<cone>( deg2rad( 90 ), 1.0 ), rl_vec3d{1.0, 0.0, 0.0} );
        CHECK( sh.signed_distance( rl_vec3d{1.0, 0.0, 0.0} ) == Approx( 0.0 ).margin( 0.001 ) );
        CHECK( sh.signed_distance( rl_vec3d{1.0, 1.0, 0.0} ) == Approx( 1.0 ).margin( 0.001 ) );
        CHECK( sh.signed_distance( rl_vec3d{1.0, -1.0, 0.0} ) == Approx( 0.0 ).margin( 0.001 ) );

        CHECK( sh.signed_distance( rl_vec3d{2.0, 0.0, 0.0} ) == Approx( 0.0 ).margin( 0.01 ) );
        CHECK( sh.signed_distance( rl_vec3d{1.0, 0.0, 1.0} ) == Approx( 0.0 ).margin( 0.01 ) );

        CHECK( sh.signed_distance( rl_vec3d{1.0, -0.5, 0.0} ) == Approx( -0.5 ).margin( 0.01 ) );
    }
}

TEST_CASE( "rotated_cone_test", "[shape]" )
{
    SECTION( "cone rotated by -90 degrees around z-axis to positive x direction, 90 degrees, length 1" ) {
        std::shared_ptr<cone> cn = std::make_shared<cone>( deg2rad( 90 ), 1.0 );
        std::shared_ptr<shape_impl> ro = std::make_shared<rotate_z_shape>( cn, deg2rad( -90 ) );
        const shape_impl &sh = *ro;
        REQUIRE( ( rl_vec3d{0.0, 1.0, 0.0}.rotated( deg2rad( -90 ) ) - rl_vec3d{1.0, 0.0, 0.0} ).magnitude()
                 == Approx( 0.0 ).margin( 0.00001 ) );
        CHECK( sh.signed_distance( rl_vec3d{0.0, 0.0, 0.0} ) == Approx( 0.0 ).margin( 0.001 ) );
        CHECK( sh.signed_distance( rl_vec3d{1.0, 0.0, 0.0} ) == Approx( 1.0 ).margin( 0.001 ) );
        CHECK( sh.signed_distance( rl_vec3d{-1.0, 0.0, 0.0} ) == Approx( 0.0 ).margin( 0.001 ) );

        CHECK( sh.signed_distance( rl_vec3d{0.0, 1.0, 0.0} ) == Approx( 0.0 ).margin( 0.01 ) );
        CHECK( sh.signed_distance( rl_vec3d{0.0, 0.0, 1.0} ) == Approx( 0.0 ).margin( 0.01 ) );

        CHECK( sh.signed_distance( rl_vec3d{0.5, 0.0, 0.0} ) == Approx( 0.5 ).margin( 0.01 ) );
    }
}

TEST_CASE( "offset_rotated_cone_test", "[shape]" )
{
    SECTION( "cone rotated by -90 degrees around z-axis to negative x direction, 90 degrees, length 1" ) {
        std::shared_ptr<cone> cn = std::make_shared<cone>( deg2rad( 90 ), 1.0 );
        std::shared_ptr<shape_impl> rotated = std::make_shared<rotate_z_shape>( cn, deg2rad( -90 ) );
        offset_shape sh( rotated, rl_vec3d{1.0, 0.0, 0.0} );
        CHECK( sh.signed_distance( rl_vec3d{1.0, 0.0, 0.0} ) == Approx( 0.0 ).margin( 0.001 ) );
        CHECK( sh.signed_distance( rl_vec3d{2.0, 0.0, 0.0} ) == Approx( 1.0 ).margin( 0.001 ) );
        CHECK( sh.signed_distance( rl_vec3d{0.0, 0.0, 0.0} ) == Approx( 0.0 ).margin( 0.001 ) );

        CHECK( sh.signed_distance( rl_vec3d{1.0, 1.0, 0.0} ) == Approx( 0.0 ).margin( 0.01 ) );
        CHECK( sh.signed_distance( rl_vec3d{1.0, 0.0, 1.0} ) == Approx( 0.0 ).margin( 0.01 ) );

        CHECK( sh.signed_distance( rl_vec3d{1.5, 0.0, 0.0} ) == Approx( 0.5 ).margin( 0.01 ) );
    }
}

TEST_CASE( "cone_factory_test", "[shape]" )
{
    cone_factory c( deg2rad( 15 ), 10.0 );
    SECTION( "(0,0,0) to (5,5,0)" ) {
        std::shared_ptr<shape> s = c.create( tripoint_zero, tripoint( 5, 5, 0 ) );
        CHECK( s->distance_at( rl_vec3d( 1, 1, 0 ) ) < 0.0 );

        CHECK( s->distance_at( rl_vec3d( 1, 0, 0 ) ) > 0.0 );
        CHECK( s->distance_at( rl_vec3d( 0, 1, 0 ) ) > 0.0 );

        CHECK( s->distance_at( rl_vec3d( -1, 0, 0 ) ) > 0.0 );
        CHECK( s->distance_at( rl_vec3d( 0, -1, 0 ) ) > 0.0 );
        CHECK( s->distance_at( rl_vec3d( -1, -1, 0 ) ) > 0.0 );

        CHECK( s->distance_at( rl_vec3d( 1, -1, 0 ) ) > 0.0 );
        CHECK( s->distance_at( rl_vec3d( -1, 1, 0 ) ) > 0.0 );

        CHECK( s->distance_at( rl_vec3d( 0, 0, 0 ) ) == Approx( 0.0 ) );

        CHECK( s->distance_at( rl_vec3d( 2, 2, 0 ) ) < 0.0 );
        CHECK( s->distance_at( rl_vec3d( 3, 3, 0 ) ) < 0.0 );
        CHECK( s->distance_at( rl_vec3d( 4, 4, 0 ) ) < 0.0 );
        CHECK( s->distance_at( rl_vec3d( 5, 5, 0 ) ) < 0.0 );
        CHECK( s->distance_at( rl_vec3d( 6, 6, 0 ) ) < 0.0 );
        CHECK( s->distance_at( rl_vec3d( 7, 7, 0 ) ) < 0.0 );

        CHECK( s->distance_at( rl_vec3d( 8, 8, 0 ) ) > 0.0 );
        CHECK( s->distance_at( rl_vec3d( 9, 9, 0 ) ) > 0.0 );
        CHECK( s->distance_at( rl_vec3d( 10, 10, 0 ) ) > 0.0 );

        auto bb = s->bounding_box();
        CAPTURE( bb.p_min );
        CAPTURE( bb.p_max );
        CHECK( bb.contains( tripoint( 1, 1, 0 ) ) );
        CHECK( bb.contains( tripoint( 2, 2, 0 ) ) );
        CHECK( bb.contains( tripoint( 3, 3, 0 ) ) );
        CHECK( bb.contains( tripoint( 4, 4, 0 ) ) );
        CHECK( bb.contains( tripoint( 5, 5, 0 ) ) );

        CHECK( !bb.contains( tripoint( -5, 0, 0 ) ) );
        CHECK( !bb.contains( tripoint( 0, -5, 0 ) ) );
    }

    SECTION( "(15,5,0) to (-15,5,0)" ) {
        std::shared_ptr<shape> s = c.create( tripoint( 15, 5, 0 ), tripoint( -15, 5, 0 ) );
        CHECK( s->distance_at( rl_vec3d( 20, 5, 0 ) ) > 0.0 );
        CHECK( s->distance_at( rl_vec3d( 0, 5, 0 ) ) > 0.0 );
        CHECK( s->distance_at( rl_vec3d( 10, 5, 0 ) ) < 0.0 );
    }
}
