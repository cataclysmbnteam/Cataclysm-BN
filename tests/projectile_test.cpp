#include "catch/catch.hpp"

#include <memory>
#include <set>
#include <vector>

#include "avatar.h"
#include "ballistics.h"
#include "damage.h"
#include "dispersion.h"
#include "game.h"
#include "item.h"
#include "map.h"
#include "map_helpers.h"
#include "point.h"
#include "projectile.h"
#include "state_helpers.h"
#include "type_id.h"

static tripoint projectile_end_point( const std::vector<tripoint> &range, const item &gun,
                                      int speed, int proj_range )
{
    projectile test_proj;
    test_proj.speed = speed;
    test_proj.range = proj_range;
    test_proj.impact = gun.gun_damage();
    for( const ammo_effect_str_id &ae_id : gun.ammo_effects() ) {
        test_proj.add_effect( ae_id );
    }

    dealt_projectile_attack attack;

    attack = projectile_attack( test_proj, range[0], range[2], dispersion_sources(), &get_avatar(),
                                nullptr );

    return attack.end_point;
}

TEST_CASE( "projectiles_through_obstacles", "[projectile]" )
{
    clear_all_state();
    // Move the player out of the way of the test area
    get_avatar().setpos( { 2, 2, 0 } );

    map &here = get_map();
    // Ensure that a projectile fired from a gun can pass through a chain link fence
    // First, set up a test area - three tiles in a row
    // One on either side clear, with a chainlink fence in the middle
    std::vector<tripoint> range = { tripoint_zero, tripoint_east, tripoint( 2, 0, 0 ) };
    for( const tripoint &pt : range ) {
        REQUIRE( here.inbounds( pt ) );
        here.ter_set( pt, ter_id( "t_dirt" ) );
        here.furn_set( pt, furn_id( "f_null" ) );
        REQUIRE_FALSE( g->critter_at( pt ) );
        REQUIRE( here.is_transparent( pt ) );
    }

    // Set an obstacle in the way, a chain fence
    here.ter_set( range[1], ter_id( "t_chainfence" ) );

    // Create a gun to fire a projectile from
    detached_ptr<item> gun = item::spawn( itype_id( "m1a" ) );
    gun->ammo_set( itype_id( "308" ), 5 );

    // Check that a bullet with the correct amount of speed can through obstacles
    CHECK( projectile_end_point( range, *gun, 1000, 3 ) == range[2] );

    // But that a bullet without the correct amount cannot
    CHECK( projectile_end_point( range, *gun, 10, 3 ) == range[0] );
}
