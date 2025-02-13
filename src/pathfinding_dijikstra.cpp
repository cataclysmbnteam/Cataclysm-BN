#include "pathfinding_dijikstra.h"

#include <algorithm>
#include <queue>

#include "cursesdef.h"
#include "input.h"
#include "output.h"
#include "ui_manager.h"

#include "game.h"
#include "map.h"
#include "map_iterator.h"
#include "point.h"
#include "submap.h"
#include "trap.h"
#include "veh_type.h"
#include "vehicle.h"
#include "vehicle_part.h"
#include "vpart_position.h"

void DijikstraPathfinding::scan_for_portals()
{
    if( DijikstraPathfinding::portals.has_value() ) {
        return;
    }

    std::map<tripoint, GraphPortal> portals;
    map &map = get_map();

    for( int z = -OVERMAP_DEPTH; z <= OVERMAP_HEIGHT; z++ ) {
        for( const tripoint &cur : map.points_on_zlevel( z ) ) {
            const maptile &cur_tile = map.maptile_at( cur );
            const auto &cur_ter = cur_tile.get_ter_t();

            if( cur_ter.has_flag( TFLAG_GOES_UP ) ) {
                // Stair bullshitery
                const tripoint above_us = cur + tripoint_above;

                if( !map.inbounds( above_us ) ) {
                    continue;
                }

                // 10 to maintain parity with legacy A*
                // closest_points_first will ensure stairs above us directly will be hit first
                for( const tripoint &maybe_stairs_p : closest_points_first( above_us, 10 ) ) {
                    const maptile &maybe_stairs_tile = map.maptile_at( maybe_stairs_p );
                    const auto &maybe_stair_ter = maybe_stairs_tile.get_ter_t();

                    if( maybe_stair_ter.has_flag( TFLAG_GOES_DOWN ) ) {
                        GraphPortal up_portal = GraphPortal( cur, false );
                        GraphPortal down_portal = GraphPortal( above_us, false );
                        portals.emplace( above_us, up_portal );
                        portals.emplace( cur, down_portal );
                        break;
                    }
                }
            } else if( cur_ter.has_flag( TFLAG_GOES_DOWN ) ) {
                // Ditto
                const tripoint below_us = cur + tripoint_below;

                if( !map.inbounds( below_us ) ) {
                    continue;
                }

                // 10 to maintain parity with legacy A*
                // closest_points_first will ensure stairs below us directly will be hit first
                for( const tripoint &maybe_stairs_p : closest_points_first( below_us, 10 ) ) {
                    const maptile &maybe_stairs_tile = map.maptile_at( maybe_stairs_p );
                    const auto &maybe_stairs_ter = maybe_stairs_tile.get_ter_t();

                    if( maybe_stairs_ter.has_flag( TFLAG_GOES_UP ) ) {
                        GraphPortal down_portal = GraphPortal( cur, false );
                        GraphPortal up_portal = GraphPortal( below_us, false );
                        portals.emplace( below_us, down_portal );
                        portals.emplace( cur, up_portal );
                        break;
                    }
                }
            } else if( cur_ter.has_flag( TFLAG_RAMP ) || cur_ter.has_flag( TFLAG_RAMP_UP ) ) {
                // Ramps are a magic 0-cost portal
                const tripoint above_us = cur + tripoint_above;

                const maptile &should_be_open_air = map.maptile_at( above_us );
                const auto should_be_open_air_ter = should_be_open_air.get_ter_t();

                if( !should_be_open_air_ter.has_flag( TFLAG_NO_FLOOR ) ) {
                    // Blocked ramp
                    continue;
                }

                GraphPortal up_portal = GraphPortal( cur, true );
                portals.emplace( above_us, up_portal );
            } else if( cur_ter.has_flag( TFLAG_RAMP_DOWN ) ) {
                // Ramps are a magic 0-cost portal
                const tripoint above_us = cur + tripoint_above;

                const maptile &should_be_open_air = map.maptile_at( above_us );
                const auto should_be_open_air_ter = should_be_open_air.get_ter_t();

                if( !should_be_open_air_ter.has_flag( TFLAG_NO_FLOOR ) ) {
                    // Inaccessible ramp
                    continue;
                }

                GraphPortal down_portal = GraphPortal( above_us, true );
                portals.emplace( cur, down_portal );
            }
        }
    }
}

std::optional<std::vector<tripoint>> DijikstraPathfinding::get_route( const tripoint &from,
                                  const RouteSettings &route_settings )
{
    auto &map = get_map();

    if( !map.inbounds( from ) ) {
        return std::nullopt;
    }

    if( this->is_disconnected( from ) ) {
        return std::nullopt;
    }

    if( this->is_unvisited( from ) ) {
        if( !this->expand_up_to( from, route_settings ) ) {
            return std::nullopt;
        }
    }

    bool can_fly = !std::isinf( this->settings.fly_cost );

    std::vector<tripoint> dirs = {
        tripoint_north_east,
        tripoint_north_west,
        tripoint_south_west,
        tripoint_south_east,
        tripoint_east,
        tripoint_north,
        tripoint_west,
        tripoint_south,
    };

    if( can_fly ) {
        dirs.push_back( tripoint_below );
        dirs.push_back( tripoint_above );
    }

    const float objective_distance = rl_dist_exact( from, this->dest );

    tripoint cur_point = from;
    float cur_cost = this->at( cur_point );

    const float max_g_cost = route_settings.max_path_g_coefficient * objective_distance;
    if( cur_cost > max_g_cost ) {
        return std::nullopt;
    }

    const float max_steps = route_settings.max_path_length_coefficient * objective_distance;

    std::vector<std::pair<float, tripoint>> candidates;
    std::vector<tripoint> result;
    result.push_back( from );

    while( cur_point != this->dest ) {
        // Is there a portal to here? [such as stairs]
        // if( this->portals.contains( cur_point ) ) {
        //     const GraphPortal *portal = &this->portals[cur_point];
        //     if( portal->from_cost < cur_cost ) {
        //         candidates.emplace_back( portal->from, portal->from_cost );
        //     }
        // };

        for( tripoint &dir : dirs ) {
            if( result.size() - 1 > max_steps ) {
                // Path too long
                return std::nullopt;
            }
            const tripoint next_point = cur_point + dir;
            const bool is_in_bounds = map.inbounds( next_point );
            if( !is_in_bounds ) {
                continue;
            }

            const bool has_valid_g_cost = !this->is_unvisited( next_point ) &&
                                          !this->is_disconnected( next_point );
            const bool is_not_forbidden = !this->forbidden_moves.contains(
                                              std::make_pair( cur_point, next_point ) );

            const bool is_in_search_radius = route_settings.is_in_search_radius( from, next_point, this->dest );
            const bool is_in_search_cone = route_settings.is_in_search_cone( from, next_point, this->dest );

            const bool is_valid = has_valid_g_cost &&
                                  is_not_forbidden &&
                                  is_in_search_radius &&
                                  is_in_search_cone;
            if( !is_valid ) {
                continue;
            };

            const float cost = this->at( next_point );
            if( cost < cur_cost ) {
                candidates.emplace_back( cost, next_point );
            }
        }

        // This should not be likely to happen, but...
        if( candidates.size() == 0 ) {
            return std::nullopt;
        }

        std::sort( candidates.begin(), candidates.end(), []( auto & p1, auto & p2 ) {
            return p1.first < p2.first;
        } );

        const auto selected_pair = &candidates[route_settings.rank_weighted_rng( candidates.size() )];

        result.push_back( selected_pair->second );
        cur_point = selected_pair->second;
        cur_cost = selected_pair->first;

        candidates.clear();
    }

    g->draw_line( from, result );

    return result;
}

bool DijikstraPathfinding::expand_up_to( const tripoint &start,
        const RouteSettings &route_settings )
{
    map &map = get_map();

    bool can_fly = !std::isinf( this->settings.fly_cost );

    std::vector<tripoint> dirs = {
        tripoint_north_east,
        tripoint_north_west,
        tripoint_south_west,
        tripoint_south_east,
        tripoint_east,
        tripoint_north,
        tripoint_west,
        tripoint_south,
    };

    if( can_fly ) {
        dirs.push_back( tripoint_below );
        dirs.push_back( tripoint_above );
    }

    while( this->frontier.size() > 0 ) {
        const tripoint next_point = this->frontier.top().second;
        this->frontier.pop();

        int cur_vehicle_part;
        const vehicle *next_vehicle = map.veh_at_internal( next_point, cur_vehicle_part );

        int i = -1;

        for( tripoint &dir : dirs ) {
            i++;

            bool is_diag = i < 4;
            bool is_vertical = i >= 8;

            // It's cur_point because we're working backwards from destination
            tripoint cur_point = next_point + dir;

            {
                const bool is_in_bounds = map.inbounds( cur_point );
                if( !is_in_bounds ) {
                    continue;
                }
            }

            // {
            //     const bool is_unvisited = this->is_unvisited( cur_point );
            //     if( !is_unvisited ) {
            //         continue;
            //     }
            // }

            {
                bool is_in_search_radius = route_settings.is_in_search_radius( start, cur_point, this->dest );
                bool is_in_search_cone = route_settings.is_in_search_cone( start, cur_point, this->dest );
                const bool is_valid_to_check = is_in_search_radius && is_in_search_cone;

                if( !is_valid_to_check ) {
                    continue;
                };
            }

            {
                const bool is_valid_to_step_into = map.valid_move( cur_point, next_point,
                                                   this->settings.bash_strength > 0, can_fly, true );
                if( !is_valid_to_step_into ) {
                    DijikstraPathfinding::forbidden_moves.insert( std::make_pair( cur_point, next_point ) );
                    continue;
                }
            }

            int new_vehicle_part;
            const vehicle *cur_vehicle = map.veh_at_internal( cur_point, new_vehicle_part );

            {
                const bool is_valid_to_step_into =
                    cur_vehicle == nullptr ?
                    true :
                    cur_vehicle->allowed_move( cur_vehicle->tripoint_to_mount( cur_point ),
                                               cur_vehicle->tripoint_to_mount( next_point ) );

                const bool is_valid_to_step_out_of =
                    next_vehicle == nullptr ?
                    true :
                    next_vehicle->allowed_move( next_vehicle->tripoint_to_mount( cur_point ),
                                                next_vehicle->tripoint_to_mount( next_point ) );

                if( !( is_valid_to_step_into && is_valid_to_step_out_of ) ) {
                    DijikstraPathfinding::forbidden_moves.insert( std::make_pair( cur_point, next_point ) );
                    continue;
                }
            }

            const maptile &new_tile = map.maptile_at( cur_point );
            const auto &terrain = new_tile.get_ter_t();
            const auto &furniture = new_tile.get_furn_t();

            const float cur_cost = this->at( next_point );

            float new_cost = 0.;
            const int move_cost = map.move_cost( cur_point );

            new_cost += is_diag ? 0.75 * move_cost : 0.5 * move_cost;

            // First, check for trivial cost modifiers
            const bool care_about_mobs = this->settings.mob_presence_penalty > 0;
            const bool is_rough = move_cost > 2;
            const bool is_sharp = terrain.has_flag( TFLAG_SHARP );
            const trap maybe_tile_trap = new_tile.get_trap_t();
            const trap maybe_ter_trap = terrain.trap.obj();
            const bool is_trap = (
                                     ( !maybe_tile_trap.is_null() && !maybe_tile_trap.is_benign() ) ||
                                     ( !maybe_ter_trap.is_null() && !maybe_ter_trap.is_benign() )
                                 );


            new_cost += this->settings.rough_terrain_cost * is_rough;
            new_cost += this->settings.sharp_terrain_cost * is_sharp;
            new_cost += this->settings.trap_cost * is_trap;

            if( care_about_mobs ) {
                new_cost += this->settings.mob_presence_penalty * ( g->critter_at( cur_point, false ) != nullptr );
            }

            const bool is_passable = move_cost != 0;
            // Calculate the cost for if the tile is impassable
            if( !is_passable ) {
                const bool is_climbable = terrain.has_flag( TFLAG_CLIMBABLE );
                const bool can_climb = !std::isinf( this->settings.climb_cost );

                const bool is_door = !!terrain.open || !!furniture.open;
                const bool can_open_doors = !std::isinf( this->settings.door_open_cost );

                const bool can_bash = this->settings.bash_strength > 0;
                const bool is_ledge = is_trap && map.has_zlevels() && terrain.has_flag( TFLAG_NO_FLOOR );

                const bool does_z_level_change = ( terrain.has_flag( TFLAG_GOES_DOWN ) ||
                                                   terrain.has_flag( TFLAG_GOES_UP ) ||
                                                   terrain.has_flag( TFLAG_RAMP ) ||
                                                   terrain.has_flag( TFLAG_RAMP_UP ) ||
                                                   terrain.has_flag( TFLAG_RAMP_DOWN ) );

                float obstacle_bypass_cost = NAN;
                if( cur_vehicle != nullptr ) {
                    // Do processing for possible vehicle first
                    const auto vpobst = vpart_position( const_cast<vehicle &>( *cur_vehicle ),
                                                        new_vehicle_part ).obstacle_at_part();
                    const int obstacle_part = vpobst ? vpobst->part_index() : -1;

                    if( obstacle_part >= 0 ) {
                        int dummy;
                        const bool part_is_door = cur_vehicle->part_flag( obstacle_part, VPFLAG_OPENABLE );
                        const bool part_opens_from_inside = cur_vehicle->part_flag( obstacle_part, "OPENCLOSE_INSIDE" );
                        const bool is_cur_point_inside = map.veh_at_internal( cur_point, dummy ) == cur_vehicle;
                        const bool valid_to_open = part_is_door && ( part_opens_from_inside ? is_cur_point_inside : true );

                        if( can_open_doors && valid_to_open ) {
                            obstacle_bypass_cost = this->settings.door_open_cost;
                        } else if( can_bash ) {
                            const int htd = cur_vehicle->hits_to_destroy( obstacle_part, this->settings.bash_strength,
                                            DT_BASH );
                            if( htd == 0 ) {
                                // We cannot bash down this part
                                obstacle_bypass_cost = INFINITY;
                            } else {
                                obstacle_bypass_cost = this->settings.bash_cost * htd;
                            }
                        } else {
                            // Nothing can be done here. Don't bother with other checks since vehicles take priority.
                            obstacle_bypass_cost = INFINITY;
                        }
                    }
                }

                if( std::isnan( obstacle_bypass_cost ) && is_climbable && can_climb ) {
                    obstacle_bypass_cost = this->settings.climb_cost;
                }
                if( std::isnan( obstacle_bypass_cost ) && is_door && can_open_doors ) {
                    // Doors that can only be open from the inside
                    const bool door_opens_from_inside = terrain.has_flag( "OPENCLOSE_INSIDE" ) ||
                                                        furniture.has_flag( "OPENCLOSE_INSIDE" );
                    const bool is_cur_point_inside = !map.is_outside( cur_point );
                    const bool valid_to_open = door_opens_from_inside ? is_cur_point_inside : true;
                    if( valid_to_open ) {
                        obstacle_bypass_cost = this->settings.door_open_cost;
                    }
                }
                if( std::isnan( obstacle_bypass_cost ) && can_bash ) {
                    // Time to consider bashing the obstacle
                    const int rating = map.bash_rating( this->settings.bash_strength, cur_point );
                    if( rating > 1 ) {
                        obstacle_bypass_cost = ( 10. / rating ) * this->settings.bash_cost;
                    } else if( rating == 1 ) {
                        obstacle_bypass_cost = 100.0 *
                                               this->settings.bash_cost; // Extremely unlikely to take this route unless no other choice is present
                    }
                }
                if( std::isnan( obstacle_bypass_cost ) ) {
                    // We can do nothing anymore, close the tile
                    obstacle_bypass_cost = INFINITY;
                }

                new_cost += obstacle_bypass_cost;
            }

            // If this fails, somebody had a negative cost somewhere
            assert( new_cost >= 0 );

            // And now, set the score and expand frontier if non-disconnected
            const float next_cost = cur_cost + new_cost;
            const bool is_lower_g = this->is_unvisited( cur_point ) || next_cost < this->at( cur_point );;

            if( is_lower_g ) {
                this->at( cur_point ) = next_cost;

                if( !std::isinf( next_cost ) ) {
                    float h = rl_dist_exact( cur_point, start );
                    this->frontier.emplace( this->at( cur_point ) + route_settings.h_coeff * h,
                                            cur_point );
                }
            }

            if( cur_point == start ) {
                // We have reached the target
                // We need to remove the heurestic bias from frontier for next pathfinding
                if( route_settings.h_coeff > 0 ) {
                    std::priority_queue<val_pair, std::vector<val_pair>, pair_greater_cmp_first> new_frontier;
                    while( frontier.size() > 0 ) {
                        const val_pair pair = frontier.top();
                        const float h = rl_dist_exact( pair.second, start );
                        new_frontier.emplace( pair.first - h, pair.second );
                        frontier.pop();
                    }

                    this->frontier = new_frontier;
                }
                return !std::isinf( new_cost );
            }
        }
    }

    // The rest of the map must be disconnected, so propagate that
    for( float &val : this->cost_array ) {
        if( std::isnan( val ) ) {
            val = INFINITY;
        }
    }
    return false;
}

std::vector<tripoint> DijikstraPathfinding::route( const tripoint &from, const tripoint &to,
        const std::optional<PathfindingSettings> maybe_path_settings,
        const std::optional<RouteSettings> maybe_route_settings )
{
    const map &map = get_map();

    tripoint from_copy = from;
    tripoint to_copy = to;

    map.clip_to_bounds( from_copy );
    map.clip_to_bounds( to_copy );

    PathfindingSettings path_settings = maybe_path_settings.has_value() ? *maybe_path_settings :
                                        PathfindingSettings();
    RouteSettings route_settings = maybe_route_settings.has_value() ? *maybe_route_settings :
                                   RouteSettings();

    for( auto &map : DijikstraPathfinding::maps ) {
        if( map.dest == to_copy && map.settings == path_settings ) {
            auto result = map.get_route( from_copy, route_settings );
            return result.has_value() ? *result : std::vector<tripoint>();
        }
    }

    DijikstraPathfinding new_map( to_copy, path_settings );
    DijikstraPathfinding::maps.push_back( std::move( new_map ) );

    auto result = new_map.get_route( from_copy, route_settings );

    return result.has_value() ? *result : std::vector<tripoint>();
};
