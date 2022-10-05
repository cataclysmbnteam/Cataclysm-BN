#include "catch/catch.hpp"

#include <vector>

#include "npc.h"
#include "item.h"
#include "ranged.h"
#include "state_helpers.h"

static constexpr tripoint shooter_pos( 60, 60, 0 );
static const std::string flag_BIPOD( "BIPOD" );

static void check_burst_penalty( const Character &shooter, item gun, int expected,
                                 bool bipod = false )
{
    if( gun.ammo_required() && !gun.ammo_sufficient() && !gun.ammo_default().is_null() ) {
        gun.ammo_set( gun.ammo_default(), -1 );
    }
    if( bipod ) {
        const std::vector<item *> mods = gun.gunmods();
        bool has_bipod = std::any_of( mods.begin(), mods.end(), []( const item * it ) {
            return it->has_flag( flag_BIPOD );
        } ) ;
        REQUIRE( has_bipod );
    }

    int penalty = ranged::burst_penalty( shooter, gun, gun.gun_recoil( bipod ) );
    CAPTURE( gun.tname() );
    int dev = expected / 5;
    CHECK( penalty >= expected - dev );
    CHECK( penalty <= expected + dev );
}

static void check_burst_penalty( const Character &shooter, const std::string &gun_type,
                                 const std::vector<std::string> &mods, int expected, bool bipod = false )
{
    itype_id gun_id( gun_type );
    item gun( gun_id );
    for( const std::string &mod_type : mods ) {
        CAPTURE( gun_type );
        CAPTURE( mod_type );
        itype_id mod_id( mod_type );
        item mod( mod_id );
        REQUIRE( gun.is_gunmod_compatible( mod ).success() );
        gun.put_in( mod );
    }
    check_burst_penalty( shooter, gun, expected, bipod );
}

static void check_burst_penalty( const Character &shooter, const std::string &gun_type,
                                 int expected, bool bipod = false )
{
    check_burst_penalty( shooter, gun_type, {}, expected, bipod );
}

TEST_CASE( "unskilled_burst_no_mods", "[ranged] [balance]" )
{
    clear_all_state();
    standard_npc shooter( "Shooter", shooter_pos, {}, 0, 8, 8, 8, 8 );
    // .22 SMG - the lightest burst (from a firearm) expected to be in the game
    check_burst_penalty( shooter, "american_180", 0 );
    // 9mm SMG - should be manageable
    check_burst_penalty( shooter, "calico", 60 );
    // .223 machine gun - should have lower penalty than a rifle of the same caliber
    check_burst_penalty( shooter, "m249", 250 );
    // .223 rifle
    check_burst_penalty( shooter, "m4a1", 600 );
    // 7.62 rifle
    check_burst_penalty( shooter, "ak47", 700 );
    // .50 machine gun - heaviest expected burst fire
    check_burst_penalty( shooter, "m2browning", 800 );
}

TEST_CASE( "average_burst_no_mods", "[ranged] [balance]" )
{
    clear_all_state();
    standard_npc shooter( "Shooter", shooter_pos, {}, 5, 10, 8, 8, 8 );
    check_burst_penalty( shooter, "american_180", 0 );
    check_burst_penalty( shooter, "calico", 25 );
    check_burst_penalty( shooter, "m249", 90 );
    check_burst_penalty( shooter, "m4a1", 220 );
    check_burst_penalty( shooter, "ak47", 270 );
    check_burst_penalty( shooter, "m2browning", 400 );
}

// Near the best achievable by an unmodified human
TEST_CASE( "great_burst_no_mods", "[ranged] [balance]" )
{
    clear_all_state();
    standard_npc shooter( "Shooter", shooter_pos, {}, 10, 14, 14, 14, 14 );
    check_burst_penalty( shooter, "american_180", 0 );
    check_burst_penalty( shooter, "calico", 4 );
    check_burst_penalty( shooter, "m249", 50 );
    check_burst_penalty( shooter, "m4a1", 120 );
    check_burst_penalty( shooter, "ak47", 150 );
    check_burst_penalty( shooter, "m2browning", 230 );
}

TEST_CASE( "average_burst_bipod", "[ranged] [balance]" )
{
    clear_all_state();
    standard_npc shooter( "Shooter", shooter_pos, {}, 5, 10, 8, 8, 8 );
    check_burst_penalty( shooter, "m249", {}, 40, true );
    check_burst_penalty( shooter, "m240", {}, 90, true );
    check_burst_penalty( shooter, "m2browning", {"underbarrel_mount", "bipod"}, 160, true );
    check_burst_penalty( shooter, "m1918", {"bipod"}, 160, true );
}

TEST_CASE( "average_burst_modded", "[ranged] [balance]" )
{
    clear_all_state();
    const std::vector<std::string> modset = {"adjustable_stock", "suppressor", "pistol_grip", "grip_mod"};
    standard_npc shooter( "Shooter", shooter_pos, {}, 5, 10, 8, 8, 8 );
    check_burst_penalty( shooter, "american_180", modset, 0 );
    check_burst_penalty( shooter, "calico", modset, 5 );
    check_burst_penalty( shooter, "m249", {"suppressor"}, 90 );
    check_burst_penalty( shooter, "m4a1", modset, 100 );
    check_burst_penalty( shooter, "ak47", {"adjustable_stock", "suppressor", "pistol_grip"}, 170 );
    check_burst_penalty( shooter, "m2browning", {"suppressor"}, 320 );
}
