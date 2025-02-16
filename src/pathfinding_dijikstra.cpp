#include "pathfinding_dijikstra.h"

#include <algorithm>
#include <queue>

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

static const std::vector<tripoint> DIRS_2D = {
    tripoint_north_east,
    tripoint_north_west,
    tripoint_south_west,
    tripoint_south_east,
    tripoint_east,
    tripoint_north,
    tripoint_west,
    tripoint_south,
};

static const std::vector<tripoint> DIRS_3D = {
    tripoint_north_east,
    tripoint_north_west,
    tripoint_south_west,
    tripoint_south_east,
    tripoint_east,
    tripoint_north,
    tripoint_west,
    tripoint_south,
    tripoint_below,
    tripoint_above
};

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

    if( this->expand_up_to( from, route_settings ) != ExpansionOutcome::PATH_FOUND ) {
        return std::nullopt;
    }

    bool can_fly = !std::isinf( this->settings.fly_cost );

    const std::vector<tripoint> &dirs = can_fly ? DIRS_3D : DIRS_2D;

    tripoint cur_point = from;
    float cur_cost = this->at( cur_point ).get_f_unbiased();

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

        for( const tripoint &dir : dirs ) {
            const tripoint next_point = cur_point + dir;
            const bool is_in_bounds = map.inbounds( next_point );
            if( !is_in_bounds ) {
                continue;
            }

            const DijikstraMapValue &val = this->at( next_point );
            const float cost = val.get_f_unbiased();

            const bool has_valid_f_cost = std::isfinite( cost );
            const bool is_not_forbidden = !this->forbidden_moves.contains( {cur_point, next_point} );

            const bool is_valid = has_valid_f_cost && is_not_forbidden;
            if( !is_valid ) {
                continue;
            };

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

    return result;
}

std::set<tripoint> flood_fill_to_boundary( const tripoint origin,
        const std::set<tripoint> &boundary )
{
    map &map = get_map();

    std::set<tripoint> result;
    result.insert( origin );
    for( const tripoint p : boundary ) {
        result.insert( p );
    }

    const tripoint DIRS[4] = {
        tripoint_east,
        tripoint_north,
        tripoint_west,
        tripoint_south
    };

    std::vector<tripoint> open = {origin};

    while( open.size() > 0 ) {
        const tripoint next = open.back();
        open.pop_back();

        for( const tripoint &offset : DIRS ) {
            const tripoint candidate = next + offset;
            if( !map.inbounds( candidate ) ) {
                continue;
            }
            if( result.contains( candidate ) ) {
                continue;
            }
            open.push_back( candidate );
            result.insert( candidate );
        }
    }

    return result;
}

ExpansionOutcome DijikstraPathfinding::expand_up_to( const tripoint &start,
        const RouteSettings &route_settings )
{
    // We'll store h-coeff biased data here
    static Frontier biased_frontier;
    // If we happen to cull our search area, we'll store culled points here
    static std::vector<tripoint> culled_frontier;
    // We'll store vehicle pointers here during pathfinding
    static std::map<tripoint, std::pair<const vehicle *, int>> vehicle_cache;
    // If this is not empty, we have encircled our target so don't bother expanding
    //   to tiles outside this area
    static std::set<tripoint> unculled_area;

    unculled_area.clear();
    vehicle_cache.clear();
    culled_frontier.clear();
    biased_frontier = Frontier();

    const bool can_fly = !std::isinf( this->settings.fly_cost );
    const bool rebuild_needed = !( this->domain == MapDomain::FULL &&
                                   !route_settings.is_limited_search() );

    const std::vector<tripoint> &dirs = can_fly ? DIRS_3D : DIRS_2D;

    const float euclidean_distance = rl_dist_exact( start, this->dest );
    const float chebyshev_distance = std::max( this->dest.x - start.x, this->dest.y - start.y );
    const float max_f = route_settings.max_path_f_coefficient * euclidean_distance;
    const float max_s = route_settings.max_path_s_coefficient * chebyshev_distance;
    map &map = get_map();

    ExpansionOutcome result = ExpansionOutcome::UNSET;

    // Reset h-values for this pathfinding
    for( DijikstraMapValue &val : this->dijikstra_array ) {
        val.h = NAN;
    }

    // Limited search requires clearing p-values, too, since they may change
    if( rebuild_needed ) {
        for( DijikstraMapValue &val : this->dijikstra_array ) {
            val.p = INFINITY;
            val.s = INT_MAX;
        }
    }

    this->at( this->dest ).p = 0.0;
    this->at( this->dest ).g = 0.0;
    this->at( this->dest ).s = 0;

    if( rebuild_needed ) {
        this->unbiased_frontier.clear();
        this->unbiased_frontier.push_back( this->dest );
    } else {
        const DijikstraValueState state = this->at( start ).get_state();
        switch( state ) {
            case DijikstraValueState::ACCESSIBLE:
                return ExpansionOutcome::PATH_FOUND;
            case DijikstraValueState::IMPASSABLE:
                return ExpansionOutcome::TARGET_INACCESSIBLE;
            case DijikstraValueState::INACCESSIBLE:
                return ExpansionOutcome::NO_PATH_EXISTS;
            default:
                break;
        }
    }

    // Drain unbiased frontier into biased_frontier
    for( const tripoint &p : this->unbiased_frontier ) {
        this->at( p ).h = rl_dist_exact( start, p );
        biased_frontier.emplace( this->at( p ).get_f_biased( route_settings.h_coeff ), p );
    }
    this->unbiased_frontier.clear();

    int it = 0; // Iteration counter

    while( biased_frontier.size() > 0 ) {
        // Periodically check if `start` is enclosed
        //   and cull frontier if it is
        // This is useful to prevent exploring the whole map when target is inaccessible
        if( ++it % 100 == 0 ) {
            // First, see if any point to the of start is processed so we can wall-walk and construct a boundary
            tripoint p = start;
            bool found_edge = false;
            while( map.inbounds( p ) ) {
                if( this->at( p ).get_state() != DijikstraValueState::UNVISITED ) {
                    found_edge = true;
                    break;
                }
                p += tripoint_east;
            }
            if( !found_edge ) {
                goto skipped_culling;
            }

            // https://imageprocessingplace.com/downloads_V3/root_downloads/tutorials/contour_tracing_Abeer_George_Ghuneim/moore.html
            std::vector<tripoint> polygon_vertices;
            const tripoint anchor = p;

            tripoint cw_offsets[8] = {
                tripoint_north_west,
                tripoint_north, // 1
                tripoint_north_east,
                tripoint_east, // 3
                tripoint_south_east,
                tripoint_south, // 5
                tripoint_south_west,
                tripoint_west // 7
            };
            const int BACKTRACK_LOOKUP[8] = {5, 7, 7, 1, 1, 3, 3, 5};
            int i = 0;
            int j = 0;
            int si = -1;
            while( true ) {
                const tripoint candidate = p + cw_offsets[i];
                if( !map.inbounds( candidate ) ) {
                    // If we are touching the boundary of the reality bubble, there isn't a meaningful way to construct an edge, so give up
                    goto skipped_culling;
                }
                if( j >= 9 ) {
                    // HIGHLY unlikely, but this must be an isolated singular point we just hit
                    goto skipped_culling;
                }
                const bool is_next = this->at( candidate ).get_state() != DijikstraValueState::UNVISITED;
                if( is_next ) {
                    polygon_vertices.push_back( candidate );
                    p = candidate;
                    i = BACKTRACK_LOOKUP[i];
                    j = 0;

                    if( candidate == anchor ) {
                        if( si == -1 ) {
                            si = i;
                        } else {
                            break;
                        }
                    }
                }
                i++;
                j++;
                i %= 8;
            }

            // Discovered polygon vertices may be repeated multiple times, crossing the same points
            // So let's normalize things
            std::vector<tripoint> polygon;
            std::set<tripoint> polygon_points;

            for( const tripoint &vertex : polygon_vertices ) {
                if( polygon_points.contains( vertex ) ) {
                    continue;
                }
                polygon.push_back( vertex );
                polygon_points.insert( vertex );
            }

            polygon.push_back( polygon.front() );

            // Calculate winding number now to determine if `start` is inside or outside
            int winding_number = 0;

            for( size_t i = 0; i < polygon.size() - 1; i++ ) {
                const int x_min = std::min( polygon[i].x, polygon[i + 1].x );
                const int x_max = std::max( polygon[i].x, polygon[i + 1].x );
                const int y_min = std::min( polygon[i].y, polygon[i + 1].y );
                const int y_max = std::max( polygon[i].y, polygon[i + 1].y );
                // For the purposes of winding number calc, we pretend that our start.y is offset by a tiny amount>
                // Which means only the following configurations are considered edge crossings
                // (L - `left`, R - `right`)
                // ```
                // L  |L| L|R |R| R|
                //  R |R|R | L|L|L |
                // ```
                if( start.x <= x_min && start.x < x_max && y_min <= start.y && start.y < y_max ) {
                    winding_number++;
                }
            }

            if( winding_number % 2 == 0 ) {
                goto skipped_culling;
            }

            unculled_area = std::move( flood_fill_to_boundary( start, polygon_points ) );
        }
skipped_culling:
        const tripoint next_point = biased_frontier.top().second;
        const DijikstraMapValue &next_val = this->at( next_point );
        biased_frontier.pop();

        if( unculled_area.size() > 0 && !unculled_area.contains( next_point ) ) {
            culled_frontier.push_back( next_point );
            continue;
        }

        // These might be valid frontier points, but if they are outside of our search area, then we will not go through them this time
        if( route_settings.is_limited_search() ) {
            bool is_in_search_radius = route_settings.is_in_search_radius( start, next_point, this->dest );
            bool is_in_search_cone = route_settings.is_in_search_cone( start, next_point, this->dest );
            bool is_f_within_limit = next_val.get_f_unbiased() < max_f;
            bool is_s_within_limit = next_val.s < max_s;

            const bool is_valid = is_in_search_radius && is_in_search_cone && is_f_within_limit &&
                                  is_s_within_limit;

            if( !is_valid ) {
                continue;
            };
        }

        int _;
        const vehicle *next_vehicle;
        if( vehicle_cache.contains( next_point ) ) {
            next_vehicle = vehicle_cache[next_point].first;
        } else {
            next_vehicle = map.veh_at_internal( next_point, _ );
            vehicle_cache.insert( {next_point, {next_vehicle, _}} );
        }

        int i = -1;
        for( const tripoint &dir : dirs ) {
            i++;

            bool is_diag = i < 4;
            bool is_vertical = i >= 8;

            // It's cur_point because we're working backwards from destination
            const tripoint cur_point = next_point + dir;

            {
                const bool is_in_bounds = map.inbounds( cur_point );
                if( !is_in_bounds ) {
                    continue;
                }
            }

            DijikstraMapValue &cur_val = this->at( cur_point );

            if( cur_val.get_state() != DijikstraValueState::UNVISITED ) {
                continue;
            }

            const bool h_calc_needed = std::isnan( cur_val.h );
            float cur_h = h_calc_needed ? rl_dist_exact( start, cur_point ) : cur_val.h;
            cur_val.h = cur_h;

            int cur_vehicle_part;
            const vehicle *cur_vehicle;
            if( vehicle_cache.contains( cur_point ) ) {
                cur_vehicle = vehicle_cache[cur_point].first;
                cur_vehicle_part = vehicle_cache[cur_point].second;
            } else {
                cur_vehicle = map.veh_at_internal( cur_point, cur_vehicle_part );
                vehicle_cache.insert( {cur_point, {cur_vehicle, cur_vehicle_part}} );
            }

            {
                bool is_move_valid = true;

                const bool is_valid_to_step_into = map.valid_move( cur_point, next_point, true, can_fly, true );
                const bool is_valid_to_step_into_veh =
                    cur_vehicle == nullptr ?
                    true :
                    cur_vehicle->allowed_move( cur_vehicle->tripoint_to_mount( cur_point ),
                                               cur_vehicle->tripoint_to_mount( next_point ) );

                const bool is_valid_to_step_out_of_veh =
                    next_vehicle == nullptr ?
                    true :
                    next_vehicle->allowed_move( next_vehicle->tripoint_to_mount( cur_point ),
                                                next_vehicle->tripoint_to_mount( next_point ) );

                is_move_valid &= is_valid_to_step_into;
                is_move_valid &= is_valid_to_step_into_veh;
                is_move_valid &= is_valid_to_step_out_of_veh;

                if( !is_move_valid ) {
                    this->forbidden_moves.emplace( cur_point, next_point );
                    continue;
                }
            }

            const bool g_calc_needed = std::isnan( cur_val.g );
            float cur_g = g_calc_needed ? 0.0 : cur_val.g;

            if( g_calc_needed ) {
                const maptile &new_tile = map.maptile_at( cur_point );
                const auto &terrain = new_tile.get_ter_t();
                const auto &furniture = new_tile.get_furn_t();
                const int move_cost = map.move_cost( cur_point );

                cur_g += is_diag ? 0.75 * move_cost : 0.5 * move_cost;

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


                cur_g += this->settings.rough_terrain_cost * is_rough;
                cur_g += this->settings.sharp_terrain_cost * is_sharp;
                cur_g += this->settings.trap_cost * is_trap;

                if( care_about_mobs ) {
                    cur_g += this->settings.mob_presence_penalty * ( g->critter_at( cur_point, true ) != nullptr );
                }

                const bool is_passable = move_cost != 0;
                // Calculate the cost for if the tile is impassable
                if( !is_passable ) {
                    const bool is_climbable = terrain.has_flag( TFLAG_CLIMBABLE );
                    const bool can_climb = !std::isinf( this->settings.climb_cost );

                    const bool is_door = !!terrain.open || !!furniture.open;
                    const bool can_open_doors = !std::isinf( this->settings.door_open_cost );

                    const bool can_bash = this->settings.bash_strength_val > 0;
                    const bool is_ledge = is_trap && map.has_zlevels() && terrain.has_flag( TFLAG_NO_FLOOR );

                    const bool does_z_level_change = ( terrain.has_flag( TFLAG_GOES_DOWN ) ||
                                                       terrain.has_flag( TFLAG_GOES_UP ) ||
                                                       terrain.has_flag( TFLAG_RAMP ) ||
                                                       terrain.has_flag( TFLAG_RAMP_UP ) ||
                                                       terrain.has_flag( TFLAG_RAMP_DOWN ) );

                    float secondary_g_delta = NAN;
                    if( cur_vehicle != nullptr ) {
                        // Do processing for possible vehicle first
                        const auto vpobst = vpart_position( const_cast<vehicle &>( *cur_vehicle ),
                                                            cur_vehicle_part ).obstacle_at_part();
                        const int obstacle_part = vpobst ? vpobst->part_index() : -1;

                        if( obstacle_part >= 0 ) {
                            int dummy;
                            const bool part_is_door = cur_vehicle->part_flag( obstacle_part, VPFLAG_OPENABLE );
                            const bool part_opens_from_inside = cur_vehicle->part_flag( obstacle_part, "OPENCLOSE_INSIDE" );
                            const bool is_cur_point_inside = map.veh_at_internal( cur_point, dummy ) == cur_vehicle;
                            const bool valid_to_open = part_is_door && ( part_opens_from_inside ? is_cur_point_inside : true );

                            if( can_open_doors && valid_to_open ) {
                                secondary_g_delta = this->settings.door_open_cost;
                            } else if( can_bash ) {
                                const int htd = cur_vehicle->hits_to_destroy( obstacle_part,
                                                this->settings.bash_strength_val * this->settings.bash_strength_quanta,
                                                DT_BASH );
                                if( htd == 0 ) {
                                    // We cannot bash down this part
                                    secondary_g_delta = INFINITY;
                                } else {
                                    secondary_g_delta = this->settings.bash_cost * htd;
                                }
                            } else {
                                // Nothing can be done here. Don't bother with other checks since vehicles take priority.
                                secondary_g_delta = INFINITY;
                            }
                        }
                    }

                    if( std::isnan( secondary_g_delta ) && is_climbable && can_climb ) {
                        secondary_g_delta = this->settings.climb_cost;
                    }
                    if( std::isnan( secondary_g_delta ) && is_door && can_open_doors ) {
                        // Doors that can only be open from the inside
                        const bool door_opens_from_inside = terrain.has_flag( "OPENCLOSE_INSIDE" ) ||
                                                            furniture.has_flag( "OPENCLOSE_INSIDE" );
                        const bool is_cur_point_inside = !map.is_outside( cur_point );
                        const bool valid_to_open = door_opens_from_inside ? is_cur_point_inside : true;
                        if( valid_to_open ) {
                            secondary_g_delta = this->settings.door_open_cost;
                        }
                    }
                    if( std::isnan( secondary_g_delta ) && can_bash ) {
                        // Time to consider bashing the obstacle
                        const int rating = map.bash_rating( this->settings.bash_strength_val *
                                                            this->settings.bash_strength_quanta,
                                                            cur_point );
                        if( rating > 1 ) {
                            secondary_g_delta = ( 10. / rating ) * this->settings.bash_cost;
                        } else if( rating == 1 ) {
                            secondary_g_delta = 100.0 *
                                                this->settings.bash_cost; // Extremely unlikely to take this route unless no other choice is present
                        }
                    }
                    if( std::isnan( secondary_g_delta ) ) {
                        // We can do nothing anymore, close the tile
                        secondary_g_delta = INFINITY;
                    }

                    cur_g += secondary_g_delta;
                }

                // If this fails, somebody had a negative cost somewhere
                // We also prevent cur_g == 0 case because this may produce a saddle point
                assert( cur_g > 0 );
            }

            cur_val.g = cur_g;
            cur_val.p = next_val.get_f_unbiased();
            cur_val.s = std::min( cur_val.s, next_val.s + 1 );

            // Reintroduce this point into frontier unless the tile is closed
            if( !std::isinf( cur_g ) ) {
                biased_frontier.push( {cur_val.get_f_biased( route_settings.h_coeff ), cur_point} );
            }

            if( cur_point == start ) {
                // We have reached the target
                if( cur_val.get_state() == DijikstraValueState::ACCESSIBLE ) {
                    result = ExpansionOutcome::PATH_FOUND;
                } else {
                    result = ExpansionOutcome::TARGET_INACCESSIBLE;
                }
                break;
            }
        }

        if( result != ExpansionOutcome::UNSET ) {
            break;
        }
    }

    bool is_fully_explored = !route_settings.is_limited_search() && biased_frontier.size() == 0;

    // We will be rebuilding on next search anyway if we had a limited search this time
    if( !route_settings.is_limited_search() ) {
        while( biased_frontier.size() > 0 ) {
            const tripoint p = std::move( biased_frontier.top().second );
            biased_frontier.pop();

            this->unbiased_frontier.push_back( p );
        }
        for( const tripoint &p : culled_frontier ) {
            this->unbiased_frontier.push_back( p );
        }
    }

    this->domain = route_settings.is_limited_search() ? MapDomain::PARTIAL : MapDomain::FULL;

    if( result == ExpansionOutcome::UNSET ) {
        if( is_fully_explored ) {
            // Map must have no path to target
            if( unculled_area.size() > 0 ) {
                for( const tripoint &p : unculled_area ) {
                    DijikstraMapValue &val = this->at( p );
                    if( val.get_state() == DijikstraValueState::UNVISITED ) {
                        val.p = NAN;
                    }
                }
            } else {
                for( DijikstraMapValue &val : this->dijikstra_array ) {
                    if( val.get_state() == DijikstraValueState::UNVISITED ) {
                        val.p = NAN;
                    }
                }
            }
            return ExpansionOutcome::NO_PATH_EXISTS;
        }
        return ExpansionOutcome::PATH_NOT_FOUND;
    }

    return result;
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
