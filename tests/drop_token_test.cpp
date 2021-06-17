#include <algorithm>
#include <climits>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "activity_handlers.h"
#include "avatar.h"
#include "calendar.h"
#include "catch/catch.hpp"
#include "drop_token.h"
#include "item.h"
#include "player_helpers.h"

class inventory;
struct act_item;

std::list<act_item> reorder_for_dropping( Character &p, const drop_locations &drop );

struct act_item {
    /// inventory item
    item_location loc;
    /// How many items need to be processed
    int count;
    /// Amount of moves that processing will consume
    int consumed_moves;

    act_item( const item_location &loc, int count, int consumed_moves )
        : loc( loc ),
          count( count ),
          consumed_moves( consumed_moves ) {}
};

TEST_CASE( "full backpack drop", "[activity][drop_token]" )
{
    const itype_id backpack_id = "backpack";
    avatar dummy;
    item an_item( "bottle_glass" );

    GIVEN( "a character with a backpack full of items and no other containers" ) {
        REQUIRE( dummy.wear_item( item( backpack_id ), false ) );
        while( dummy.can_pickWeight( an_item, true ) && dummy.can_pickVolume( an_item ) ) {
            dummy.i_add( an_item );
        }

        REQUIRE( dummy.worn.size() == 1 );
        REQUIRE( dummy.inv.size() >= 1 );

        WHEN( "he considers dropping items" ) {
            drop_locations drop;
            drop.push_back( drop_location( item_location( dummy, &dummy.worn.front() ), 1 ) );
            std::list<act_item> drop_list = reorder_for_dropping( dummy, drop );
            THEN( "he will try to drop all carried items" ) {
                // TODO: Check that all items will be dropped. inv.size() doesn't work because stacks
                AND_THEN( "all of them will have identical drop tokens, marking the backpack as parent" ) {
                    item_drop_token first_token = *drop_list.front().loc->drop_token;
                    for( const act_item &ait : drop_list ) {
                        CHECK( *ait.loc->drop_token == first_token );
                    }
                }
                AND_THEN( "the backpack will have non-zero drop cost, while all contents will have zero drop cost" ) {
                    REQUIRE( drop_list.front().loc->typeId() == backpack_id );
                    auto iter = drop_list.begin();
                    CHECK( iter->consumed_moves > 0 );
                    iter++;
                    while( iter != drop_list.end() ) {
                        CHECK( iter->consumed_moves == 0 );
                        iter++;
                    }
                }
            }
        }
    }
}
