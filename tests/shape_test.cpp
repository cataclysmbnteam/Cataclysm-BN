#include "shape_impl.h"
#include "catch/catch.hpp"

class square_distance_from_zero_shape : public shape_impl
{
        double signed_distance( const rl_vec3d &p ) const override {
            return p.dot_product( p );
        }

        inclusive_cuboid<rl_vec3d> bounding_box() const override {
            return inclusive_cuboid<rl_vec3d>( rl_vec3d(), rl_vec3d() );
        }
};

TEST_CASE( "cone_test", "[shape]" )
{
    SECTION( "90 degrees, length 1" ) {
        cone c( 90_degrees, 1.0 );
        CHECK( c.signed_distance( rl_vec3d{0.0, 0.0, 0.0} ) == Approx( 0.0 ).margin( 0.001 ) );
        CHECK( c.signed_distance( rl_vec3d{0.0, 1.0, 0.0} ) == Approx( 1.0 ).margin( 0.001 ) );
        CHECK( c.signed_distance( rl_vec3d{0.0, -1.0, 0.0} ) == Approx( 0.0 ).margin( 0.001 ) );

        CHECK( c.signed_distance( rl_vec3d{1.0, 0.0, 0.0} ) == Approx( 0.0 ).margin( 0.01 ) );
        CHECK( c.signed_distance( rl_vec3d{0.0, 0.0, 1.0} ) == Approx( 0.0 ).margin( 0.01 ) );

        CHECK( c.signed_distance( rl_vec3d{0.0, -0.5, 0.0} ) == Approx( -0.5 ).margin( 0.01 ) );
    }

    SECTION( "45 degrees, length 2" ) {
        cone c( 45_degrees, 2.0 );
        CHECK( c.signed_distance( rl_vec3d{0.0, 0.0, 0.0} ) == Approx( 0.0 ).margin( 0.001 ) );
        CHECK( c.signed_distance( rl_vec3d{0.0, 1.0, 0.0} ) == Approx( 1.0 ).margin( 0.001 ) );
        CHECK( c.signed_distance( rl_vec3d{0.0, -2.0, 0.0} ) == Approx( 0.0 ).margin( 0.001 ) );
        CHECK( c.signed_distance( rl_vec3d{0.0, -1.0, 0.0} ) == Approx( -0.707 ).margin( 0.01 ) );
    }
}

TEST_CASE( "offset_cone_test", "[shape]" )
{
    SECTION( "cone offset by (-1, 0, 0), 90 degrees, length 1" ) {
        offset_shape sh( std::make_shared<cone>( 90_degrees, 1.0 ), rl_vec3d{1.0, 0.0, 0.0} );
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
        std::shared_ptr<cone> cn = std::make_shared<cone>( 90_degrees, 1.0 );
        std::shared_ptr<shape_impl> ro = std::make_shared<rotate_z_shape>( cn, -90_degrees );
        const shape_impl &sh = *ro;
        REQUIRE( ( rl_vec3d{0.0, 1.0, 0.0}.rotated( units::to_radians( -90_degrees ) ) - rl_vec3d{1.0, 0.0, 0.0} ).magnitude()
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
        std::shared_ptr<cone> cn = std::make_shared<cone>( 90_degrees, 1.0 );
        std::shared_ptr<shape_impl> rotated = std::make_shared<rotate_z_shape>( cn, -90_degrees );
        offset_shape sh( rotated, rl_vec3d{1.0, 0.0, 0.0} );
        CHECK( sh.signed_distance( rl_vec3d{1.0, 0.0, 0.0} ) == Approx( 0.0 ).margin( 0.001 ) );
        CHECK( sh.signed_distance( rl_vec3d{2.0, 0.0, 0.0} ) == Approx( 1.0 ).margin( 0.001 ) );
        CHECK( sh.signed_distance( rl_vec3d{0.0, 0.0, 0.0} ) == Approx( 0.0 ).margin( 0.001 ) );

        CHECK( sh.signed_distance( rl_vec3d{1.0, 1.0, 0.0} ) == Approx( 0.0 ).margin( 0.01 ) );
        CHECK( sh.signed_distance( rl_vec3d{1.0, 0.0, 1.0} ) == Approx( 0.0 ).margin( 0.01 ) );

        CHECK( sh.signed_distance( rl_vec3d{1.5, 0.0, 0.0} ) == Approx( 0.5 ).margin( 0.01 ) );
    }
}

TEST_CASE( "rotate_z_shape_test", "[shape]" )
{
    std::shared_ptr<shape_impl> s = std::make_shared<square_distance_from_zero_shape>();
    REQUIRE( s->signed_distance( rl_vec3d( 1.0, 0.0, 0.0 ) ) == Approx( 1.0 ) );
    SECTION( "rotate unit vector by multiples of 22.5 degrees" ) {
        for( int mult = -8; mult <= 8; mult++ ) {
            std::shared_ptr<shape_impl> r = std::make_shared<rotate_z_shape>( s, mult * 22.5_degrees );
            CHECK( r->signed_distance( rl_vec3d( 1.0, 0.0, 0.0 ) ) == Approx( 1.0 ) );
            CHECK( r->signed_distance( rl_vec3d( 0.0, 1.0, 0.0 ) ) == Approx( 1.0 ) );
            CHECK( r->signed_distance( rl_vec3d( 0.0, 0.0, 1.0 ) ) == Approx( 1.0 ) );
        }
    }
}

TEST_CASE( "cylinder_test", "[shape]" )
{
    constexpr double length = 1.0;
    constexpr double radius = 0.5;
    std::shared_ptr<shape_impl> cyl = std::make_shared<cylinder>( length, radius );
    SECTION( "just the cylinder" ) {
        const std::shared_ptr<shape_impl> &s = cyl;
        CHECK( s->signed_distance( rl_vec3d{0.0, 0.0, 0.0} ) == Approx( 0.0 ) );
        CHECK( s->signed_distance( rl_vec3d{0.0, -length, 0.0} ) == Approx( 0.0 ) );
        CHECK( s->signed_distance( rl_vec3d{radius, 0.0, 0.0} ) == Approx( 0.0 ) );
        CHECK( s->signed_distance( rl_vec3d{-radius, 0.0, 0.0} ) == Approx( 0.0 ) );
        CHECK( s->signed_distance( rl_vec3d{0.0, 0.0, radius} ) == Approx( 0.0 ) );
        CHECK( s->signed_distance( rl_vec3d{0.0, 0.0, -radius} ) == Approx( 0.0 ) );
        CHECK( s->signed_distance(
                   rl_vec3d{M_SQRT2 / 2.0 * radius, 0.0, M_SQRT2 / 2.0 * radius} ) == Approx( 0.0 ) );
    }

    SECTION( "offset by (radius, 2*length, 0)" ) {
        std::shared_ptr<shape_impl> s = std::make_shared<offset_shape>( cyl,
                                        rl_vec3d( radius, 2.0 * length, 0.0 ) );
        CHECK( s->signed_distance( rl_vec3d{0.0, 2.0, 0.0} ) == Approx( 0.0 ) );
        CHECK( s->signed_distance( rl_vec3d{0.0, 1.0, 0.0} ) == Approx( 0.0 ) );
        CHECK( s->signed_distance( rl_vec3d{1.0, 1.0, 0.0} ) == Approx( 0.0 ) );
        CHECK( s->signed_distance( rl_vec3d{0.5, 1.0, 0.5} ) == Approx( 0.0 ) );
    }

    SECTION( "rotated to point in positive x direction" ) {
        std::shared_ptr<shape_impl> s = std::make_shared<rotate_z_shape>( cyl, -90_degrees );
        CHECK( s->signed_distance( rl_vec3d{0.0, 0.0, 0.0} ) == Approx( 0.0 ) );
        CHECK( s->signed_distance( rl_vec3d{-length, 0.0, 0.0} ) == Approx( 0.0 ) );
        CHECK( s->signed_distance( rl_vec3d{0.0, radius, 0.0} ) == Approx( 0.0 ) );
        CHECK( s->signed_distance( rl_vec3d{0.0, 0.0, radius} ) == Approx( 0.0 ) );
    }
}

TEST_CASE( "cone_factory_test", "[shape]" )
{
    cone_factory c( 15_degrees, 10.0 );
    SECTION( "(0,0,0) to (5,5,0)" ) {
        std::shared_ptr<shape> s = c.create( rl_vec3d(), rl_vec3d( 5, 5, 0 ) );
        CHECK( s->distance_at( rl_vec3d( 0, 0, 0 ) ) > 0.0 );

        CHECK( s->distance_at( rl_vec3d( 1, 1, 0 ) ) < 0.0 );

        CHECK( s->distance_at( rl_vec3d( 1, 0, 0 ) ) > 0.0 );
        CHECK( s->distance_at( rl_vec3d( 0, 1, 0 ) ) > 0.0 );

        CHECK( s->distance_at( rl_vec3d( -1, 0, 0 ) ) > 0.0 );
        CHECK( s->distance_at( rl_vec3d( 0, -1, 0 ) ) > 0.0 );
        CHECK( s->distance_at( rl_vec3d( -1, -1, 0 ) ) > 0.0 );

        CHECK( s->distance_at( rl_vec3d( 1, -1, 0 ) ) > 0.0 );
        CHECK( s->distance_at( rl_vec3d( -1, 1, 0 ) ) > 0.0 );

        for( size_t i = 2; i <= 7; i++ ) {
            CHECK( s->distance_at( rl_vec3d( i, i, 0 ) ) < 0.0 );
        }
        for( size_t i = 8; i <= 10; i++ ) {
            CHECK( s->distance_at( rl_vec3d( i, i, 0 ) ) > 0.0 );
        }

        auto bb = s->bounding_box();
        CAPTURE( bb.p_min );
        CAPTURE( bb.p_max );
        for( size_t i = 0; i <= 5; i++ ) {
            CHECK( bb.contains( tripoint( i, i, 0 ) ) );
        }
    }

    SECTION( "(15,5,0) to (-15,5,0)" ) {
        std::shared_ptr<shape> s = c.create( rl_vec3d( 15, 5, 0 ), rl_vec3d( -15, 5, 0 ) );
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

static void shape_coverage_vs_distance_no_obstacle( const shape_factory_impl &c,
        const tripoint &origin, const tripoint &end )
{
    std::shared_ptr<shape> s = c.create( rl_vec3d( origin ), rl_vec3d( end ) );
    projectile p;
    p.impact = damage_instance();
    p.impact.add_damage( DT_STAB, 10 );
    auto cov = ranged::expected_coverage( *s, get_map(), 3 );

    map &here = get_map();
    auto bb = s->bounding_box();
    REQUIRE( bb.p_min != bb.p_max );
    bool had_any = false;
    CHECK( s->distance_at( rl_vec3d( origin ) ) > 0.0 );
    CHECK( cov[origin] <= 0.0 );
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
    cone_factory c( 15_degrees, 10.0 );
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
    cone_factory c( 22.5_degrees, 10.0 );
    const tripoint origin( 60, 60, 0 );
    const tripoint offset( 5, 5, 0 );
    const tripoint end = origin + offset;
    std::shared_ptr<shape> s = c.create( rl_vec3d( origin ), rl_vec3d( end ) );
    auto cov = ranged::expected_coverage( *s, get_map(), 3 );

    for( size_t i = 1; i <= 4; i++ ) {
        CHECK( cov[origin + point( i, i )] == 1.0 );
    }

    CHECK( cov[origin + point( 2, 1 )] == 1.0 );
    CHECK( cov[origin + point( 1, 2 )] == 1.0 );
}

TEST_CASE( "expected shape coverage through windows", "[shape]" )
{
    clear_map();
    cone_factory c( 22.5_degrees, 10.0 );
    const tripoint origin( 60, 60, 0 );
    const tripoint offset( 5, 0, 0 );
    const tripoint end = origin + offset;
    map &here = get_map();
    for( int wall_offset = -10; wall_offset <= 10; wall_offset++ ) {
        here.ter_set( tripoint( 62, 60 + wall_offset, 0 ), t_window );
    }

    std::shared_ptr<shape> s = c.create( rl_vec3d( origin ), rl_vec3d( end ) );
    auto cov = ranged::expected_coverage( *s, here, 3 );
    CHECK( cov[origin + point_east] == 1.0 );

    CHECK( cov[origin + 2 * point_east] == Approx( 0.25 ) );
    CHECK( cov[origin + 3 * point_east] == Approx( 0.25 ) );
    CHECK( cov[origin + 4 * point_east] == Approx( 0.25 ) );
}
