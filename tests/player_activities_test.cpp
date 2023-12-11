#include "catch/catch.hpp"
#include "map_helpers.h"
#include "player_helpers.h"

#include "activity_actor_definitions.h"
#include "avatar.h"
#include "calendar.h"
#include "character.h"
#include "flag.h"
#include "game.h"
#include "itype.h"
#include "iuse_actor.h"
#include "map.h"
#include "point.h"
#include "player_activity.h"

static const activity_id ACT_NULL( "ACT_NULL" );
static const activity_id ACT_BOLTCUTTING( "ACT_BOLTCUTTING" );

static const furn_str_id furn_t_test_f_boltcut1( "test_f_boltcut1" );
static const furn_str_id furn_t_test_f_boltcut2( "test_f_boltcut2" );
static const furn_str_id furn_t_test_f_boltcut3( "test_f_boltcut3" );

static const itype_id itype_test_boltcutter( "test_boltcutter" );
static const itype_id itype_test_boltcutter_elec( "test_boltcutter_elec" );

static const ter_str_id ter_test_t_boltcut1( "test_t_boltcut1" );
static const ter_str_id ter_test_t_boltcut2( "test_t_boltcut2" );

TEST_CASE( "boltcut", "[activity][boltcut]" )
{
    map &mp = get_map();
    avatar &dummy = get_avatar();

    auto setup_dummy = [&dummy]() -> item & {
        item &loc = dummy.i_add( item::spawn( itype_test_boltcutter ) );
        dummy.wield( loc );

        REQUIRE( dummy.primary_weapon().typeId() == itype_test_boltcutter );

        return loc;
    };

    auto setup_activity = [&dummy]( item & cutter ) -> void {
        std::unique_ptr<boltcutting_activity_actor> act = std::make_unique<boltcutting_activity_actor>(
            tripoint_zero, safe_reference<item>( cutter )
        );
        act->testing = true;
        dummy.assign_activity( std::make_unique<player_activity>( std::move( act ) ) );
    };

    SECTION( "boltcut start checks" ) {
        GIVEN( "a tripoint with nothing" ) {
            clear_map();
            clear_avatar();

            mp.ter_set( tripoint_zero, t_null );
            REQUIRE( mp.ter( tripoint_zero ) == t_null );

            item &boltcutter = setup_dummy();
            setup_activity( boltcutter );

            THEN( "boltcutting activity can't start" ) {
                CHECK( dummy.activity->id() == ACT_NULL );
            }
        }

        GIVEN( "a tripoint with invalid terrain" ) {
            clear_map();
            clear_avatar();

            mp.ter_set( tripoint_zero, t_dirt );
            REQUIRE( mp.ter( tripoint_zero ) == t_dirt );

            item &boltcutter = setup_dummy();
            setup_activity( boltcutter );

            THEN( "boltcutting activity can't start" ) {
                CHECK( dummy.activity->id() == ACT_NULL );
            }
        }

        GIVEN( "a tripoint with valid terrain" ) {
            clear_map();
            clear_avatar();

            mp.ter_set( tripoint_zero, ter_test_t_boltcut1 );
            REQUIRE( mp.ter( tripoint_zero ) == ter_test_t_boltcut1 );

            item &boltcutter = setup_dummy();
            setup_activity( boltcutter );

            THEN( "boltcutting activity can start" ) {
                CHECK( dummy.activity->id() == ACT_BOLTCUTTING );
            }
        }

        GIVEN( "a tripoint with valid furniture" ) {
            clear_map();
            clear_avatar();

            mp.furn_set( tripoint_zero, furn_t_test_f_boltcut1 );
            REQUIRE( mp.furn( tripoint_zero ) == furn_t_test_f_boltcut1 );

            item &boltcutter = setup_dummy();
            setup_activity( boltcutter );

            THEN( "boltcutting activity can start" ) {
                CHECK( dummy.activity->id() == ACT_BOLTCUTTING );
            }
        }

        GIVEN( "a tripoint with valid terrain" ) {
            clear_map();
            clear_avatar();

            mp.ter_set( tripoint_zero, ter_test_t_boltcut1 );
            REQUIRE( mp.ter( tripoint_zero ) == ter_test_t_boltcut1 );

            item &boltcutter = setup_dummy();
            setup_activity( boltcutter );
            REQUIRE( dummy.activity->id() == ACT_BOLTCUTTING );

            WHEN( "terrain has a duration of 10 seconds" ) {
                REQUIRE( ter_test_t_boltcut1->boltcut->duration() == 10_seconds );
                THEN( "moves_left is equal to 10 seconds" ) {
                    CHECK( dummy.activity->moves_left == to_moves<int>( 10_seconds ) );
                }
            }
        }

        GIVEN( "a tripoint with valid furniture" ) {
            clear_map();
            clear_avatar();

            mp.furn_set( tripoint_zero, furn_t_test_f_boltcut1 );
            REQUIRE( mp.furn( tripoint_zero ) == furn_t_test_f_boltcut1 );

            item &boltcutter = setup_dummy();
            setup_activity( boltcutter );
            REQUIRE( dummy.activity->id() == ACT_BOLTCUTTING );

            WHEN( "furniture has a duration of 5 seconds" ) {
                REQUIRE( furn_t_test_f_boltcut1->boltcut->duration() == 5_seconds );
                THEN( "moves_left is equal to 5 seconds" ) {
                    CHECK( dummy.activity->moves_left == to_moves<int>( 5_seconds ) );
                }
            }
        }
    }

    SECTION( "boltcut turn checks" ) {
        GIVEN( "player is in mid activity" ) {
            clear_map();
            clear_avatar();

            mp.furn_set( tripoint_zero, furn_t_test_f_boltcut3 );
            REQUIRE( mp.furn( tripoint_zero ) == furn_t_test_f_boltcut3 );

            item &loc = dummy.i_add( item::spawn( itype_test_boltcutter_elec, calendar::start_of_cataclysm,
                                                  2 ) );
            dummy.wield( loc );

            REQUIRE( dummy.primary_weapon().typeId() == itype_test_boltcutter_elec );

            setup_activity( loc );
            REQUIRE( dummy.activity->id() == ACT_BOLTCUTTING );
            process_activity( dummy );

            WHEN( "player runs out of charges" ) {
                REQUIRE( dummy.activity->id() == ACT_NULL );

                THEN( "player recharges with fuel" ) {
                    loc.ammo_set( loc.ammo_default(), -1 );

                    AND_THEN( "player can resume the activity" ) {
                        setup_activity( loc );
                        dummy.moves = dummy.get_speed();
                        dummy.activity->do_turn( dummy );
                        CHECK( dummy.activity->id() == ACT_BOLTCUTTING );
                        CHECK( dummy.activity->moves_left < to_moves<int>( furn_t_test_f_boltcut3->boltcut->duration() ) );
                    }
                }
            }
        }
    }

    SECTION( "boltcut finish checks" ) {
        GIVEN( "a tripoint with valid terrain" ) {
            clear_map();
            clear_avatar();

            mp.ter_set( tripoint_zero, ter_test_t_boltcut1 );
            REQUIRE( mp.ter( tripoint_zero ) == ter_test_t_boltcut1 );

            item &boltcutter = setup_dummy();
            setup_activity( boltcutter );

            REQUIRE( dummy.activity->id() == ACT_BOLTCUTTING );
            process_activity( dummy );
            REQUIRE( dummy.activity->id() == ACT_NULL );

            THEN( "terrain gets converted to new terrain type" ) {
                CHECK( mp.ter( tripoint_zero ) == t_dirt );
            }
        }

        GIVEN( "a tripoint with valid furniture" ) {
            clear_map();
            clear_avatar();

            mp.furn_set( tripoint_zero, furn_t_test_f_boltcut1 );
            REQUIRE( mp.furn( tripoint_zero ) == furn_t_test_f_boltcut1 );

            item &boltcutter = setup_dummy();
            setup_activity( boltcutter );

            REQUIRE( dummy.activity->id() == ACT_BOLTCUTTING );
            process_activity( dummy );
            REQUIRE( dummy.activity->id() == ACT_NULL );

            THEN( "furniture gets converted to new furniture type" ) {
                CHECK( mp.furn( tripoint_zero ) == f_null );
            }
        }

        GIVEN( "a tripoint with valid furniture" ) {
            clear_map();
            clear_avatar();

            mp.furn_set( tripoint_zero, furn_t_test_f_boltcut2 );
            REQUIRE( mp.furn( tripoint_zero ) == furn_t_test_f_boltcut2 );

            item &boltcutter = setup_dummy();
            setup_activity( boltcutter );

            REQUIRE( dummy.activity->id() == ACT_BOLTCUTTING );
            process_activity( dummy );
            REQUIRE( dummy.activity->id() == ACT_NULL );

            THEN( "furniture gets converted to new furniture type" ) {
                CHECK( mp.furn( tripoint_zero ) == furn_t_test_f_boltcut1 );
            }
        }


        GIVEN( "a tripoint with a valid furniture with byproducts" ) {
            clear_map();
            clear_avatar();

            mp.ter_set( tripoint_zero, ter_test_t_boltcut2 );
            REQUIRE( mp.ter( tripoint_zero ) == ter_test_t_boltcut2 );

            item &boltcutter = setup_dummy();
            setup_activity( boltcutter );

            REQUIRE( ter_test_t_boltcut2->boltcut->byproducts().size() == 2 );

            REQUIRE( dummy.activity->id() == ACT_BOLTCUTTING );
            process_activity( dummy );
            REQUIRE( dummy.activity->id() == ACT_NULL );

            const itype_id test_amount( "test_rock" );
            const itype_id test_random( "test_2x4" );

            WHEN( "boltcut acitivy finishes" ) {
                CHECK( dummy.activity->id() == ACT_NULL );

                THEN( "player receives the items" ) {
                    int count_amount = 0;
                    int count_random = 0;
                    for( const auto &it : get_map().i_at( tripoint_zero ) ) {
                        // can't use switch here
                        const itype_id it_id = it->typeId();
                        if( it_id == test_amount ) {
                            count_amount += it->charges;
                        } else if( it_id == test_random ) {
                            count_random += 1;
                        }
                    }

                    CHECK( count_amount == 3 );
                    CHECK( ( 7 <= count_random && count_random <= 9 ) );
                }
            }
        }
    }
}
