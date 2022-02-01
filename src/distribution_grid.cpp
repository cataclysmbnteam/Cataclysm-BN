#include <unordered_set>

#include "debug.h"
#include "distribution_grid.h"
#include "coordinate_conversions.h"
#include "active_tile_data.h"
#include "map.h"
#include "mapbuffer.h"
#include "map_iterator.h"
#include "submap.h"
#include "options.h"
#include "overmapbuffer.h"

static distribution_grid empty_grid( {}, MAPBUFFER );

distribution_grid::distribution_grid( const std::vector<tripoint> &global_submap_coords,
                                      mapbuffer &buffer ) :
    submap_coords( global_submap_coords ),
    mb( buffer )
{
    for( const tripoint &sm_coord : submap_coords ) {
        submap *sm = mb.lookup_submap( sm_coord );
        if( sm == nullptr ) {
            // Debugmsg already printed in mapbuffer.cpp
            return;
        }

        for( auto &active : sm->active_furniture ) {
            const tripoint ms_pos = sm_to_ms_copy( sm_coord );
            const tripoint abs_pos = ms_pos + active.first;
            contents[sm_coord].emplace_back( active.first, abs_pos );
            flat_contents.emplace_back( abs_pos );
        }
    }
}

bool distribution_grid::empty() const
{
    return contents.empty();
}

distribution_grid::operator bool() const
{
    return !empty() && !submap_coords.empty();
}

void distribution_grid::update( time_point to )
{
    for( const std::pair<const tripoint, std::vector<tile_location>> &c : contents ) {
        submap *sm = mb.lookup_submap( c.first );
        if( sm == nullptr ) {
            return;
        }

        for( const tile_location &loc : c.second ) {
            auto &active = sm->active_furniture[loc.on_submap];
            if( !active ) {
                debugmsg( "No active furniture at %d,%d,%d",
                          loc.absolute.x, loc.absolute.y, loc.absolute.z );
                contents.clear();
                return;
            }
            active->update( to, loc.absolute, *this );
        }
    }
}

// TODO: Shouldn't be here
#include "vehicle.h"
static itype_id itype_battery( "battery" );
int distribution_grid::mod_resource( int amt, bool recurse )
{
    std::vector<vehicle *> connected_vehicles;
    for( const std::pair<const tripoint, std::vector<tile_location>> &c : contents ) {
        for( const tile_location &loc : c.second ) {
            battery_tile *battery = active_tiles::furn_at<battery_tile>( loc.absolute );
            if( battery != nullptr ) {
                amt = battery->mod_resource( amt );
                if( amt == 0 ) {
                    return 0;
                }
                continue;
            }

            if( !recurse ) {
                continue;
            }

            vehicle_connector_tile *connector = active_tiles::furn_at<vehicle_connector_tile>( loc.absolute );
            if( connector != nullptr ) {
                for( const tripoint &veh_abs : connector->connected_vehicles ) {
                    vehicle *veh = vehicle::find_vehicle( veh_abs );
                    if( veh == nullptr ) {
                        // TODO: Disconnect
                        debugmsg( "lost vehicle at %d,%d,%d", veh_abs.x, veh_abs.y, veh_abs.z );
                        continue;
                    }
                    connected_vehicles.push_back( veh );
                }
            }
        }
    }

    // TODO: Giga ugly. We only charge the first vehicle to get it to use its recursive graph traversal because it's inaccessible from here due to being a template method
    if( !connected_vehicles.empty() ) {
        if( amt > 0 ) {
            amt = connected_vehicles.front()->charge_battery( amt, true );
        } else {
            amt = -connected_vehicles.front()->discharge_battery( -amt, true );
        }
    }

    return amt;
}

int distribution_grid::get_resource( bool recurse ) const
{
    int res = 0;
    std::vector<vehicle *> connected_vehicles;
    for( const std::pair<const tripoint, std::vector<tile_location>> &c : contents ) {
        for( const tile_location &loc : c.second ) {
            battery_tile *battery = active_tiles::furn_at<battery_tile>( loc.absolute );
            if( battery != nullptr ) {
                res += battery->get_resource();
                continue;
            }

            if( !recurse ) {
                continue;
            }

            vehicle_connector_tile *connector = active_tiles::furn_at<vehicle_connector_tile>( loc.absolute );
            if( connector != nullptr ) {
                for( const tripoint &veh_abs : connector->connected_vehicles ) {
                    vehicle *veh = vehicle::find_vehicle( veh_abs );
                    if( veh == nullptr ) {
                        // TODO: Disconnect
                        debugmsg( "lost vehicle at %d,%d,%d", veh_abs.x, veh_abs.y, veh_abs.z );
                        continue;
                    }
                    connected_vehicles.push_back( veh );
                }
            }
        }
    }

    // TODO: Giga ugly. We only charge the first vehicle to get it to use its recursive graph traversal because it's inaccessible from here due to being a template method
    if( !connected_vehicles.empty() ) {
        res = connected_vehicles.front()->fuel_left( itype_battery, true );
    }

    return res;
}

distribution_grid_tracker::distribution_grid_tracker()
    : distribution_grid_tracker( MAPBUFFER )
{}

distribution_grid_tracker::distribution_grid_tracker( mapbuffer &buffer )
    : mb( buffer )
{
}

distribution_grid &distribution_grid_tracker::make_distribution_grid_at( const tripoint &sm_pos )
{
    if( !get_option<bool>( "ELECTRIC_GRID" ) ) {
        return empty_grid;
    }
    const std::set<tripoint> overmap_positions = overmap_buffer.electric_grid_at( sm_to_omt_copy(
                sm_pos ) );
    assert( !overmap_positions.empty() );
    std::vector<tripoint> submap_positions;
    for( const tripoint &omp : overmap_positions ) {
        submap_positions.emplace_back( omt_to_sm_copy( omp ) + point_zero );
        submap_positions.emplace_back( omt_to_sm_copy( omp ) + point_east );
        submap_positions.emplace_back( omt_to_sm_copy( omp ) + point_south );
        submap_positions.emplace_back( omt_to_sm_copy( omp ) + point_south_east );
    }
    shared_ptr_fast<distribution_grid> dist_grid = make_shared_fast<distribution_grid>
            ( submap_positions, mb );
    for( const tripoint &smp : submap_positions ) {
        parent_distribution_grids[smp] = dist_grid;
    }

    // This ugly expression + lint suppresion are needed to convince clang-tidy
    // that we are, in fact, NOT leaking memory.
    return *parent_distribution_grids[submap_positions.front()];
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
}

// TODO: Ugly, there should be a cleaner way
#include "worldfactory.h"

void distribution_grid_tracker::on_saved()
{
    parent_distribution_grids.clear();
    if( !get_option<bool>( "ELECTRIC_GRID" ) ||
        world_generator->active_world == nullptr ) {
        return;
    }
    tripoint min_bounds = tripoint( bounds.p_min, -OVERMAP_DEPTH );
    tripoint max_bounds = tripoint( bounds.p_max, OVERMAP_HEIGHT );
    // TODO: Only those which existed before the save
    for( const tripoint &sm_pos : tripoint_range( min_bounds, max_bounds ) ) {
        if( parent_distribution_grids.find( sm_pos ) == parent_distribution_grids.end() ) {
            make_distribution_grid_at( sm_pos );
        }
    }
}

void distribution_grid_tracker::on_changed( const tripoint &p )
{
    tripoint sm_pos = ms_to_sm_copy( p );
    // TODO: If not in bounds, just drop the grid, rebuild lazily
    if( parent_distribution_grids.count( sm_pos ) > 0 ||
        bounds.contains( sm_pos.xy() ) ) {
        // TODO: Don't rebuild, update
        make_distribution_grid_at( sm_pos );
    }

}

void distribution_grid_tracker::on_options_changed()
{
    on_saved();
}

distribution_grid &distribution_grid_tracker::grid_at( const tripoint &p )
{
    tripoint sm_pos = ms_to_sm_copy( p );
    auto iter = parent_distribution_grids.find( sm_pos );
    if( iter != parent_distribution_grids.end() ) {
        return *iter->second;
    }

    // This is ugly for the const case
    return make_distribution_grid_at( sm_pos );
}

const distribution_grid &distribution_grid_tracker::grid_at( const tripoint &p ) const
{
    return const_cast<const distribution_grid &>(
               const_cast<distribution_grid_tracker *>( this )->grid_at( p ) );
}

std::uintptr_t distribution_grid_tracker::debug_grid_id( const tripoint &omp ) const
{
    tripoint sm_pos = omt_to_sm_copy( omp );
    auto iter = parent_distribution_grids.find( sm_pos );
    if( iter != parent_distribution_grids.end() ) {
        distribution_grid *ret = iter->second.get();
        return reinterpret_cast<std::uintptr_t>( ret );
    } else {
        return 0;
    }
}

void distribution_grid_tracker::update( time_point to )
{
    // TODO: Don't recalc this every update
    std::unordered_set<const distribution_grid *> updated;
    for( auto &pr : parent_distribution_grids ) {
        if( updated.count( pr.second.get() ) == 0 ) {
            updated.emplace( pr.second.get() );
            pr.second->update( to );
        }
    }
}

void distribution_grid_tracker::load( half_open_rectangle area )
{
    bounds = area;
    // TODO: Don't reload everything when not needed
    on_saved();
}

void distribution_grid_tracker::load( const map &m )
{
    point p_min = m.get_abs_sub().xy();
    point p_max = p_min + point( m.getmapsize(), m.getmapsize() );
    load( half_open_rectangle( p_min, p_max ) );
}
