#include "catch/catch.hpp"

#include "avatar.h"
#include "calendar.h"
#include "morale.h"
#include "morale_types.h"

TEST_CASE( "player_morale_from_effects_consistent", "[morale][effect][slow]" )
{
    avatar dummy;
    dummy.add_effect( efftype_id( "test_high" ), 1_minutes );
    // Why the fuck is normalize needed here? set_body should be in constructor
    dummy.normalize();
    const time_point start = calendar::turn_zero;
    const time_point end = start + 2_minutes;
    const time_duration tick_size = 1_turns;
    // As in game::do_turn
    for( time_point t = start; t < end; t += tick_size ) {
        dummy.process_turn();
        if( to_turns<int>( t - start ) % to_turns<int>( 10_turns ) == 0 ) {
            dummy.update_morale();
        }
        if( to_turns<int>( t - start ) % to_turns<int>( 9_turns ) == 0 ) {
            REQUIRE( dummy.check_and_recover_morale() );
        }
    }
}
