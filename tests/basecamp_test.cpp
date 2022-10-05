#include "avatar.h"
#include "basecamp.h"
#include "calendar.h"
#include "clzones.h"
#include "faction.h"
#include "game.h"
#include "game_constants.h"
#include "map.h"
#include "overmapbuffer.h"
#include "point.h"

#include "catch/catch.hpp"
#include "map_helpers.h"
#include "state_helpers.h"

enum class canned_status {
    without_can,
    canned_sealed,
    canned_unsealed
};

static const itype_id meat_cooked_id( "meat_cooked" );
static const itype_id sealed_can_id( "can_medium" );
static const itype_id unsealed_can_id( "can_medium_unsealed" );

static item make_food( const itype_id &inner_food_id, int amount, canned_status canned,
                       bool is_rotten )
{
    const time_point time_of_making = is_rotten ? calendar::turn_zero : calendar::turn;
    item food( inner_food_id, time_of_making, amount );

    item can;
    switch( canned ) {
        case canned_status::without_can:
            return food;
        case canned_status::canned_sealed:
            can = item( sealed_can_id, calendar::turn_zero );
            break;
        case canned_status::canned_unsealed:
            can = item( unsealed_can_id, calendar::turn_zero );
            break;
    }
    REQUIRE( can.contents.insert_item( food ).success() );
    return can;
}

static int rounded_int( double val )
{
    return static_cast<int>( std::round( val ) );
}

TEST_CASE( "distribute_food" )
{
    clear_all_state();
    constexpr tripoint origin( 60, 60, 0 );
    constexpr tripoint thirty_steps_rd = tripoint( 30, 30, 0 );
    g->u.setpos( origin );
    const tripoint_abs_omt omt_pos = g->u.global_omt_location();
    g->m.add_camp( omt_pos, "faction_camp_for_distribute" );
    basecamp *bcp = overmap_buffer.find_camp( omt_pos.xy() ).value();
    bcp->set_bb_pos( origin + tripoint_east );
    zone_manager &zmgr = zone_manager::get_manager();
    const faction *yours = g->u.get_faction();
    zmgr.add( "Zone for dumping food", zone_type_id( "CAMP_FOOD" ),
              g->u.get_faction()->id, false, true,
              origin - thirty_steps_rd,
              origin + thirty_steps_rd );
    zmgr.add( "Storage zone", zone_type_id( "CAMP_STORAGE" ),
              g->u.get_faction()->id, false, true,
              origin - thirty_steps_rd,
              origin + thirty_steps_rd );

    calendar::turn = calendar::turn_zero + 365_days * 5;

    constexpr int kcal_in_meat = 402;

    SECTION( "Cooked meat without can is consumed" ) {
        const int previous_kcal = yours->food_supply;
        g->m.add_item_or_charges( origin, make_food( meat_cooked_id, 2, canned_status::without_can, false ),
                                  false );
        bcp->distribute_food();
        CHECK( yours->food_supply - previous_kcal == rounded_int( 0.6 * 2 * kcal_in_meat ) );
        CHECK( g->m.i_at( origin ).empty() );
    }

    SECTION( "Jerky is consumed and no calories wasted" ) {
        const int previous_kcal = yours->food_supply;
        g->m.add_item_or_charges( origin, make_food( itype_id( "jerky" ), 50, canned_status::without_can,
                                  false ),
                                  false );
        bcp->distribute_food();
        constexpr int kcal_in_jerky = 348;
        CHECK( yours->food_supply - previous_kcal == 50 * kcal_in_jerky );
        CHECK( g->m.i_at( origin ).empty() );
    }

    SECTION( "Rotten food is not consumed" ) {
        const int previous_kcal = yours->food_supply;
        g->m.add_item_or_charges( origin, make_food( meat_cooked_id, 1, canned_status::without_can, true ),
                                  false );
        bcp->distribute_food();
        CHECK( yours->food_supply == previous_kcal );
        const map_stack stack = g->m.i_at( origin );
        CHECK( stack.size() == 1 );
        CHECK( std::all_of( stack.begin(), stack.end(),
        []( const item & it ) {
            return it.typeId() == meat_cooked_id && it.count() == 1 && it.rotten();
        } ) );
        g->m.i_clear( origin );
    }

    SECTION( "Canned edible food consumed leaving opened can" ) {
        const int previous_kcal = yours->food_supply;
        g->m.add_item_or_charges( origin, make_food( meat_cooked_id, 3, canned_status::canned_sealed,
                                  false ),
                                  false );
        bcp->distribute_food();
        CHECK( yours->food_supply - previous_kcal == 3 * kcal_in_meat );
        const map_stack stack = g->m.i_at( origin );
        CHECK( stack.size() == 1 );
        // Should be unsealed
        CHECK( std::all_of( stack.begin(), stack.end(),
        []( const item & it ) {
            return it.typeId() == unsealed_can_id && it.count() == 1 && it.contents.empty();
        } ) );
        g->m.i_clear( origin );
    }

    SECTION( "Open edible food consumed leaving opened can" ) {
        const int previous_kcal = yours->food_supply;
        g->m.add_item_or_charges( origin, make_food( meat_cooked_id, 10, canned_status::canned_unsealed,
                                  false ),
                                  false );
        bcp->distribute_food();
        CHECK( yours->food_supply - previous_kcal == rounded_int( 10 * kcal_in_meat * 0.6 ) );
        const map_stack stack = g->m.i_at( origin );
        CHECK( stack.size() == 1 );
        CHECK( std::all_of( stack.begin(), stack.end(),
        []( const item & it ) {
            return it.typeId() == unsealed_can_id && it.count() == 1 && it.contents.empty();
        } ) );
        g->m.i_clear( origin );
    }

    SECTION( "Unwanted food still remains in same can" ) {
        for( const canned_status status : {
                 canned_status::canned_sealed, canned_status::canned_unsealed
             } ) {
            const int previous_kcal = yours->food_supply;
            const item can_of_spam = make_food( itype_id( "can_spam" ), 1, status, false );
            g->m.add_item_or_charges( origin, can_of_spam, false );
            bcp->distribute_food();
            CHECK( yours->food_supply == previous_kcal );
            const map_stack stack = g->m.i_at( origin );
            CHECK( stack.size() == 1 );
            CHECK( std::all_of( stack.begin(), stack.end(),
            [&can_of_spam]( const item & it ) {
                return it.typeId() == can_of_spam.typeId() && it.count() == 1
                       && it.contents.front().typeId() == can_of_spam.contents.front().typeId()
                       && it.contents.front().count() == can_of_spam.contents.front().count();
            } ) );
            g->m.i_clear( origin );
        }
    }

    SECTION( "Not food remains as is" ) {
        const int previous_kcal = yours->food_supply;
        item it( "2x4" );
        g->m.add_item_or_charges( origin, it );
        bcp->distribute_food();
        CHECK( yours->food_supply == previous_kcal );
        const map_stack stack = g->m.i_at( origin );
        CHECK( stack.size() == 1 );
        CHECK( std::all_of( stack.begin(), stack.end(),
        [&it]( const item & i ) {
            return i.typeId() == it.typeId() && i.count() == 1 && i.age() == it.age();
        } ) );
        g->m.i_clear( origin );
    }

    SECTION( "And even bleach remains as is" ) {
        const int previous_kcal = yours->food_supply;
        // It is not food but I reuse container putting
        item bleach = make_food( itype_id( "bleach" ), 5, canned_status::canned_sealed, false );
        g->m.add_item_or_charges( origin, bleach );
        bcp->distribute_food();
        CHECK( yours->food_supply == previous_kcal );
        const map_stack stack = g->m.i_at( origin );
        CHECK( stack.size() == 1 );
        CHECK( std::all_of( stack.begin(), stack.end(),
        [&bleach]( const item & i ) {
            return i.typeId() == bleach.typeId()
                   && bleach.count() == 1 && i.age() == bleach.age()
                   && i.contents.front().typeId() == bleach.contents.front().typeId()
                   && i.contents.front().count() == bleach.contents.front().count()
                   ;
        } ) );
        g->m.i_clear( origin );
    }
}
