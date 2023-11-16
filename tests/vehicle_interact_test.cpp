#include "catch/catch.hpp"

#include <memory>
#include <string>
#include <vector>

#include "avatar.h"
#include "calendar.h"
#include "game.h"
#include "inventory.h"
#include "item.h"
#include "map.h"
#include "map_helpers.h"
#include "player_helpers.h"
#include "point.h"
#include "requirements.h"
#include "state_helpers.h"
#include "type_id.h"
#include "veh_type.h"
#include "vehicle.h"
#include "vehicle_part.h"

static void test_repair( std::vector<detached_ptr<item>> &tools, bool expect_craftable )
{

    const tripoint test_origin( 60, 60, 0 );
    g->u.setpos( test_origin );
    g->u.wear_item( item::spawn( "backpack" ), false );
    for( detached_ptr<item> &gear : tools ) {
        g->u.i_add( std::move( gear ) );
    }

    const tripoint vehicle_origin = test_origin + tripoint_south_east;
    vehicle *veh_ptr = get_map().add_vehicle( vproto_id( "bicycle" ), vehicle_origin, -90_degrees,
                       0, 0 );
    REQUIRE( veh_ptr != nullptr );
    // Find the frame at the origin.
    vehicle_part *origin_frame = nullptr;
    for( vehicle_part *part : veh_ptr->get_parts_at( vehicle_origin, "", part_status_flag::any ) ) {
        if( part->info().location == "structure" ) {
            origin_frame = part;
            break;
        }
    }
    REQUIRE( origin_frame != nullptr );
    REQUIRE( origin_frame->hp() == origin_frame->info().durability );
    veh_ptr->mod_hp( *origin_frame, -100 );
    REQUIRE( origin_frame->hp() < origin_frame->info().durability );

    const vpart_info &vp = origin_frame->info();
    // Assertions about frame part?

    requirement_data reqs = vp.repair_requirements();
    // Bust cache on crafting_inventory()
    g->u.mod_moves( 1 );
    inventory crafting_inv = g->u.crafting_inventory();
    bool can_repair = vp.repair_requirements().can_make_with_inventory( g->u.crafting_inventory(),
                      is_crafting_component );
    CHECK( can_repair == expect_craftable );
}

TEST_CASE( "repair_vehicle_part" )
{
    clear_all_state();
    const time_point bday = calendar::start_of_cataclysm;
    SECTION( "welder" ) {
        std::vector<detached_ptr<item>> tools;
        tools.push_back( item::spawn( "welder", bday, 500 ) );
        tools.push_back( item::spawn( "goggles_welding" ) );
        test_repair( tools, true );
    }
    SECTION( "UPS_modded_welder" ) {
        std::vector<detached_ptr<item>> tools;
        detached_ptr<item> welder = item::spawn( "welder", bday, 0 );
        welder->put_in( item::spawn( "battery_ups" ) );
        tools.push_back( std::move( welder ) );
        tools.push_back( item::spawn( "UPS_off", bday, 500 ) );
        tools.push_back( item::spawn( "goggles_welding" ) );
        test_repair( tools, true );
    }
    SECTION( "welder_missing_goggles" ) {
        std::vector<detached_ptr<item>> tools;
        tools.push_back( item::spawn( "welder", bday, 500 ) );
        test_repair( tools, false );
    }
    SECTION( "welder_missing_charge" ) {
        std::vector<detached_ptr<item>> tools;
        tools.push_back( item::spawn( "welder", bday, 5 ) );
        tools.push_back( item::spawn( "goggles_welding" ) );
        test_repair( tools, false );
    }
    SECTION( "UPS_modded_welder_missing_charges" ) {
        std::vector<detached_ptr<item>> tools;
        detached_ptr<item> welder = item::spawn( "welder", bday, 0 );
        welder->put_in( item::spawn( "battery_ups" ) );
        tools.push_back( std::move( welder ) );
        tools.push_back( item::spawn( "UPS_off", bday, 5 ) );
        tools.push_back( item::spawn( "goggles_welding" ) );
        test_repair( tools, false );
    }
}
