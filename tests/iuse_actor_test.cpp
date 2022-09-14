#include "catch/catch.hpp"

#include <climits>
#include <list>
#include <memory>

#include "avatar.h"
#include "game.h"
#include "inventory.h"
#include "item.h"
#include "monster.h"
#include "mtype.h"
#include "player.h"
#include "player_helpers.h"
#include "point.h"
#include "state_helpers.h"
#include "string_id.h"
#include "type_id.h"

static monster *find_adjacent_monster( const tripoint &pos )
{
    tripoint target = pos;
    for( target.x = pos.x - 1; target.x <= pos.x + 1; target.x++ ) {
        for( target.y = pos.y - 1; target.y <= pos.y + 1; target.y++ ) {
            if( target == pos ) {
                continue;
            }
            if( monster *const candidate = g->critter_at<monster>( target ) ) {
                return candidate;
            }
        }
    }
    return nullptr;
}

TEST_CASE( "manhack", "[iuse_actor][manhack]" )
{
    clear_all_state();
    player &dummy = g->u;

    g->clear_zombies();
    item &test_item = dummy.i_add( item( "bot_manhack", calendar::start_of_cataclysm,
                                         item::default_charges_tag{} ) );

    int test_item_pos = dummy.inv.position_by_item( &test_item );
    REQUIRE( test_item_pos != INT_MIN );

    monster *new_manhack = find_adjacent_monster( dummy.pos() );
    REQUIRE( new_manhack == nullptr );

    dummy.invoke_item( &test_item );

    test_item_pos = dummy.inv.position_by_item( &test_item );
    REQUIRE( test_item_pos == INT_MIN );

    new_manhack = find_adjacent_monster( dummy.pos() );
    REQUIRE( new_manhack != nullptr );
    REQUIRE( new_manhack->type->id == mtype_id( "mon_manhack" ) );
    g->clear_zombies();
}

