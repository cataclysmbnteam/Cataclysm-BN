#include <algorithm>
#include <functional>
#include <optional>
#include <queue>
#include <vector>

#include "cata_utility.h"
#include "game_constants.h"
#include "map.h"
#include "point.h"
#include "rng.h"

const size_t DIJIKSTRA_ARRAY_SIZE = OVERMAP_LAYERS * MAPSIZE_Y * MAPSIZE_X;
typedef std::pair<float, tripoint> val_pair;

// A struct defining abilities of the actor and how to respond to various terrain features
struct PathfindingSettings {
    // Our bash strength
    int bash_strength = 0;
    // Even if we can bash, multiply time needed to do it by this
    // >1 to make bashes less attractive, <1 to make them more attractive. Do not use negative values.
    float bash_cost = 3.0;

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

    PathfindingSettings() = default;
    PathfindingSettings( PathfindingSettings const & ) = default;

    bool operator==( const PathfindingSettings &rhs ) const = default;
};

// A struct defining various coefficient used when creating/calculating a path from a dijikstra map
//   or determining tiles a valid path can cross where valid means at each point in the path the cost function decreases
struct RouteSettings {
    // How directed the pathfinding is. A value of 1.0 makes pathfinding equivalent to A*, 0.0 is raw Dijikstra;
    //   this adjusts precision, high values will converge quicker, but produce a possibly less than shortest path.
    float h_coeff = 1.0;

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
    r -- `r`adius (distance from `S` to `E`)
    r' -- search `r`adius (`r` * `search_radius_coeff`)
    Test if `t` is inside circle of radius r'.
    */
    float search_radius_coeff = INFINITY;

    // Test if `pos` is in the circle of radius distance from `start` to `end` by `search_radius_coeff` centered at `end`
    bool is_in_search_radius( const tripoint start, const tripoint pos, const tripoint end ) const {
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
    `-search_cone_angle` <= `a` <= `search_cone_angle`?
    */
    float search_cone_angle = 180.0;

    // Test if `pos` is in the cone of `search_cone_angle` projected from `start` to `end`
    bool is_in_search_cone( const tripoint start, const tripoint pos, const tripoint end ) const {
        assert( 0.0 <= this->search_cone_angle );

        if( this->search_cone_angle >= 180. ) {
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

    // If our chosen path is longer than this coefficient mulplitied by the minimum amount of tiles needed to
    //   go from start tile to destination in a straight line, then the path is considered not found
    float max_path_length_coefficient = INFINITY;

    // Limit our search only to paths whose g value is less than this coefficient multiplied by the distance
    //   between start and destination
    float max_path_g_coefficient = INFINITY;

    RouteSettings() = default;
    RouteSettings( RouteSettings const & ) = default;
};

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

class DijikstraPathfinding
{
    private:
        // `dest`ination of this map
        const tripoint dest;
        // `settings` which were used to spawn this map
        const PathfindingSettings settings;

        // 1D array containing our map
        // NAN for unvisited tiles
        // INF for disconnected tiles
        float cost_array[DIJIKSTRA_ARRAY_SIZE];

        // We don't want to calculate dijikstra of the whole map every time,
        //   so we store wave `frontier` to proceed from later if needed
        std::priority_queue<val_pair, std::vector<val_pair>, pair_greater_cmp_first> frontier;

        // See `DijikstraPathfinding::route`
        std::optional<std::vector<tripoint>> get_route( const tripoint &origin,
                                          const RouteSettings &route_settings );

        // Moves in this map that are between adjacent non-disconnected tiles that may NOT be taken
        std::set<std::pair<tripoint, tripoint>> forbidden_moves;

        // Continue expanding the dijikstra map until we reach `origin` or nothing remains of the frontier. Returns whether a route is present.
        bool expand_up_to( const tripoint &origin, const RouteSettings &route_settings );

        bool is_unvisited( const tripoint &p ) {
            return std::isnan( this->at( p ) );
        }

        bool is_disconnected( const tripoint &p ) {
            return std::isinf( this->at( p ) );
        }

        // Get cost `at` `p`
        float &at( const tripoint &p ) {
            assert( p.x >= 0 && p.y >= 0 && p.x < MAPSIZE_X && p.y < MAPSIZE_Y );

            // Row major ordering
            size_t index = 0;
            index += p.x;
            index += p.y * MAPSIZE_X;
            index += ( p.z + OVERMAP_DEPTH ) * MAPSIZE_Y * MAPSIZE_X;

            assert( index < DIJIKSTRA_ARRAY_SIZE );

            return this->cost_array[index];
        };

        // Global state: memoized dijikstra maps. Clear every game turn.
        inline static std::vector<DijikstraPathfinding> maps{};

        // A directed graph edge representing connected non-adjacent tiles
        // (multi-level ledges dropping down, stairs, literal distant portals etc).
        inline static std::optional<std::map<tripoint, GraphPortal>> portals = std::nullopt;
        // Scan the whole map for portal-like jumps if `portals` is nullopt
        static void scan_for_portals();
    public:
        DijikstraPathfinding( const tripoint dest, const PathfindingSettings settings )
            : dest( dest ), settings( settings ) {
            std::fill_n( this->cost_array, std::size( this->cost_array ), NAN );
            this->at( dest ) = 0.;
            this->frontier.emplace( 0., dest );
        };

        // get `route` from `from` to `to` if available in accordance to `route_settings` while `path_settings` defines our capabilities, otherwise empty vector.
        // Route will include `from` and `to`.
        static std::vector<tripoint> route( const tripoint &from, const tripoint &to,
                                            const std::optional<PathfindingSettings> path_settings = std::nullopt,
                                            const std::optional<RouteSettings> route_settings = std::nullopt );

        static void clear_paths() {
            DijikstraPathfinding::maps.clear();
        }

};
