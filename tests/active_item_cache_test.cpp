#include "catch/catch.hpp"

#include <memory>
#include <set>

#include "calendar.h"
#include "game.h"
#include "game_constants.h"
#include "item.h"
#include "map.h"
#include "point.h"
#include "state_helpers.h"

TEST_CASE( "place_active_item_at_various_coordinates", "[item]" )
{
    clear_all_state();
    for( int z = -OVERMAP_DEPTH; z < OVERMAP_HEIGHT; ++z ) {
        for( int x = 0; x < MAPSIZE_X; ++x ) {
            for( int y = 0; y < MAPSIZE_Y; ++y ) {
                get_map().i_clear( { x, y, z } );
            }
        }
    }
    REQUIRE( get_map().get_submaps_with_active_items().empty() );
    // An arbitrary active item.
    item &active = *item::spawn_temporary( "firecracker_act", calendar::start_of_cataclysm,
                                           item::default_charges_tag() );
    active.activate();

    // For each space in a wide area place the item and check if the cache has been updated.
    int z = 0;
    for( int x = 0; x < MAPSIZE_X; ++x ) {
        for( int y = 0; y < MAPSIZE_Y; ++y ) {
            REQUIRE( get_map().i_at( { x, y, z } ).empty() );
            CAPTURE( x, y, z );
            tripoint abs_loc = get_map().get_abs_sub() + tripoint( x / SEEX, y / SEEY, z );
            CAPTURE( abs_loc.x, abs_loc.y, abs_loc.z );
            REQUIRE( get_map().get_submaps_with_active_items().empty() );
            REQUIRE( get_map().get_submaps_with_active_items().find( abs_loc ) ==
                     get_map().get_submaps_with_active_items().end() );
            detached_ptr<item> n = item::spawn( active );
            item &item_ref = *n;
            get_map().add_item( { x, y, z }, std::move( n ) );
            REQUIRE( item_ref.active );
            REQUIRE_FALSE( get_map().get_submaps_with_active_items().empty() );
            REQUIRE( get_map().get_submaps_with_active_items().find( abs_loc ) !=
                     get_map().get_submaps_with_active_items().end() );
            REQUIRE_FALSE( get_map().i_at( { x, y, z } ).empty() );
            get_map().i_clear( { x, y, z } );
        }
    }
}
