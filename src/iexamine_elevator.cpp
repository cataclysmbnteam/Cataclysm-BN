#include "game.h"
#include "iexamine.h"
#include "mapdata.h"
#include "output.h"
#include "omdata.h"
#include "overmapbuffer.h"
#include "player.h"
#include "coordinates.h"
#include "map.h"
#include "point.h"
#include "ui.h"
#include "vpart_range.h"
#include "point_rotate.h"

namespace
{

using elevator_tiles = std::vector<tripoint>;
auto get_elevator_here( const Character &you ) -> elevator_tiles
{
    const auto &here = get_map();
    const auto &points = closest_points_first( you.pos(), SEEX - 1 );

    elevator_tiles tiles;
    std::copy_if( points.begin(), points.end(), std::back_inserter( tiles ),
    [&here]( const tripoint & pos ) {
        return here.has_flag( TFLAG_ELEVATOR, pos );
    } );

    return tiles;
}

auto get_elevator_dest( const elevator_tiles &elevator_here,
                        const tripoint &sm_orig,
                        int turns,
                        int movez ) -> elevator_tiles
{
    elevator_tiles tiles;
    std::transform( elevator_here.begin(), elevator_here.end(), std::back_inserter( tiles ),
    [turns, &sm_orig, movez]( tripoint const & p ) {
        return rotate_point_sm( { p.xy(), movez }, sm_orig, turns );
    } );

    return tiles;
}

auto choose_elevator_destz( const tripoint &examp, const tripoint_abs_omt &this_omt,
                            const tripoint &sm_orig ) -> int
{
    const auto this_floor = _( " (this floor)" );

    map &here = get_map();
    uilist choice;
    choice.title = _( "Select destination floor" );
    for( int z = OVERMAP_HEIGHT; z >= -OVERMAP_DEPTH; z-- ) {
        const tripoint_abs_omt that_omt{ this_omt.xy(), z };
        const int delta = get_rot_delta( this_omt, that_omt );
        const tripoint zp =
            rotate_point_sm( { examp.xy(), z }, sm_orig, delta );

        if( here.ter( zp )->examine == &iexamine::elevator ) {
            const std::string omt_name = overmap_buffer.ter_existing( that_omt )->get_name();
            const auto floor_name = z == examp.z ? this_floor : "";
            const std::string name = string_format( "%3iF %s%s", z, omt_name, floor_name );

            choice.addentry( z, z != examp.z, MENU_AUTOASSIGN, name );
        }
    }
    choice.query();
    return choice.ret;
}

} // namespace

namespace
{

enum class overlap_status { outside, inside, overlap };

auto vehicle_status( const wrapped_vehicle &veh, const elevator_tiles &tiles ) -> overlap_status
{
    const auto &ps = veh.v->get_all_parts();
    const int all_vparts_count = ps.part_count();
    const int vparts_inside = std::count_if( ps.begin(), ps.end(), [&]( const vpart_reference & vp ) {
        const tripoint p = veh.pos + vp.part().precalc[0];
        const auto eit = std::find( tiles.cbegin(), tiles.cend(), p );
        return eit != tiles.cend();
    } );

    if( vparts_inside == all_vparts_count ) {
        return overlap_status::inside;
    } else if( vparts_inside == 0 ) {
        return overlap_status::outside;
    } else {
        return overlap_status::overlap;
    }
}

struct elevator_vehicles {
    bool blocking = false;
    std::vector<vehicle *> v;
};

auto get_vehicles_on_elevator( const elevator_tiles &tiles ) -> elevator_vehicles
{
    const VehicleList vehs = get_map().get_vehicles();
    std::vector<vehicle *> ret;

    for( const wrapped_vehicle &veh : vehs ) {
        const auto status = vehicle_status( veh, tiles );
        if( status == overlap_status::overlap ) {
            return { true, { veh.v } };
        }
        if( status == overlap_status::inside ) {
            ret.push_back( veh.v );
        }
    }
    return { false, ret };
}

} // namespace

namespace
{

// still not sure whether there's a utility function for this
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
auto move_item( map &here, const tripoint &src, const tripoint &dest ) -> void
{
    map_stack items = here.i_at( src );
    for( auto it = items.begin(); it != items.end(); ) {
        here.add_item_or_charges( dest, *it );
        it = here.i_rem( src, it );
    }
};

auto move_creatures_away_from( const elevator_tiles &dest ) -> void
{
    map &here = get_map();

    const auto is_movable = [&]( const tripoint & candidate ) {
        return !here.has_flag( TFLAG_ELEVATOR, candidate )
               && here.passable( candidate )
               && !g->critter_at( candidate );
    };

    for( Creature &critter : g->all_creatures() ) {
        const tripoint local_pos = here.getlocal( here.getglobal( critter.pos() ) );

        const auto eit = std::find( dest.cbegin(), dest.cend(), local_pos );
        if( eit == dest.cend() ) {
            continue;
        }

        const auto xs = closest_points_first( *eit, 10 );
        const auto candidate = std::find_if( xs.begin(), xs.end(), is_movable );
        if( candidate == xs.end() ) {
            continue;
        }
        critter.setpos( *candidate );
    }
}

} //namespace

/**
 * If underground, move 2 levels up else move 2 levels down. Stable movement between levels 0 and -2.
 */
void iexamine::elevator( player &p, const tripoint &examp )
{
    map &here = get_map();
    const tripoint_abs_ms old_abs_pos = here.getglobal( p.pos() );
    const tripoint_abs_omt this_omt = project_to<coords::omt>( here.getglobal( examp ) );
    const tripoint sm_orig = here.getlocal( project_to<coords::ms>( this_omt ) );

    const auto elevator_here = get_elevator_here( p );
    const auto vehs = get_vehicles_on_elevator( elevator_here );
    if( vehs.blocking ) {
        const auto &first_veh_name = vehs.v.front()->name;
        popup( string_format( _( "The %s is blocking the elevator." ), first_veh_name ) );
        return;
    }

    const int movez = choose_elevator_destz( examp, this_omt, sm_orig );
    if( movez < -OVERMAP_DEPTH ) {
        return;
    }

    const tripoint_abs_omt that_omt{ this_omt.xy(), movez };
    const int turns = get_rot_delta( this_omt, that_omt );

    const auto elevator_dest = get_elevator_dest( elevator_here, sm_orig, turns, movez );

    move_creatures_away_from( elevator_dest );

    // move along every item in the elevator
    for( decltype( elevator_here )::size_type i = 0; i < elevator_here.size(); i++ ) {
        const tripoint &src = elevator_here[i];
        move_item( here, src, elevator_dest[i] );
    }

    // move along all creatures on the elevator
    for( Creature &critter : g->all_creatures() ) {
        const auto eit = std::find( elevator_here.cbegin(), elevator_here.cend(), critter.pos() );
        if( eit != elevator_here.cend() ) {
            critter.setpos( elevator_dest[ std::distance( elevator_here.cbegin(), eit ) ] );
        }
    }

    // move vehicles
    for( vehicle *v : vehs.v ) {
        const tripoint p = rotate_point_sm( { v->global_pos3().xy(), movez }, sm_orig, turns );
        here.displace_vehicle( *v, p - v->global_pos3() );
        v->turn( turns * 90_degrees );
        v->face = tileray( v->turn_dir );
        v->precalc_mounts( 0, v->turn_dir, v->pivot_anchor[0] );
    }
    here.reset_vehicle_cache();

    g->vertical_shift( movez );
    cata_event_dispatch::avatar_moves( *p.as_avatar(), here, old_abs_pos.raw() );
}
