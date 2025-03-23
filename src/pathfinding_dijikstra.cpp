#include "pathfinding_dijikstra.h"

#include <algorithm>
#include <functional>
#include <memory>
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

static const std::vector<point> DIRS_2D = {
    point_north_east,
    point_north_west,
    point_south_west,
    point_south_east,
    point_east,
    point_north,
    point_west,
    point_south,
};

void DijikstraPathfinding::scan_for_z_changes( int z_level )
{
    assert( -OVERMAP_DEPTH <= z_level && z_level <= OVERMAP_HEIGHT );

    if( DijikstraPathfinding::get_is_z_level_explored( z_level ) ) {
        return;
    }

    const map &map = get_map();

    for( const tripoint &cur : map.points_on_zlevel( z_level ) ) {
        const maptile &cur_tile = map.maptile_at( cur );
        const auto &cur_ter = cur_tile.get_ter_t();

        if( cur_ter.has_flag( TFLAG_NO_FLOOR ) ) {
            // Open air
            const tripoint below_us = cur + tripoint_below;

            if( !map.inbounds_z( below_us.z ) ) {
                continue;
            }

            if( map.impassable_ter_furn( below_us ) ) {
                continue;
            };
            // We won't do vehicle checks for simplicity

            const ZLevelChange going_down = ZLevelChange{ cur, below_us, DijikstraPathfinding::ZLevelChange::Type::OPEN_AIR };
            const ZLevelChange going_up = ZLevelChange{ below_us, cur, DijikstraPathfinding::ZLevelChange::Type::OPEN_AIR };

            DijikstraPathfinding::get_z_changes( z_level - 1 ).push_back( going_down );
            DijikstraPathfinding::get_z_changes( z_level ).push_back( going_up );
        } else if( cur_ter.has_flag( TFLAG_GOES_UP ) ) {
            // Stair bullshitery
            const tripoint above_us = cur + tripoint_above;

            if( !map.inbounds_z( above_us.z ) ) {
                continue;
            }

            // 10 to maintain parity with legacy A*
            // closest_points_first will ensure stairs above us directly will be hit first
            for( const tripoint &maybe_stairs_p : closest_points_first( above_us, 10 ) ) {
                const maptile &maybe_stairs_tile = map.maptile_at( maybe_stairs_p );
                const auto &maybe_stair_ter = maybe_stairs_tile.get_ter_t();

                if( maybe_stair_ter.has_flag( TFLAG_GOES_DOWN ) ) {
                    const ZLevelChange stairs_up = ZLevelChange{ cur, above_us, DijikstraPathfinding::ZLevelChange::Type::STAIRS };
                    const ZLevelChange stairs_down = ZLevelChange{ above_us, cur, DijikstraPathfinding::ZLevelChange::Type::STAIRS };
                    DijikstraPathfinding::get_z_changes( z_level ).push_back( stairs_down );
                    DijikstraPathfinding::get_z_changes( z_level + 1 ).push_back( stairs_up );
                    break;
                }
            }
        } else if( cur_ter.has_flag( TFLAG_GOES_DOWN ) ) {
            // Ditto
            const tripoint below_us = cur + tripoint_below;

            if( !map.inbounds_z( below_us.z ) ) {
                continue;
            }

            // 10 to maintain parity with legacy A*
            // closest_points_first will ensure stairs below us directly will be hit first
            for( const tripoint &maybe_stairs_p : closest_points_first( below_us, 10 ) ) {
                const maptile &maybe_stairs_tile = map.maptile_at( maybe_stairs_p );
                const auto &maybe_stairs_ter = maybe_stairs_tile.get_ter_t();

                if( maybe_stairs_ter.has_flag( TFLAG_GOES_UP ) ) {
                    const ZLevelChange stairs_down = ZLevelChange{ cur, below_us, DijikstraPathfinding::ZLevelChange::Type::STAIRS };
                    const ZLevelChange stairs_up = ZLevelChange{ below_us, cur, DijikstraPathfinding::ZLevelChange::Type::STAIRS };
                    DijikstraPathfinding::get_z_changes( z_level ).push_back( stairs_up );
                    DijikstraPathfinding::get_z_changes( z_level - 1 ).push_back( stairs_down );
                    break;
                }
            }
        } else if( cur_ter.has_flag( TFLAG_RAMP_UP ) ) {
            const tripoint above_us = cur + tripoint_above;

            if( !map.inbounds_z( above_us.z ) ) {
                continue;
            }

            const ZLevelChange ramp_up = ZLevelChange{ cur, above_us, DijikstraPathfinding::ZLevelChange::Type::RAMP };
            DijikstraPathfinding::get_z_changes( z_level + 1 ).push_back( ramp_up );
        } else if( cur_ter.has_flag( TFLAG_RAMP_DOWN ) ) {
            const tripoint below_us = cur + tripoint_below;

            if( !map.inbounds_z( below_us.z ) ) {
                continue;
            }

            const ZLevelChange ramp_down = ZLevelChange{ cur, below_us, DijikstraPathfinding::ZLevelChange::Type::RAMP };
            DijikstraPathfinding::get_z_changes( z_level - 1 ).push_back( ramp_down );
        }
    }

    DijikstraPathfinding::get_is_z_level_explored( z_level ) = true;
}

inline std::optional<std::vector<tripoint>> DijikstraPathfinding::get_route_2d(
            const point &from,
            const RouteSettings &route_settings )
{
    map &map = get_map();

    if( !this->is_in_limited_domain( from, from, route_settings ) ) {
        // This should only fail if max f-limit is failed
        return std::nullopt;
    }

    if( this->expand_2d_up_to( from, route_settings ) != ExpansionOutcome::PATH_FOUND ) {
        return std::nullopt;
    }

    const int chebyshev_distance = square_dist_fast(
                                       tripoint( from, this->z ),
                                       tripoint( this->dest, this->z ) );
    const float max_s = route_settings.max_s_coeff * chebyshev_distance;

    point cur_point = from;
    float cur_cost = this->d_map.get_f_unbiased( cur_point );

    std::vector<std::pair<float, point>> candidates;
    std::vector<tripoint> result;
    result.push_back( tripoint( from, this->z ) );

    while( cur_point != this->dest ) {
        for( const point &dir : DIRS_2D ) {
            const point next_point = cur_point + dir;
            const bool is_in_bounds = map.inbounds( next_point );
            if( !is_in_bounds ) {
                continue;
            }

            const float cost = this->d_map.get_f_unbiased( next_point );

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

        // This should be likely to happen, but...
        if( candidates.empty() ) {
            // Maybe instead of looking at directly adjacent points,
            //   increase the radius until we find a gradient?
            return std::nullopt;
        }

        std::sort( candidates.begin(), candidates.end(), []( auto & p1, auto & p2 ) {
            return p1.first < p2.first;
        } );

        const auto selected_pair = &candidates[route_settings.rank_weighted_rng( candidates.size() )];

        result.push_back( tripoint( selected_pair->second, this->z ) );
        cur_point = selected_pair->second;
        cur_cost = selected_pair->first;

        candidates.clear();

        // Path is too long in terms of steps taken
        if( result.size() - 2 > max_s ) {
            return std::nullopt;
        }
    }

    return result;
}

inline bool DijikstraPathfinding::is_in_limited_domain(
    const point &start, const point &p, const RouteSettings &route_settings )
{
    // Could be NaN if max_f_coeff = INFINITY * 0
    const float max_f = route_settings.max_f_coeff * (
                            route_settings.f_limit_based_on_max_dist ?
                            route_settings.max_dist :
                            rl_dist_exact( tripoint( start, this->z ), tripoint( this->dest, this->z ) )
                        );

    bool is_in_f_limited_area = is_nan( max_f ) || this->d_map.get_f_unbiased( p ) <= max_f;
    bool is_in_search_radius = route_settings.is_in_search_radius( start, p, this->dest );
    bool is_in_search_cone = route_settings.is_in_search_cone( start, p, this->dest );

    return is_in_f_limited_area || is_in_search_radius || is_in_search_cone;
}

inline void DijikstraPathfinding::detect_culled_frontier(
    const point &start, const RouteSettings &route_settings, std::unordered_set<point> &out )
{
    map &map = get_map();

    std::unordered_set<point> result;
    std::vector<point> stack;
    result.insert( start );
    stack.push_back( start );

    // This is deliberately a depth-first search in order to fail quickly upon reaching map edge
    while( !stack.empty() ) {
        const point p = stack.back();
        stack.pop_back();

        for( const point &dir : DIRS_2D ) {
            const point next = p + dir;
            if( !map.inbounds( next ) ) {
                // If we reached map edge, this means we're in an unclosed area
                return;
            }
            if( result.contains( next ) ) {
                continue;
            }
            // Visited area marks a boundary. This does include tiles outside search area, eventually.
            if( this->d_map.get_state( next ) != DijikstraMap::State::UNVISITED ) {
                continue;
            }
            if( !this->is_in_limited_domain( start, next, route_settings ) ) {
                // Failed domain test means we've reached a virtual boundary
                // Might as well make it INACCESSIBLE as well so we don't need to redo the check again
                this->d_map.p_at( next ) = NAN;
                continue;
            }
            stack.push_back( next );
            result.insert( next );
            break;
        }
    }
    // We have successfully been contained
    out = std::move( result );
}

inline DijikstraPathfinding::ExpansionOutcome DijikstraPathfinding::expand_2d_up_to(
    const point &start,
    const RouteSettings &route_settings )
{
    // We'll store h-coeff biased data here
    static Frontier biased_frontier;
    // If we happen to cull our search area, we'll store culled points here
    static std::vector<point> culled_frontier;
    // If this is not empty, we have encircled our target so don't bother expanding
    //   to tiles outside this area
    static std::unordered_set<point> unculled_area;

    if( start == this->dest ) {
        // Special case where if we already are standing on the destination tile
        return ExpansionOutcome::PATH_FOUND;
    }

    const tripoint start_with_z = tripoint( start, this->z );

    unculled_area.clear();
    culled_frontier.clear();
    biased_frontier = Frontier();

    const bool rebuild_needed = !route_settings.is_relative_search_domain() ||
                                this->domain == MapDomain::RELATIVE_DOMAIN;

    if( !rebuild_needed ) {
        const DijikstraMap::State state = this->d_map.get_state( start );
        switch( state ) {
            case DijikstraMap::State::ACCESSIBLE:
                return ExpansionOutcome::PATH_FOUND;
            case DijikstraMap::State::IMPASSABLE:
                return ExpansionOutcome::TARGET_INACCESSIBLE;
            case DijikstraMap::State::INACCESSIBLE:
                return ExpansionOutcome::NO_PATH_EXISTS;
            default:
                break;
        }
    } else {
        // Limited search requires clearing p-values, too, since they may change
        this->d_map.p = DijikstraPathfinding::FULL_INFINITY;

        this->unbiased_frontier.clear();
        this->unbiased_frontier.push_back( this->dest );
    }

    ExpansionOutcome result = ExpansionOutcome::UNSET;

    // Reset h-values for this pathfinding
    this->d_map.h = DijikstraPathfinding::FULL_NAN;
    this->d_map.p_at( this->dest ) = 0.0;
    this->d_map.g_at( this->dest ) = 0.0;

    // Drain unbiased frontier into biased_frontier
    for( const point &p : this->unbiased_frontier ) {
        this->d_map.h_at( p ) = rl_dist_exact( start_with_z, tripoint( p, this->z ) );
        biased_frontier.emplace( this->d_map.get_f_biased( p, route_settings.h_coeff ), p );
    }
    this->unbiased_frontier.clear();

    int it = 0; // Iteration counter
    const bool can_open_doors = !is_inf( this->settings.door_open_cost );
    const bool can_bash = this->settings.bash_strength_val > 0;
    const bool can_climb = !is_inf( this->settings.climb_cost );
    const bool care_about_mobs = this->settings.mob_presence_penalty > 0;
    const bool care_about_traps = this->settings.trap_cost > 0;
    const map &map = get_map();

    while( !biased_frontier.empty() ) {
        // Periodically check if `start` is enclosed
        //   and cull frontier if it is
        // This is useful to prevent exploring the whole map when target is inaccessible
        if( ++it % 200 == 0 ) {
            this->detect_culled_frontier( start, route_settings, unculled_area );
        }
        const point next_point = biased_frontier.top().second;
        const tripoint next_point_with_z = tripoint( next_point, this->z );

        biased_frontier.pop();

        if( !unculled_area.empty() && !unculled_area.contains( next_point ) ) {
            culled_frontier.push_back( next_point );
            continue;
        }

        // These might be valid frontier points, but if they are outside of our search area, then we will not go through them this time
        if( !this->is_in_limited_domain( start, next_point, route_settings ) ) {
            this->d_map.p_at( next_point ) = NAN; // Make the tile inaccessible [used by detect_culled_frontier]
            continue;
        }

        int _;
        const vehicle *next_vehicle;
        next_vehicle = map.veh_at_internal( next_point_with_z, _ );

        for( const point &dir : DIRS_2D ) {
            bool is_diag = dir.x != 0 && dir.y != 0;

            // It's cur_point because we're working backwards from destination
            const point cur_point = next_point + dir;
            const tripoint cur_point_with_z = tripoint( cur_point, this->z );

            if( !map.inbounds( cur_point ) ) {
                continue;
            }

            if( this->d_map.get_state( cur_point ) != DijikstraMap::State::UNVISITED ) {
                continue;
            }

            const bool h_calc_needed = is_nan( this->d_map.h_at( cur_point ) );
            float cur_h = h_calc_needed ?
                          rl_dist_exact( tripoint( start, this->z ), cur_point_with_z ) :
                          this->d_map.h_at( cur_point );
            this->d_map.h_at( cur_point ) = cur_h;

            int cur_vehicle_part;
            const vehicle *cur_vehicle;
            cur_vehicle = map.veh_at_internal( cur_point_with_z, cur_vehicle_part );

            {
                bool is_move_valid = true;

                const bool is_valid_to_step_into = this->settings.test_move_validity ?
                                                   map.valid_move( cur_point_with_z, next_point_with_z, true, this->settings.can_fly, true ) :
                                                   true;
                const bool is_valid_to_step_into_veh =
                    cur_vehicle == nullptr ?
                    true :
                    cur_vehicle->allowed_move( cur_vehicle->tripoint_to_mount( cur_point_with_z ),
                                               cur_vehicle->tripoint_to_mount( next_point_with_z ) );

                const bool is_valid_to_step_out_of_veh =
                    next_vehicle == nullptr ?
                    true :
                    next_vehicle->allowed_move( next_vehicle->tripoint_to_mount( cur_point_with_z ),
                                                next_vehicle->tripoint_to_mount( next_point_with_z ) );

                is_move_valid &= is_valid_to_step_into;
                is_move_valid &= is_valid_to_step_into_veh;
                is_move_valid &= is_valid_to_step_out_of_veh;

                if( !is_move_valid ) {
                    this->forbidden_moves.emplace( cur_point, next_point );
                    continue;
                }
            }

            const bool g_calc_needed = is_nan( this->d_map.g_at( cur_point ) );
            float cur_g = g_calc_needed ? 0.0 : this->d_map.g_at( cur_point );

            if( g_calc_needed ) {
                const maptile &new_tile = map.maptile_at( cur_point_with_z );
                const auto &terrain = new_tile.get_ter_t();
                const auto &furniture = new_tile.get_furn_t();
                const int move_cost = map.move_cost_internal( furniture, terrain, cur_vehicle, cur_vehicle_part );

                cur_g += is_diag ? 0.75 * move_cost : 0.5 * move_cost;

                // First, check for trivial cost modifiers
                const bool is_rough = move_cost > 2;
                const bool is_sharp = terrain.has_flag( TFLAG_SHARP );

                cur_g += is_rough ? this->settings.rough_terrain_cost : 0.0;
                cur_g += is_sharp ? this->settings.sharp_terrain_cost : 0.0;

                if( care_about_mobs ) {
                    cur_g += g->critter_at( cur_point_with_z,
                                            true ) != nullptr ? this->settings.mob_presence_penalty : 0.0;
                }

                if( care_about_traps ) {
                    const trap &maybe_ter_trap = terrain.trap.obj();
                    const trap &maybe_trap = maybe_ter_trap.is_benign() ? new_tile.get_trap_t() : maybe_ter_trap;
                    const bool is_trap = !maybe_trap.is_benign();

                    cur_g += is_trap ? this->settings.trap_cost : 0.0;
                }

                const bool is_ledge = map.has_zlevels() && terrain.has_flag( TFLAG_NO_FLOOR );
                if( is_ledge && !this->settings.can_fly ) {
                    // Close ledges outright for non-fliers
                    cur_g += INFINITY;
                }

                // And finally, add a potential field extra
                if( this->settings.extra_g_costs.contains( cur_point ) ) {
                    cur_g += this->settings.extra_g_costs.at( cur_point );
                }

                const bool is_passable = move_cost != 0;
                // Calculate the cost for if the tile is impassable
                if( !is_passable ) {
                    const bool is_climbable = terrain.has_flag( TFLAG_CLIMBABLE );
                    const bool is_door = !!terrain.open || !!furniture.open;

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
                            const bool is_cur_point_inside = map.veh_at_internal( cur_point_with_z, dummy ) == cur_vehicle;
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

                    if( is_nan( secondary_g_delta ) && is_climbable && can_climb ) {
                        secondary_g_delta = this->settings.climb_cost;
                    }
                    if( is_nan( secondary_g_delta ) && is_door && can_open_doors ) {
                        // Doors that can only be open from the inside
                        const bool door_opens_from_inside = terrain.has_flag( "OPENCLOSE_INSIDE" ) ||
                                                            furniture.has_flag( "OPENCLOSE_INSIDE" );
                        const bool is_cur_point_inside = !map.is_outside( cur_point );
                        const bool valid_to_open = door_opens_from_inside ? is_cur_point_inside : true;
                        if( valid_to_open ) {
                            secondary_g_delta = this->settings.door_open_cost;
                        }
                    }
                    if( is_nan( secondary_g_delta ) && can_bash ) {
                        // Time to consider bashing the obstacle
                        const int rating = map.bash_rating_internal(
                                               this->settings.bash_strength_val * this->settings.bash_strength_quanta,
                                               furniture, terrain, false, cur_vehicle, cur_vehicle_part );
                        if( rating > 1 ) {
                            secondary_g_delta = ( 10. / rating ) * this->settings.bash_cost;
                        } else if( rating == 1 ) {
                            secondary_g_delta = 100.0 *
                                                this->settings.bash_cost; // Extremely unlikely to take this route unless no other choice is present
                        }
                    }
                    if( is_nan( secondary_g_delta ) ) {
                        // We can do nothing anymore, close the tile
                        secondary_g_delta = INFINITY;
                    }

                    cur_g += secondary_g_delta;
                }
            }

            this->d_map.g_at( cur_point ) = cur_g;
            this->d_map.p_at( cur_point ) = this->d_map.get_f_unbiased( next_point );

            // Reintroduce this point into frontier unless the tile is closed
            if( !is_inf( cur_g ) ) {
                biased_frontier.push( {this->d_map.get_f_biased( cur_point, route_settings.h_coeff ), cur_point} );
            }

            if( cur_point == start ) {
                // We have reached the target
                if( this->d_map.get_state( cur_point ) == DijikstraMap::State::ACCESSIBLE ) {
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

    bool is_fully_explored = !route_settings.is_relative_search_domain() && biased_frontier.empty();

    // We will be rebuilding on next search anyway if we had a limited search this time
    if( !route_settings.is_relative_search_domain() ) {
        while( !biased_frontier.empty() ) {
            const point p = biased_frontier.top().second;
            biased_frontier.pop();

            this->unbiased_frontier.push_back( p );
        }
        for( const point &p : culled_frontier ) {
            this->unbiased_frontier.push_back( p );
        }
    }

    this->domain = route_settings.is_relative_search_domain() ? MapDomain::RELATIVE_DOMAIN :
                   MapDomain::ABSOLUTE_DOMAIN;

    if( result == ExpansionOutcome::UNSET ) {
        if( is_fully_explored ) {
            // Map must have no path to target
            for( size_t i = 0; i < DIJIKSTRA_ARRAY_SIZE; i++ ) {
                if( is_nan( this->d_map.g[i] ) ) {
                    this->d_map.p[i] = NAN;
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

    if( from == to ) {
        return std::vector<tripoint> { from, to };
    }

    if( rl_dist_exact( from, to ) > route_settings.max_dist ) {
        return std::vector<tripoint>();
    }

    if( from_copy.z == to_copy.z ) {
        // 2D search
        for( auto &map : DijikstraPathfinding::maps ) {
            if( map->dest == to_copy.xy() && map->z == to_copy.z && map->settings == path_settings ) {
                auto result = map->get_route_2d( from_copy.xy(), route_settings );
                return result.has_value() ? *result : std::vector<tripoint>();
            }
        }

        std::unique_ptr<DijikstraPathfinding> d_map = std::make_unique<DijikstraPathfinding>(
                    to_copy, path_settings );
        auto result = d_map->get_route_2d( from_copy.xy(), route_settings );

        DijikstraPathfinding::maps.push_back( std::move( d_map ) );

        return result.has_value() ? *result : std::vector<tripoint>();
    }

    // 3D search
    // We won't bother with complicated Z-level paths because that vastly, vastly increases the pathfinding cost
    // Instead, we will **only** consider taking z_changes that bring us closer to target's Z level.
    std::vector<tripoint> full_route;

    const bool we_go_up = to_copy.z > from_copy.z;

    for( int z = std::min( from_copy.z, to_copy.z ); z <= std::max( from_copy.z, to_copy.z ); z++ ) {
        DijikstraPathfinding::scan_for_z_changes( z );
    }

    tripoint cur_to = to_copy;
    std::unordered_set<tripoint> ramp_excluded;

    while( cur_to.z != from_copy.z ) {
        std::vector<DijikstraPathfinding::ZLevelChange> candidates;
        for( const auto &z_change : DijikstraPathfinding::get_z_changes( cur_to.z ) ) {
            bool can_be_taken = true;
            switch( z_change.type ) {
                case DijikstraPathfinding::ZLevelChange::Type::STAIRS:
                    can_be_taken &= path_settings.can_climb_stairs || path_settings.can_fly;
                    break;
                case DijikstraPathfinding::ZLevelChange::Type::OPEN_AIR:
                    can_be_taken &= path_settings.can_fly;
                    break;
                case DijikstraPathfinding::ZLevelChange::Type::RAMP:
                    // Ramps can be taken by all creatures currently
                    break;
            }

            const bool does_not_overshoot = we_go_up ? z_change.from.z >= from_copy.z : z_change.from.z <=
                                            from_copy.z;
            const bool leads_closer_to_from = we_go_up ?
                                              z_change.from.z < cur_to.z :
                                              z_change.from.z > cur_to.z;
            if( can_be_taken && does_not_overshoot && leads_closer_to_from ) {
                candidates.push_back( z_change );
            }
        }

        if( candidates.empty() ) {
            // No trivial Z-level path must exist, so go ahead and return nothing
            return std::vector<tripoint>();
        }

        // Now, find the best next Z level
        int best_next_z_level = to_copy.z;
        for( const auto &z_change : candidates ) {
            if( we_go_up ) {
                best_next_z_level = std::min( z_change.from.z, best_next_z_level );
            } else {
                best_next_z_level = std::max( z_change.from.z, best_next_z_level );
            }
        }

        float best_distance = INFINITY;
        tripoint best_from;
        tripoint best_to;
        bool best_is_ramp = false;

        for( const auto &z_change : candidates ) {
            if( z_change.from.z != best_next_z_level ) {
                continue;
            }
            const float dist = rl_dist_exact( cur_to, z_change.to );
            if( dist < best_distance ) {
                best_from = z_change.from;
                best_to = z_change.to;
                best_distance = dist;
                best_is_ramp = z_change.type == DijikstraPathfinding::ZLevelChange::Type::RAMP;
            }
        }
        const auto next_route = DijikstraPathfinding::route( cur_to, best_to, path_settings,
                                route_settings );
        if( next_route.empty() ) {
            // No trivial Z-level path must exist, so go ahead and return nothing
            return std::vector<tripoint>();
        }

        full_route.insert( full_route.end(), next_route.begin(), next_route.end() );
        if( best_is_ramp && !path_settings.can_fly ) {
            // Ramps are special in that we do not step on the last tile unless we're flying
            ramp_excluded.insert( best_from );
        }
        cur_to = best_from;
    }

    // We arrived to the target Z level
    const auto last_route = DijikstraPathfinding::route(
                                cur_to, from_copy,
                                path_settings, route_settings );
    if( last_route.empty() ) {
        // No route at the last Z level
        return std::vector<tripoint>();
    }
    full_route.insert( full_route.end(), last_route.begin(), last_route.end() );
    std::reverse( full_route.begin(), full_route.end() );

    // Finally, remove ramp tiles
    std::erase_if( full_route, [&ramp_excluded]( const tripoint & p ) {
        return ramp_excluded.contains( p );
    } );

    return full_route;
};
