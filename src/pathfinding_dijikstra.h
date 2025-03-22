#ifndef CATA_SRC_PATHFINDING_DIJIKSTRA_H
#define CATA_SRC_PATHFINDING_DIJIKSTRA_H

#include <algorithm>
#include <array>
#include <cstring>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "cata_utility.h"
#include "game_constants.h"
#include "line.h"
#include "point.h"
#include "rng.h"

namespace
{
// Thanks for nothing, MVSC
// For our MVSC builds, std::is_nan and std::is_inf are not constexpr
//   so we have to make our own

constexpr bool is_nan( float x )
{
    return x != x;
}
constexpr bool is_inf( float x )
{
    return x == INFINITY;
}
}
// A struct defining abilities of the actor and how to respond to various terrain features
struct PathfindingSettings {
    // Our approximate bash strength is `bash_strength_val` * `bash_strength_quanta`
    // We quantize bash strength to reduce the amount of maps created for different mob types, considering the actual bash strength
    //   does not change g-values much
    int bash_strength_val = 0;
    // Our approximate bash strength is `bash_strength_val` * `bash_strength_quanta`
    // We quantize bash strength to reduce the amount of maps created for different mob types, considering the actual bash strength
    //   does not change g-values much.
    int bash_strength_quanta = 10;

    // Even if we can bash, multiply time needed to do it by this
    // >1 to make bashes less attractive, <1 to make them more attractive. Do not use negative values.
    float bash_cost = 2.0;

    // Cost of climbing terrain. INFINITY if we can't
    float climb_cost = INFINITY;

    // Cost of moving through a trap tile, INFINITY to avoid completely. `can_fly == true` overrides this value to be 0.
    float trap_cost = 0.;

    // Cost of opening a door. INFINITY to never open doors, otherwise 2 to open and then move in.
    float door_open_cost = INFINITY;

    // Extra penalty for moving through rough terrain
    float rough_terrain_cost = 0.;

    // Extra penalty for moving through sharp terrain
    float sharp_terrain_cost = 0.;

    // If a mob is in the way currently, add this extra cost. INFINITY to always path around other critters.
    float mob_presence_penalty = 0.;

    // Whether we proactively test if taking a move is valid or not for every single direction.
    // This ensures that a returned path will be valid for the caller to take, but this is **ungodly** expensive.
    // So only use if you absolutely MUST be sure a path is move-valid.
    bool test_move_validity = true;

    // Can we fly? This implies we can climb stairs (`can_climb_stairs = true`),
    //   move over trap tiles freely (`trap_cost = 0`)
    //   and travel over open air and go up and down from there
    bool can_fly = false;

    // Can we climb stairs? `can_fly == true` overrides this value to be true.
    bool can_climb_stairs = false;

    // A map of tiles that have an extra G-cost assigned to them. Used for potential fields, preclosed tiles, etc.
    std::unordered_map<point, float> extra_g_costs;

    bool operator==( const PathfindingSettings &rhs ) const = default;
};

// A struct defining various coefficient used when creating/calculating a path from a dijikstra map
//   or determining tiles a valid path can cross where valid means at each point in the path the cost function decreases
struct RouteSettings {
    // How directed the pathfinding is. A value of 1.0 makes pathfinding equivalent to A*, 0.0 is raw Dijikstra;
    //   this adjusts precision, high values will converge quicker, but produce a possibly less than shortest path.
    float h_coeff = 1.0;

    // If multiple tiles are valid for a path, where valid means at each point in the path the cost function decreases,
    //   we may want to select one of them randomly to decongest routes across multiple pathfinds.
    // To do so, we'll rank up all the tiles based on their cost with first one being the most optimal path and last one being a least optimal path.
    // This coefficient determines how weights are distributed among these paths
    // -1 -- always choose the longest path (why would you though...)
    //  0 -- choose any tile with no bias [very "flowy" pathing]
    //  1 -- always choose the shortest path [in open terrain, this is just a straight line]
    // Don't bother setting this too close to 1.0, it will just make the path linear with rare single steps away from the shortest path
    float alpha = 1.0;

    unsigned int rank_weighted_rng( const unsigned int n ) const {
        assert( -1. <= this->alpha && this->alpha <= 1. );

        // Trivial cases
        if( this->alpha >= 1. ) {
            return 0;
        }

        if( this->alpha <= -1. ) {
            return n - 1;
        }

        if( this->alpha == 0. ) {
            return rng( 0, n - 1 );
        }

        const float r = rng_float( 0.0, 1.0 );
        return static_cast<unsigned int>( n * powf( r, ( 1. + this->alpha ) / ( 1. - this->alpha ) ) );
    }

    /*
    ```plain
        -----
       /  |r'\
      /   |   \
     /   ---   \
    |   / |r\   |
    |  |  |  |  |
    |--S--m--E--|
    |  |  |  |  |
    | t \ | /   |
     \   ---   /
      \   |   /
       \  |  /
        -----
     ```
    S -- `start`
    m -- `midpoint` (middle point between `start` and `end`)
    E -- `end`
    t -- candidate `t`ile
    r -- `r`adius (euclidean distance from `S` to `E`)
    r' -- search `r`adius (`r` * `search_radius_coeff`)
    Limit our search area to only tiles `t` that are inside circle of radius r'.

    **WARNING**: This necessiates rebuilding dijikstra map due to being relative domain, though g-values won't be recalculated which are generally the most expensive part.
      Additionally, limiting the search area too much might cause the destination to be inaccessible
      which is the worst case for pathfinding as it forces a complete scan of the whole search area though we have workarounds for that.
    Use only if needed.
    */
    float search_radius_coeff = INFINITY;
    // Test if `pos` is in the circle of radius distance from `start` to `end` by `search_radius_coeff` centered at `end`
    constexpr bool is_in_search_radius( const point start, const point pos,
                                        const point end ) const {
        if( is_inf( search_radius_coeff ) ) {
            return true;
        }

        const point midpoint = ( end + start ) / 2;

        const float objective_distance = rl_dist_exact( tripoint( start, 0 ), tripoint( end, 0 ) );
        const float search_radius = ( objective_distance * this->search_radius_coeff ) / 2;
        const float distance_to_objective = rl_dist_exact( tripoint( pos, 0 ), tripoint( midpoint, 0 ) );

        return distance_to_objective <= search_radius;
    }

    /*
    ```plain
          t
         / .
        /   .
       /     .
      /       .
     /a        .
    S----------E
    ```
    S -- `start`
    E -- `end`
    t -- candidate `t`ile
    a -- `a`ngle of tSE
    Limit our search area to only tiles `t` where `-search_cone_angle` <= `a` <= `search_cone_angle`

    **WARNING**: This necessiates rebuilding dijikstra map due to being relative domain, though g-values won't be recalculated which are generally the most expensive part.
      Additionally, limiting the search area too much might cause the destination to be inaccessible
      which is the worst case for pathfinding as it forces a complete scan of the whole search area though we have workarounds for that.
    Use only if needed.
    */
    float search_cone_angle = 180.0;
    // Test if `pos` is in the cone of `search_cone_angle` projected from `start` to `end`
    constexpr bool is_in_search_cone( const point start, const point pos,
                                      const point end ) const {
        assert( 0.0 <= this->search_cone_angle );

        if( this->search_cone_angle >= 180. ) {
            return true;
        }

        // A couple special cases for boundaries
        if( start == pos || start == end ) {
            return true;
        }

        const units::angle max_cone_angle = units::from_degrees( this->search_cone_angle );

        const point objective_delta = end - start;
        const units::angle objective_angle = units::atan2( objective_delta.y, objective_delta.x );

        const point conic_delta = pos - start;
        const units::angle conic_angle = units::atan2( conic_delta.y, conic_delta.x );

        const units::angle deviation = conic_angle - objective_angle;

        return -max_cone_angle <= deviation && deviation <= max_cone_angle;
    }

    /* Limit our search area such that a path will contain steps only up to this coefficient multiplied by chebyshev distance between start and end.
    In other words, it limits the amount of tiles to step through for any given path.
    Be aware this does **not** limit search domain, which means if nothing else limits it,
      the whole map will be explored and a longer in terms of steps, but less expensive of terms of time path will be rejected even if a valid shorter path exists.
    */
    float max_s_coeff = INFINITY;

    /* Limit our search only to paths whose unbiased f-value is less than
    this coefficient multiplied by the distance between start and end
    if `f_limit_based_on_max_dist` is false
    otherwise we multiply `max_dist` value instead.
    */
    float max_f_coeff = INFINITY;

    // Don't pathfind if target is more than this distance away.
    float max_dist = INFINITY;

    // Do we use distance between start and end or do we use `max_dist`
    //   for f-value limited search domain?
    // Check `max_f_coeff` for more detail.
    bool f_limit_based_on_max_dist = true;

    // Does the search domain depend on start position?
    constexpr bool is_relative_search_domain() const {
        return !( this->search_cone_angle >= 180. || is_inf( this->search_radius_coeff ) );
    }
};

class DijikstraPathfinding
{
    private:
        const static size_t DIJIKSTRA_ARRAY_SIZE = MAPSIZE_Y * MAPSIZE_X;
        typedef std::pair<float, point> val_pair;
        typedef std::priority_queue<val_pair, std::vector<val_pair>, pair_greater_cmp_first> Frontier;

        // Just a few preallocated array to memcpys from
        inline static std::array<float, DIJIKSTRA_ARRAY_SIZE> FULL_NAN = {0};
        inline static std::array<float, DIJIKSTRA_ARRAY_SIZE> FULL_INFINITY = {0};

        struct DijikstraMap {
            enum class State {
                UNVISITED, // Tile has not been expanded to yet
                ACCESSIBLE, // Tile is reachable
                IMPASSABLE, // Tile is reachable, but cannot be gone into
                INACCESSIBLE, // Tile is completely unreachable (or outside search area)
            };
            std::array<float, DIJIKSTRA_ARRAY_SIZE> p; // Smallest adjacent DijikstraValue's f
            // Get `p`-value at `p`
            constexpr float &p_at( const point &p ) {
                return this->p[this->get_flat_index( p )];
            };

            std::array<float, DIJIKSTRA_ARRAY_SIZE> g; // Associated tile's g cost [movement, bashing down...]
            // Get `g`-value at `p`
            constexpr float &g_at( const point &p ) {
                return this->g[this->get_flat_index( p )];
            };

            std::array<float, DIJIKSTRA_ARRAY_SIZE> h; // Heurestic to start [manhattan distance]
            // Get `h`-value at `p`
            constexpr float &h_at( const point &p ) {
                return this->h[this->get_flat_index( p )];
            };

            explicit DijikstraMap() {
                if( !is_inf( DijikstraPathfinding::FULL_INFINITY[0] ) ) {
                    DijikstraPathfinding::FULL_INFINITY.fill( INFINITY );
                }
                if( !is_nan( DijikstraPathfinding::FULL_NAN[0] ) ) {
                    DijikstraPathfinding::FULL_NAN.fill( NAN );
                }

                this->p = DijikstraPathfinding::FULL_INFINITY;
                this->g = DijikstraPathfinding::FULL_NAN;
                this->h = DijikstraPathfinding::FULL_NAN;
            }

            inline static constexpr size_t get_flat_index( const point &p ) {
                assert( 0 <= p.x &&
                        0 <= p.y &&
                        p.x < MAPSIZE_X &&
                        p.y < MAPSIZE_Y );
                size_t index = 0;
                index += p.x;
                index += p.y * MAPSIZE_X;

                return index;
            }

            // f0 = p + g
            inline constexpr float get_f_unbiased( const point &p ) {
                return this->p_at( p ) + this->g_at( p );
            }

            // f1 = p + g + `h_coeff` * h
            inline constexpr float get_f_biased( const point &p, float h_coeff ) {
                return this->get_f_unbiased( p ) + h_coeff * this->h_at( p );
            }

            inline constexpr State get_state( const point &p ) {
                if( is_inf( this->p_at( p ) ) ) {
                    return State::UNVISITED;
                }
                if( is_nan( this->p_at( p ) ) ) {
                    return State::INACCESSIBLE;
                }
                if( is_inf( this->g_at( p ) ) ) {
                    return State::IMPASSABLE;
                }
                return State::ACCESSIBLE;
            }
        };

        // `dest`ination of this map [2D]
        const point dest;
        // `z` level of this map
        const int z;
        // `settings` which were used to spawn this map
        const PathfindingSettings settings;

        // 1D array containing our map
        DijikstraMap d_map;

        enum class MapDomain {
            RELATIVE_DOMAIN, // Map's search domain limit includes relative limits (that is, depending on start position)
            ABSOLUTE_DOMAIN // Map's search domain limit is centered at the destination
        };
        MapDomain domain = MapDomain::RELATIVE_DOMAIN;

        // We don't want to calculate dijikstra of the whole map every time,
        //   so we store wave `frontier` to proceed from later if needed
        std::vector<point> unbiased_frontier;

        // Moves we don't allow to happen
        std::set<std::pair<point, point>> forbidden_moves;

        // Test if `p` is in our limited domain defined by `route_settings` relative to `start`
        inline bool is_in_limited_domain( const point &start, const point &p,
                                          const RouteSettings &route_settings );

        // See `DijikstraPathfinding::route`
        inline std::optional<std::vector<tripoint>> get_route_2d( const point &origin,
                const RouteSettings &route_settings );

        // Determine if `start` is surrounded by already visited tiles in `d_map` or tiles allowed by `route_settings`
        //   and if so, clear and fill `out` with all unexplored tiles left.
        inline void detect_culled_frontier( const point &start,
                                            const RouteSettings &route_settings,
                                            std::unordered_set<point> &out );

        enum class ExpansionOutcome {
            PATH_FOUND, // Path exists
            TARGET_INACCESSIBLE, // Although pathfinding reached the target, the target is inside some inaccessible location
            PATH_NOT_FOUND, // The map has not been explored fully, but a path may still exist with a wider search area
            NO_PATH_EXISTS, // Map explored fully, no path exists
            UNSET // Internal use
        };
        // Continue expanding the dijikstra map until we reach `origin` or nothing remains of the frontier. Returns whether a route is present.
        inline ExpansionOutcome expand_2d_up_to( const point &origin,
                const RouteSettings &route_settings );

        // Global state: memoized dijikstra maps. Clear every game turn.
        inline static std::vector<std::unique_ptr<DijikstraPathfinding>> maps;

        // Location we can change our Z level with
        struct ZLevelChange {
            enum class Type {
                STAIRS,
                RAMP,
                OPEN_AIR
            };

            const tripoint from;
            const tripoint to;
            const Type type;
        };

        // Z-level changes that lead to specified Z level.
        inline static std::vector<ZLevelChange> z_changes[OVERMAP_LAYERS];
        // Bit array of already explored Z levels
        inline static bool z_levels_explored[OVERMAP_LAYERS];

        // Scan Z-level for Z level changes
        static void scan_for_z_changes( int z_level );

        // Get a reference to ZLevelChange
        static std::vector<ZLevelChange> &get_z_changes( const int z ) {
            assert( -OVERMAP_DEPTH <= z && z <= OVERMAP_HEIGHT );

            return DijikstraPathfinding::z_changes[z + OVERMAP_DEPTH];
        }
        static bool &get_is_z_level_explored( const int z ) {
            assert( -OVERMAP_DEPTH <= z && z <= OVERMAP_HEIGHT );

            return DijikstraPathfinding::z_levels_explored[z + OVERMAP_DEPTH];
        }
    public:
        explicit DijikstraPathfinding( const tripoint dest, const PathfindingSettings settings )
            : dest( dest.xy() ), z( dest.z ), settings( settings ) {};

        // get `route` from `from` to `to` if available in accordance to `route_settings` while `path_settings` defines our capabilities, otherwise empty vector.
        // Found route will include `from` and `to`.
        static std::vector<tripoint> route( const tripoint &from, const tripoint &to,
                                            const std::optional<PathfindingSettings> path_settings = std::nullopt,
                                            const std::optional<RouteSettings> route_settings = std::nullopt );

        static void reset() {
            DijikstraPathfinding::maps.clear();
            for( int z_index = 0; z_index < OVERMAP_LAYERS; z_index++ ) {
                DijikstraPathfinding::z_changes[z_index].clear();
                DijikstraPathfinding::z_levels_explored[z_index] = false;
            }
        }
};
#endif // CATA_SRC_PATHFINDING_DIJIKSTRA_H
