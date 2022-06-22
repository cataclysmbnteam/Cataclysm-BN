#include "catch/catch.hpp"

#include <list>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "avatar.h"
#include "game.h"
#include "inventory.h"
#include "item.h"
#include "item_location.h"
#include "map.h"
#include "map_helpers.h"
#include "type_id.h"
#include "vehicle.h"
#include "vehicle_selector.h"
#include "veh_type.h"

TEST_CASE( "reload_on_vehicle_cargo", "[magazine] [visitable] [item] [item_location]" )
{
    const tripoint vehicle_center = tripoint( 65, 65, 0 );
    clear_map_and_put_player_underground();

    map &here = get_map();
    const vproto_id car_id( "car" );
    const itype_id ups_id( "UPS_off" );
    vehicle *veh = here.add_vehicle( car_id, vehicle_center, 0_radians, 0, 0, false );
    REQUIRE( veh != nullptr );

    item ups( ups_id );
    int part_num = veh->part_with_feature( 0, VPFLAG_CARGO, true );
    REQUIRE( part_num >= 0 );
    auto stack_iter_opt = veh->add_item( part_num, ups );
    REQUIRE( stack_iter_opt );

    vehicle_cursor vc = vehicle_cursor( *veh, static_cast<std::ptrdiff_t>( part_num ) );

    const item *it = nullptr;
    vc.visit_items( [&it, &ups_id]( const item * e ) {
        if( e->typeId() == ups_id ) {
            it = e;
            return VisitResponse::ABORT;
        }
        return VisitResponse::NEXT;
    } );
    REQUIRE( it != nullptr );

    item_location item_on_vehicle = item_location( vc, const_cast<item *>( it ) );
    REQUIRE( item_on_vehicle );

    g->reload( item_on_vehicle, false, false );
}
