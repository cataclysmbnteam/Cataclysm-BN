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

// TODO: Better file
#include "map.h"
#include "ranged.h"
#include "projectile.h"
#include "map_helpers.h"
std::map<tripoint, double> expected_coverage(
    const shape &sh, const map &here, const projectile &proj );

static void shape_coverage_vs_distance_no_obstacle( const shape_factory_impl &c,
        const tripoint &origin, const tripoint &end )
{
    std::shared_ptr<shape> s = c.create( origin, end );
    projectile p;
    p.impact = damage_instance();
    p.impact.add_damage( DT_STAB, 10 );
    auto cov = expected_coverage( *s, get_map(), p );

    map &here = get_map();
    auto bb = s->bounding_box();
    REQUIRE( bb.p_min != bb.p_max );
    bool had_any = false;
    for( const tripoint &p : here.points_in_rectangle( bb.p_min, bb.p_max ) ) {
        double signed_distance = s->distance_at( p );
        bool distance_on_shape_is_negative = signed_distance < 0.0;
        bool point_is_covered = cov.find( p ) != cov.end() && cov.at( p ) > 0.0;
        CAPTURE( p );
        CAPTURE( signed_distance );
        CAPTURE( cov[p] );
        CHECK( distance_on_shape_is_negative == point_is_covered );
        had_any |= distance_on_shape_is_negative;
    }

    CHECK( had_any );
}

TEST_CASE( "expected shape coverage mass test", "[shape]" )
{
    clear_map();
    cone_factory c( deg2rad( 15 ), 10.0 );
    const tripoint origin( 60, 60, 0 );
    for( const tripoint &end : points_in_radius<tripoint>( origin, 5 ) ) {
        shape_coverage_vs_distance_no_obstacle( c, origin, end );
    }

    // Hard case
    shape_coverage_vs_distance_no_obstacle( c, {65, 65, 0}, tripoint{65, 65, 0} + point( 2, 1 ) );
}

TEST_CASE( "expected shape coverage without obstacles", "[shape]" )
{
    clear_map();
    cone_factory c( deg2rad( 22.5 ), 10.0 );
    const tripoint origin( 60, 60, 0 );
    const tripoint offset( 5, 5, 0 );
    const tripoint end = origin + offset;
    std::shared_ptr<shape> s = c.create( origin, end );
    projectile p;
    p.impact = damage_instance();
    p.impact.add_damage( DT_STAB, 10 );
    auto cov = expected_coverage( *s, get_map(), p );

    CHECK( cov[origin + point( 4, 4 )] == 1.0 );
    CHECK( cov[origin + point( 3, 3 )] == 1.0 );
    CHECK( cov[origin + point( 2, 2 )] == 1.0 );
    CHECK( cov[origin + point( 1, 1 )] == 1.0 );

    CHECK( cov[origin + point( 2, 1 )] == 1.0 );
    CHECK( cov[origin + point( 1, 2 )] == 1.0 );
}
