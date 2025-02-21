#include <algorithm>
#include <array>
#include <cstring>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <vector>

#include "cata_utility.h"
#include "game_constants.h"
#include "line.h"
#include "point.h"
#include "rng.h"

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

    // Cost of moving through a trap tile, INFINITY to avoid completely
    float trap_cost = 0.;

    // Cost of opening a door. INFINITY to never open doors, otherwise 2 to open and then move in.
    float door_open_cost = INFINITY;

    // Extra penalty for moving through rough terrain
    float rough_terrain_cost = 0.;

    // Extra penalty for moving through sharp terrain
    float sharp_terrain_cost = 0.;

    // Cost of moving up/down from any position. INFINITY if we can't fly.
    float fly_cost = INFINITY;

    // Cost of climbing stairs up and down. INFINITY if we can't.
    float stair_movement_cost = INFINITY;

    // If a mob is in the way currently, add this extra cost. INFINITY to always path around other critters.
    float mob_presence_penalty = 0.;

    // Whether we proactively test if taking a move is valid or not for every single direction.
    // This ensures that a returned path will be valid for the caller to take, but this is **ungodly** expensive.
    // So only use if you absolutely MUST be sure a path is move-valid.
    bool test_move_validity = true;

    constexpr bool operator==( const PathfindingSettings &rhs ) const = default;
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
    //  0 -- choose any tile with no bias
    //  1 -- always choose the shortest path
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
    |--S--E--|--|
    |  |  |  |  |
    | t \ | /   |
     \   ---   /
      \   |   /
       \  |  /
        -----
     ```
    S -- `start`
    E -- `end`
    t -- candidate `t`ile
    r -- `r`adius (euclidean distance from `S` to `E`)
    r' -- search `r`adius (`r` * `search_radius_coeff`)
    Limit our search area to only tiles `t` that are inside circle of radius r' are valid for pathfinding.

    **WARNING**: This necessiates rebuilding dijikstra map due to being partial domain, though g-values won't be recalculated which are generally the most expensive part.
      Additionally, limiting the search area too much might cause the destination to be inaccessible
      which is the worst case for pathfinding as it forces a complete scan of the whole search area.
    Use only if needed.
    */
    float search_radius_coeff = INFINITY;
    // Test if `pos` is in the circle of radius distance from `start` to `end` by `search_radius_coeff` centered at `end`
    constexpr bool is_in_search_radius( const tripoint start, const tripoint pos,
                                        const tripoint end ) const {
        if( std::isinf( search_radius_coeff ) ) {
            return true;
        }

        const float objective_distance = rl_dist_exact( start, end );
        const float search_radius = objective_distance * this->search_radius_coeff;
        const float distance_to_objective = rl_dist_exact( pos, end );

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

    **WARNING**: This necessiates rebuilding dijikstra map due to being partial domain, though g-values won't be recalculated which are generally the most expensive part.
      Additionally, limiting the search area too much might cause the destination to be inaccessible
      which is the worst case for pathfinding as it forces a complete scan of the whole search area.
    Use only if needed.
    */
    float search_cone_angle = 180.0;
    // Test if `pos` is in the cone of `search_cone_angle` projected from `start` to `end`
    constexpr bool is_in_search_cone( const tripoint start, const tripoint pos,
                                      const tripoint end ) const {
        assert( 0.0 <= this->search_cone_angle );

        if( this->search_cone_angle >= 180. ) {
            return true;
        }

        // A couple special cases for boundaries
        if( start == pos || start == end ) {
            return true;
        }

        const units::angle max_cone_angle = units::from_degrees( this->search_cone_angle );

        const point objective_delta = end.xy() - start.xy();
        const units::angle objective_angle = units::atan2( objective_delta.y, objective_delta.x );

        const point conic_delta = pos.xy() - start.xy();
        const units::angle conic_angle = units::atan2( conic_delta.y, conic_delta.x );

        const units::angle deviation = conic_angle - objective_angle;

        return -max_cone_angle <= deviation && deviation <= max_cone_angle;
    }

    /* Limit our search area to tiles  chebyshev distance

    **WARNING**: This necessiates rebuilding dijikstra map due to being partial domain, though g-values won't be recalculated which are generally the most expensive part.
      Additionally, limiting the search area too much might cause the destination to be inaccessible
      which is the worst case for pathfinding as it forces a complete scan of the whole search area though we have workarounds for that.
    Use only if needed.
    */
    float max_path_s_coefficient = INFINITY;

    /* Limit our search only to paths whose g value is less than this coefficient multiplied by the distance
    between start and destination

    **WARNING**: This necessiates rebuilding dijikstra map due to being partial domain, though g-values won't be recalculated which are generally the most expensive part.
      Additionally, limiting the search area too much might cause the destination to be inaccessible
      which is the worst case for pathfinding as it forces a complete scan of the whole search area though we have workarounds for that.
    Use only if needed.
    */
    float max_path_f_coefficient = INFINITY;

    // Is the search domain limited?
    constexpr bool is_limited_search() const {
        return !(
                   this->search_cone_angle >= 180. &&
                   std::isinf( this->search_radius_coeff ) &&
                   std::isinf( this->max_path_s_coefficient ) &&
                   std::isinf( this->max_path_f_coefficient )
               );
    }
};

class DijikstraPathfinding
{
    private:
        const static size_t DIJIKSTRA_ARRAY_SIZE = OVERMAP_LAYERS * MAPSIZE_Y * MAPSIZE_X;
        typedef std::pair<float, tripoint> val_pair;
        typedef std::priority_queue<val_pair, std::vector<val_pair>, pair_greater_cmp_first> Frontier;

        // Just a few preallocated array to memcpy from
        inline static std::array<float, DIJIKSTRA_ARRAY_SIZE> FULL_NAN = {0};
        inline static std::array<float, DIJIKSTRA_ARRAY_SIZE> FULL_INFINITY = {0};
        inline static std::array<int, DIJIKSTRA_ARRAY_SIZE> FULL_INT_MAX = {0};

        struct DijikstraMap {
            enum class State {
                UNVISITED, // Tile has not been expanded to yet
                ACCESSIBLE, // Tile is reachable
                IMPASSABLE, // Tile is reachable, but cannot be gone into
                INACCESSIBLE // Tile is completely unreachable
            };
            std::array<float, DIJIKSTRA_ARRAY_SIZE> p; // Smallest adjacent DijikstraValue's f
            // Get `p`-value at `p`
            constexpr float &p_at( const tripoint &p ) {
                return this->p[this->get_flat_index( p )];
            };

            std::array<float, DIJIKSTRA_ARRAY_SIZE> g; // Associated tile's g cost [movement, bashing down...]
            // Get `g`-value at `p`
            constexpr float &g_at( const tripoint &p ) {
                return this->g[this->get_flat_index( p )];
            };

            std::array<float, DIJIKSTRA_ARRAY_SIZE> h; // Heurestic to start [manhattan distance]
            // Get `h`-value at `p`
            constexpr float &h_at( const tripoint &p ) {
                return this->h[this->get_flat_index( p )];
            };

            std::array<int, DIJIKSTRA_ARRAY_SIZE> s; // Steps from `dest`ination
            // Get `s`-value at `p`
            constexpr int &s_at( const tripoint &p ) {
                return this->s[this->get_flat_index( p )];
            };

            explicit DijikstraMap() {
                if( !std::isinf( DijikstraPathfinding::FULL_INFINITY[0] ) ) {
                    DijikstraPathfinding::FULL_INFINITY.fill( INFINITY );
                }
                if( !std::isnan( DijikstraPathfinding::FULL_NAN[0] ) ) {
                    DijikstraPathfinding::FULL_NAN.fill( NAN );
                }
                if( DijikstraPathfinding::FULL_INT_MAX[0] != INT_MAX ) {
                    DijikstraPathfinding::FULL_INT_MAX.fill( INT_MAX );
                }

                this->p = DijikstraPathfinding::FULL_INFINITY;
                this->g = DijikstraPathfinding::FULL_NAN;
                this->h = DijikstraPathfinding::FULL_NAN;
                this->s = DijikstraPathfinding::FULL_INT_MAX;
            }

            inline static constexpr size_t get_flat_index( const tripoint &p ) {
                assert( 0 <= p.x &&
                        0 <= p.y &&
                        -OVERMAP_DEPTH <= p.z &&
                        p.x < MAPSIZE_X &&
                        p.y < MAPSIZE_Y &&
                        p.z <= OVERMAP_HEIGHT );
                size_t index = 0;
                index += p.x;
                index += p.y * MAPSIZE_X;
                index += ( p.z + OVERMAP_DEPTH ) * MAPSIZE_Y * MAPSIZE_X;

                return index;
            }

            // f0(x, y) = p + g(x, y)
            inline constexpr float get_f_unbiased( const tripoint &p ) {
                return this->p_at( p ) + this->g_at( p );
            }

            // f1(x, y) = p + g(x, y) + `h_coeff` * h(x, y)
            inline constexpr float get_f_biased( const tripoint &p, float h_coeff ) {
                return this->get_f_unbiased( p ) + h_coeff * this->h_at( p );
            }

            inline constexpr State get_state( const tripoint &p ) {
                if( std::isnan( this->g_at( p ) ) ) {
                    return State::UNVISITED;
                }
                if( std::isinf( this->g_at( p ) ) ) {
                    return State::IMPASSABLE;
                }
                if( std::isnan( this->p_at( p ) ) ) {
                    return State::INACCESSIBLE;
                }
                return State::ACCESSIBLE;
            }
        };

        // `dest`ination of this map
        const tripoint dest;
        // `settings` which were used to spawn this map
        const PathfindingSettings settings;

        // 1D array containing our map
        DijikstraMap d_map;

        enum class MapDomain {
            PARTIAL, // Map was built with limits to the search area
            FULL // Map was built with no limits to the search area
        };
        MapDomain domain = MapDomain::PARTIAL;

        // We don't want to calculate dijikstra of the whole map every time,
        //   so we store wave `frontier` to proceed from later if needed
        std::vector<tripoint> unbiased_frontier;

        // Moves we don't allow to happen
        std::set<std::pair<tripoint, tripoint>> forbidden_moves;

        // See `DijikstraPathfinding::route`
        inline std::optional<std::vector<tripoint>> get_route( const tripoint &origin,
                const RouteSettings &route_settings );

        enum class ExpansionOutcome {
            PATH_FOUND, // Path exists
            TARGET_INACCESSIBLE, // Although pathfinding reached the target, the target is inside some impassable location
            PATH_NOT_FOUND, // The map has not been explored fully, but a path may still exist with a wider search area
            NO_PATH_EXISTS, // Map explored fully, no path exists
            UNSET // Internal use
        };
        // Continue expanding the dijikstra map until we reach `origin` or nothing remains of the frontier. Returns whether a route is present.
        inline ExpansionOutcome expand_up_to( const tripoint &origin, const RouteSettings &route_settings );

        // Global state: memoized dijikstra maps. Clear every game turn.
        inline static std::vector<DijikstraPathfinding> maps;

        struct GraphPortal {
            const tripoint from;
            // Do we get teleported to destination from here upon entry or upon specific action?
            // false for ramp-like portals, true for stairs-like portals.
            const bool is_instant;
            // Time to go through this portal. 0 for ramp-like portals, non-0 for stair-like portals.
            float from_cost = NAN;

            GraphPortal( const tripoint from, const bool is_instant ) :
                from( from ), is_instant( is_instant ) {};
        };

        // A directed graph edge representing connected non-adjacent tiles
        // (multi-level ledges dropping down, stairs, literal distant portals etc).
        inline static std::optional<std::unordered_map<tripoint, GraphPortal>> portals;

        // Scan the whole map for portal-like jumps if `portals` is nullopt
        static void scan_for_portals();
    public:
        explicit DijikstraPathfinding( const tripoint dest, const PathfindingSettings settings )
            : dest( dest ), settings( settings ) {};

        // get `route` from `from` to `to` if available in accordance to `route_settings` while `path_settings` defines our capabilities, otherwise empty vector.
        // Route will include `from` and `to`.
        static std::vector<tripoint> route( const tripoint &from, const tripoint &to,
                                            const std::optional<PathfindingSettings> path_settings = std::nullopt,
                                            const std::optional<RouteSettings> route_settings = std::nullopt );

        static void reset() {
            DijikstraPathfinding::maps.clear();
            DijikstraPathfinding::portals.reset();
        }
};
