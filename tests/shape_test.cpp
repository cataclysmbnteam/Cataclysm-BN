#include "shape_impl.h"
#include "catch/catch.hpp"

constexpr double deg2rad( double degrees )
{
    return degrees * M_PI / 180.0;
}

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
