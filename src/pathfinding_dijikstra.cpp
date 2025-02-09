#include "pathfinding_dijikstra.h"

#include <algorithm>

#include "cursesdef.h"
#include "input.h"
#include "output.h"
#include "ui_manager.h"

#include "game.h"
#include "map.h"
#include "submap.h"
#include "trap.h"
#include "veh_type.h"
#include "vehicle.h"
#include "vehicle_part.h"
#include "vpart_position.h"

std::optional<std::vector<tripoint>> DijikstraPathfinding::get_route( const tripoint from,
                                  const RouteSettings route_settings )
{
    if( !g->m.inbounds( from ) ) {
        return std::nullopt;
    }

    if( this->is_disconnected( from ) ) {
        return std::nullopt;
    }

    if( this->is_unvisited( from ) ) {
        if( !this->expand_up_to( from ) ) {
            return std::nullopt;
        }
    }

    bool can_fly = !std::isinf( this->settings.fly_cost ); // TODO: add to pathfinding settings

    std::vector<tripoint> dirs = {
        tripoint( -1, -1, 0 ),
        tripoint( -1, 0, 0 ),
        tripoint( -1, 1, 0 ),
        tripoint( 0, -1, 0 ),
        tripoint( 0, 1, 0 ),
        tripoint( 1, -1, 0 ),
        tripoint( 1, 0, 0 ),
        tripoint( 1, 1, 0 ),
    };

    if( can_fly ) {
        dirs.push_back( tripoint( 0, 0, -1 ) );
        dirs.push_back( tripoint( 0, 0, 1 ) );
    }

    auto &map = g->m;

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
            tripoint next_point = cur_point + dir;
            const bool is_in_bounds = map.inbounds( next_point );
            if( !is_in_bounds ) {
                continue;
            }

            const bool has_valid_g_cost = !this->is_unvisited( next_point ) &&
                                          !this->is_disconnected( next_point );
            const bool is_not_forbidden = !this->forbidden_moves.contains(
                                              std::make_pair( cur_point, next_point ) );

            const bool is_in_search_radius = route_settings.is_in_search_radius( from, next_point, this->dest );

            // Search cone stuff
            bool is_in_search_cone = route_settings.is_in_search_cone( from, next_point, this->dest );

            bool is_valid = has_valid_g_cost &&
                            is_not_forbidden &&
                            is_in_search_radius &&
                            is_in_search_cone;
            if( !is_valid ) {
                continue;
            };

            float cost = this->at( next_point );
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

        auto selected_pair = &candidates[route_settings.rank_weighted_rng( candidates.size() )];

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
    std::vector<tripoint> dirs = {
        tripoint( -1, -1, 0 ),
        tripoint( -1, 0, 0 ),
        tripoint( -1, 1, 0 ),
        tripoint( 0, -1, 0 ),
        tripoint( 0, 1, 0 ),
        tripoint( 1, -1, 0 ),
        tripoint( 1, 0, 0 ),
        tripoint( 1, 1, 0 ),
    };

    bool can_fly = !std::isinf( this->settings.fly_cost );

    if( can_fly ) {
        dirs.push_back( tripoint( 0, 0, -1 ) );
        dirs.push_back( tripoint( 0, 0, 1 ) );
    }

    map &map = g->m;

    while( this->frontier.size() > 0 ) {
        const tripoint next_point = this->frontier.top().second;
        this->frontier.pop();

        int cur_vehicle_part;
        const vehicle *next_vehicle = map.veh_at_internal( next_point, cur_vehicle_part );

        int i = -1;

        for( tripoint &dir : dirs ) {
            i++;

            bool is_diag = false;
            bool is_vertical = false;
            switch( i ) {
                case 0:
                case 2:
                case 5:
                case 7:
                    is_diag = true;
                    break;
                case 8:
                case 9:
                    is_vertical = true;
                    break;
            }

            // It's cur_point because we're working backwards from destination
            tripoint cur_point = next_point + dir;

            {
                const bool is_in_bounds = map.inbounds( cur_point );
                if( !is_in_bounds ) {
                    continue;
                }
            }

            {
                const bool is_unvisited = this->is_unvisited( cur_point );
                if( !is_unvisited ) {
                    continue;
                }
            }

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
                            float hp = cur_vehicle->cpart( obstacle_part ).hp();
                            obstacle_bypass_cost = this->settings.bash_cost * hp / this->settings.bash_strength;
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
            this->at( cur_point ) = cur_cost + new_cost;

            if( !std::isinf( new_cost ) ) {
                float h = rl_dist_exact( cur_point, start );
                this->frontier.emplace( this->at( cur_point ) + route_settings.h_coeff * h,
                                        cur_point );
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

std::vector<tripoint> DijikstraPathfinding::route( tripoint from, tripoint to,
        const PathfindingSettings path_settings,
        const RouteSettings route_settings )
{
    const map &map = g->m;

    map.clip_to_bounds( from );
    map.clip_to_bounds( to );

    for( auto &map : DijikstraPathfinding::maps ) {
        if( map.dest == to && map.settings == path_settings ) {
            auto result = map.get_route( from, route_settings );
            return result.has_value() ? *result : std::vector<tripoint>();
        }
    }

    DijikstraPathfinding new_map( to, path_settings );
    DijikstraPathfinding::maps.push_back( std::move( new_map ) );

    auto result = new_map.get_route( from, route_settings );
    return result.has_value() ? *result : std::vector<tripoint>();
};
