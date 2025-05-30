#pragma once

#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <shared_mutex>

#include "coordinates.h"
#include "enums.h"
#include "json.h"
#include "memory_fast.h"
#include "overmap_types.h"
#include "point.h"
#include "string_id.h"
#include "type_id.h"

class character_id;
enum class cube_direction : int;
class map_extra;
class monster;
class npc;
class overmap;
class overmap_special;
class overmap_special_batch;
class throbber_popup;
class vehicle;
struct mapgen_arguments;
struct mongroup;
struct om_vehicle;
struct radio_tower;
struct regional_settings;

namespace om_direction
{
enum class type;
} // namespace om_direction

struct overmap_path_params {
    int road_cost = -1;
    int field_cost = -1;
    int dirt_road_cost = -1;
    int trail_cost = -1;
    int forest_cost = -1;
    int small_building_cost = -1;
    int shore_cost = -1;
    int swamp_cost = -1;
    int water_cost = -1;
    int air_cost = -1;
    int other_cost = -1;
    bool avoid_danger = true;
    bool only_known_by_player = true;

    static constexpr int standard_cost = 10;
    static overmap_path_params for_player();
    static overmap_path_params for_npc();
    static overmap_path_params for_land_vehicle( float offroad_coeff, bool tiny, bool amphibious );
    static overmap_path_params for_watercraft();
    static overmap_path_params for_aircraft();
};

struct radio_tower_reference {
    /** The radio tower itself, points into @ref overmap::radios */
    radio_tower *tower;
    /** The global absolute position of the tower (in submap coordinates) */
    point_abs_sm abs_sm_pos;
    /** Perceived signal strength (tower output strength minus distance) */
    int signal_strength;
    operator bool() const {
        return tower != nullptr;
    }
};

struct city_reference {
    static const city_reference invalid;
    /** The city itself, points into @ref overmap::cities */
    const struct city *city;
    /** The global absolute position of the city (in submap coordinates!) */
    tripoint_abs_sm abs_sm_pos;
    /** Distance to center of the search */
    int distance;

    operator bool() const {
        return city != nullptr;
    }

    int get_distance_from_bounds() const;
};

struct overmap_with_local_coords {
    overmap *om;
    tripoint_om_omt local;

    bool operator!() const {
        return !om;
    }

    explicit operator bool() const {
        return !!om;
    }
};

/**
 * Standard arguments for finding overmap terrain
 * @param origin Location of search
 * @param types vector of Terrain type/matching rule to use to find the type
 * @param exclude_types vector of Terrain type/matching rule to use to find the type
 * @param search_range The [minimum, maximum] search distance. If maximum is 0, a function specific default is used.
 * @param search_layers If set, overrides the search layers to this range. Layer from search origin otherwise.
 * @param min_distance Matches within min_distance are ignored.
 * @param seen If true, only terrain seen by the player should be searched.
 * If false, only terrain not seen by the player should be searched.
 * @param explored If true, only terrain marked as explored by the player should be searched.
 * If false, only terrain not marked as explored by the player should be searched.
 * @param existing_only If true, will restrict searches to existing overmaps only. This
 * is particularly useful if we want to attempt to add a missing overmap special to an existing
 * overmap rather than creating many overmaps in an attempt to find it.
 * @param om_special If set, the terrain must be part of the specified overmap special.
 * @param popup If set, the popup will be periodically updated to indicate ongoing search.
 * @param max_results, If set, limits the search result to at most n entries
*/
struct omt_find_params {
    ~omt_find_params();

    std::vector<std::pair<std::string, ot_match_type>> types;
    std::vector<std::pair<std::string, ot_match_type>> exclude_types;
    std::pair<int, int> search_range = { 0, 0 };
    std::optional<std::pair<int, int>> search_layers = std::nullopt;
    std::optional<bool> seen = std::nullopt;
    std::optional<bool> explored = std::nullopt;
    bool existing_only = false;
    std::optional<overmap_special_id> om_special = std::nullopt;
    shared_ptr_fast<throbber_popup> popup = nullptr;
    std::optional<int> max_results = std::nullopt;
};

constexpr const std::pair<int, int> omt_find_all_layers = { -OVERMAP_DEPTH, OVERMAP_HEIGHT };
constexpr const std::pair<int, int> omt_find_above_ground_layer = { 0, OVERMAP_HEIGHT };

/**
 * Standard arguments for finding overmap route
 * @param radius Radius of revealed terrain around the route
 * @param road_only If set, restricts route to only use roads
 * @param popup If set, the popup will be periodically updated to indicate ongoing pathfinding.
 */
struct omt_route_params {
    ~omt_route_params();

    int radius = 0;
    bool road_only = false;
    shared_ptr_fast<throbber_popup> popup = nullptr;
};

class overmapbuffer
{
    public:
        overmapbuffer();

        /**
         * Uses overmap coordinates, that means x and y are directly
         * compared with the position of the overmap.
         */
        overmap &get( const point_abs_om & );
        void save();
        void clear();
        void create_custom_overmap( const point_abs_om &, overmap_special_batch &specials );

        /**
        * Generates overmap tiles, if missing
        */
        void generate( const std::vector<point_abs_om> &locs );

        /**
         * Returns the overmap terrain at the given OMT coordinates.
         * Creates a new overmap if necessary.
         */
        const oter_id &ter( const tripoint_abs_omt &p );
        /**
         * Returns the overmap terrain at the given OMT coordinates.
         * Returns ot_null if the point is not in any existing overmap.
         */
        const oter_id &ter_existing( const tripoint_abs_omt &p );
        void ter_set( const tripoint_abs_omt &p, const oter_id &id );
        std::string *join_used_at( const std::pair<tripoint_abs_omt, cube_direction> & );
        std::optional<mapgen_arguments> *mapgen_args( const tripoint_abs_omt & );
        /**
         * Uses global overmap terrain coordinates.
         */
        bool has_note( const tripoint_abs_omt &p );
        /**
         * Check whether the tile has a note that's marked as dangerous.
         * If such note exists, returns danger radius (may be 0).
         */
        std::optional<int> has_note_with_danger_radius( const tripoint_abs_omt &p );
        bool is_marked_dangerous( const tripoint_abs_omt &p );
        const std::string &note( const tripoint_abs_omt &p );
        void add_note( const tripoint_abs_omt &, const std::string &message );
        void delete_note( const tripoint_abs_omt &p );
        void mark_note_dangerous( const tripoint_abs_omt &p, int radius, bool is_dangerous );
        bool has_extra( const tripoint_abs_omt &p );
        const string_id<map_extra> &extra( const tripoint_abs_omt &p );
        void add_extra( const tripoint_abs_omt &p, const string_id<map_extra> &id );
        void delete_extra( const tripoint_abs_omt &p );
        bool is_explored( const tripoint_abs_omt &p );
        void toggle_explored( const tripoint_abs_omt &p );
        bool is_path( const tripoint_abs_omt &p );
        void toggle_path( const tripoint_abs_omt &p );
        bool seen( const tripoint_abs_omt &p );
        void set_seen( const tripoint_abs_omt &p, bool seen = true );
        bool has_vehicle( const tripoint_abs_omt &p );
        bool has_horde( const tripoint_abs_omt &p );
        int get_horde_size( const tripoint_abs_omt &p );
        std::vector<om_vehicle> get_vehicle( const tripoint_abs_omt &p );
        const regional_settings &get_settings( const tripoint_abs_omt &p );
        /**
         * Accessors for horde introspection into overmaps.
         * Probably also useful for NPC overmap-scale navigation.
         */
        /**
         * Returns the 3x3 array of scent values surrounding the origin point.
         * @param origin is in world-global omt coordinates.
         */
        std::array<std::array<scent_trace, 3>, 3> scents_near( const tripoint_abs_omt &origin );
        /**
         * Method to retrieve the scent at a given location.
         **/
        scent_trace scent_at( const tripoint_abs_omt &pos );
        /**
         * Method to set a scent trace.
         * @param loc is in world-global omt coordinates.
         * @param strength sets the intensity of the scent trace,
         *     used for determining if a monster can detect the scent.
         */
        void set_scent( const tripoint_abs_omt &loc, int strength );
        /**
         * Check for any dangerous monster groups at the global overmap terrain coordinates.
         * If there are any, it's not safe.
         */
        bool is_safe( const tripoint_abs_omt &p );

        /**
         * Move the tracking mark of the given vehicle.
         * @param veh The vehicle whose tracking device is active and
         * that has been moved.
         * @param old_msp The previous position (before the movement) of the
         * vehicle. In map square coordinates (see vehicle::real_global_pos), it's
         * used to remove the vehicle from the old overmap if the new position is
         * on another overmap.
         */
        void move_vehicle( vehicle *veh, const point_abs_ms &old_msp );
        /**
         * Add the vehicle to be tracked in the overmap.
         */
        void add_vehicle( vehicle *veh );
        /**
         * Remove the vehicle from being tracked in the overmap.
         */
        void remove_vehicle( const vehicle *veh );

        /**
         * Get all npcs in a area with given radius around given central point.
         * Only npcs on the given z-level are considered.
         * Uses square_dist for distance calculation.
         * @param p Central point in submap coordinates.
         * @param radius Maximal distance of npc from (x,y). If the npc
         * is at most this far away from (x,y) it will be returned.
         * A radius of 0 returns only those npcs that are on the
         * specific submap.
         */

        std::vector<shared_ptr_fast<npc>> get_npcs_near( const tripoint_abs_sm &p, int radius );
        /**
         * Get all (currently loaded!) npcs that have a companion
         * mission set.
         */
        std::vector<shared_ptr_fast<npc>> get_companion_mission_npcs( int range = 100 );
        /**
         * Uses overmap terrain coordinates, this also means radius is
         * in overmap terrain.
         * A radius of 0 returns all npcs that are on that specific
         * overmap terrain tile.
         */
        std::vector<shared_ptr_fast<npc>> get_npcs_near_omt( const tripoint_abs_omt &p, int radius );
        /**
         * Same as @ref get_npcs_near(int,int,int,int) but uses
         * player position as center.
         */
        std::vector<shared_ptr_fast<npc>> get_npcs_near_player( int radius );
        /**
         * Find the npc with the given ID.
         * Returns NULL if the npc could not be found.
         * Searches all loaded overmaps.
         */
        shared_ptr_fast<npc> find_npc( character_id id );
        /**
         * Get all NPCs active on the overmap
         */
        std::vector<shared_ptr_fast<npc>> get_overmap_npcs();
        /**
         * Find npc by id and if found, erase it from the npc list
         * and return it ( or return nullptr if not found ).
         */
        shared_ptr_fast<npc> remove_npc( const character_id &id );
        /**
         * Adds the npc to an overmap ( based on the npcs current location )
         * and stores it there. The overmap takes ownership of the pointer.
         */
        void insert_npc( const shared_ptr_fast<npc> &who );

        /**
         * Find all places with the specific overmap terrain type.
         * This function may create a new overmap if needed.
         * @param origin Location of search
         * see omt_find_params for definitions of the terms
         */
        std::vector<tripoint_abs_omt> find_all( const tripoint_abs_omt &origin,
                                                const omt_find_params &params );
    private:
        std::vector<tripoint_abs_omt> find_all_async( const tripoint_abs_omt &origin,
                const omt_find_params &params );
        std::vector<tripoint_abs_omt> find_all_sync( const tripoint_abs_omt &origin,
                const omt_find_params &params );
    public:

        /**
         * Returns a random point of specific terrain type among those found in certain search
         * radius.
         * This function may create new overmaps if needed.
         * @param origin Location of search
         * see omt_find_params for definitions of the terms
         */
        tripoint_abs_omt find_random( const tripoint_abs_omt &origin,
                                      const omt_find_params &params );

        /**
         * Returns the closest point of terrain type.
         * @param origin Location of search
         * see omt_find_params for definitions of the terms
         */
        tripoint_abs_omt find_closest( const tripoint_abs_omt &origin,
                                       const omt_find_params &params );


        /**
         * Mark a square area around center on Z-level z
         * as seen.
         * @param center is in absolute overmap terrain coordinates.
         * @param radius The half size of the square to make visible.
         * A value of 0 makes only center visible, radius 1 makes a
         * square 3x3 visible.
         * @param z Z level to make area on
         * @return true if something has actually been revealed.
         */
        bool reveal( const point_abs_omt &center, int radius, int z );
        bool reveal( const tripoint_abs_omt &center, int radius );
        bool reveal( const tripoint_abs_omt &center, int radius,
                     const std::function<bool( const oter_id & )> &filter );
        std::vector<tripoint_abs_omt> get_travel_path(
            const tripoint_abs_omt &src, const tripoint_abs_omt &dest, overmap_path_params params );
        bool reveal_route( const tripoint_abs_omt &source, const tripoint_abs_omt &dest,
                           const omt_route_params &params );

        /* These functions return the overmap that contains the given
         * overmap terrain coordinate, and the local coordinates of that point
         * within the overmap (for use with overmap APIs).
         * get_existing_om_global will not create a new overmap and
         * if the requested overmap does not yet exist it returns
         * { nullptr, tripoint_zero }.
         * get_om_global creates a new overmap if needed.
         */
        overmap_with_local_coords get_existing_om_global( const point_abs_omt &p );
        overmap_with_local_coords get_existing_om_global( const tripoint_abs_omt &p );
        overmap_with_local_coords get_om_global( const point_abs_omt &p );
        overmap_with_local_coords get_om_global( const tripoint_abs_omt &p );

        /**
         * Pass global overmap coordinates (same as @ref get).
         * @returns true if the buffer has a overmap with
         * the given coordinates.
         */
        bool has( const point_abs_om &p );
        /**
         * Get an existing overmap, does not create a new one
         * and may return NULL if the requested overmap does not
         * exist.
         * (x,y) are global overmap coordinates (same as @ref get).
         */
        overmap *get_existing( const point_abs_om &p );
        /**
         * Returns whether or not the location has been generated (e.g. mapgen has run).
         * @param loc is in world-global omt coordinates.
         * @returns True if the location has been generated.
         */
        bool is_omt_generated( const tripoint_abs_omt &loc );

        using t_point_with_note = std::pair<point_abs_omt, std::string>;
        using t_notes_vector = std::vector<t_point_with_note>;
        t_notes_vector get_all_notes( int z ) {
            return get_notes( z, nullptr ); // NULL => don't filter notes
        }
        t_notes_vector find_notes( int z, const std::string &pattern ) {
            return get_notes( z, &pattern ); // filter with pattern
        }
        using t_point_with_extra = std::pair<point_abs_omt, string_id<map_extra>>;
        using t_extras_vector = std::vector<t_point_with_extra>;
        t_extras_vector get_all_extras( int z ) {
            return get_extras( z, nullptr ); // NULL => don't filter extras
        }
        t_extras_vector find_extras( int z, const std::string &pattern ) {
            return get_extras( z, &pattern ); // filter with pattern
        }
        /**
         * Signal nearby hordes to move to given location.
         * @param center The origin of the signal, hordes (that recognize the signal) want to go
         * to there. In global submap coordinates.
         * @param sig_power The signal strength, higher values means it visible farther away.
         */
        void signal_hordes( const tripoint_abs_sm &center, int sig_power );
        /**
         * Process nearby monstergroups (dying mostly).
         */
        void process_mongroups();
        /**
         * Let hordes move a step. Note that this may move monster groups inside the reality bubble,
         * therefore you should probably call @ref map::spawn_monsters to spawn them.
         */
        void move_hordes();
        // hordes -- this uses overmap terrain coordinates!
        std::vector<mongroup *> monsters_at( const tripoint_abs_omt &p );
        /**
         * Monster groups at p - absolute submap coordinates.
         * Groups with no population are not included.
         */
        std::vector<mongroup *> groups_at( const tripoint_abs_sm &p );

        /**
         * Spawn monsters from the overmap onto the main map (game::m).
         * p is an absolute *submap* coordinate.
         */
        void spawn_monster( const tripoint_abs_sm &p );
        /**
         * Despawn the monster back onto the overmap. The monsters position
         * (monster::pos()) is interpreted as relative to the main map.
         */
        void despawn_monster( const monster &critter );
        /**
         * Find radio station with given frequency, search an unspecified area around
         * the current player location.
         * If no matching tower has been found, it returns an object with the tower pointer set
         * to null.
         */
        radio_tower_reference find_radio_station( int frequency );
        /**
         * Find all radio stations that can be received around the current player location.
         * All entries in the returned vector are valid (have a valid tower pointer).
         */
        std::vector<radio_tower_reference> find_all_radio_stations();
        /**
         * Find all cities within the specified @ref radius.
         * Result is sorted by proximity to @ref location in ascending order.
         */
        std::vector<city_reference> get_cities_near( const tripoint_abs_sm &location, int radius );
        /**
         * Find the closest city. If no city is close, returns an object with city set to nullptr.
         * @param center The center of the search, the distance for determining the closest city is
         * calculated as distance to this point. In global submap coordinates!
         */
        city_reference closest_city( const tripoint_abs_sm &center );

        city_reference closest_known_city( const tripoint_abs_sm &center );

        std::string get_description_at( const tripoint_abs_sm &where );

        /**
         * Place the specified overmap special directly on the map using the provided location and rotation.
         * Intended to be used when you have a special in hand, the desired location and rotation are known
         * and the special should be directly placed rather than using the overmap's placement algorithm.
         * @param special The overmap special to place.
         * @param p The location to place the overmap special. Absolute overmap terrain coordinates.
         * @param dir The direction to rotate the overmap special before placement.
         * @param must_be_unexplored If true, will require that all of the terrains where the special would be
         * placed are unexplored.
         * @param force If true, placement will bypass the checks for valid placement.
         * @returns If the special was placed, a vector of the points used, else nullopt.
         */
        std::optional<std::vector<tripoint_abs_omt>> place_special(
                    const overmap_special &special, const tripoint_abs_omt &origin,
                    om_direction::type dir, bool must_be_unexplored, bool force );
        /**
         * Place the specified overmap special using the overmap's placement algorithm. Intended to be used
         * when you have a special that you want placed but it should be placed similarly to as if it were
         * created during overmap generation.
         * @param special_id The id of overmap special to place.
         * @param center Used in conjunction with radius to search the specified and adjacent overmaps for
         * a valid placement location. Absolute overmap terrain coordinates.
         * @param min_radius Used in conjunction with center. Absolute overmap terrain units.
         * @param max_radius Used in conjunction with center. Absolute overmap terrain units.
         * @returns True if the special was placed, else false.
         */
        bool place_special( const overmap_special_id &special_id, const tripoint_abs_omt &center,
                            int min_radius, int max_radius );

    private:
        std::shared_mutex mutex;
        /**
         * Common function used by the find_closest/all/random to determine if the location is
         * findable based on the specified criteria.
         * @param location Location of search
         * see omt_find_params for definitions of the terms
         */
        bool is_findable_location( const tripoint_abs_omt &location, const omt_find_params &params );
        bool is_findable_location( const overmap_with_local_coords &map_loc,
                                   const omt_find_params &params );

        std::unordered_map< point_abs_om, std::unique_ptr< overmap > > overmaps;
        /**
         * Set of overmap coordinates of overmaps that are known
         * to not exist on disk. See @ref get_existing for usage.
         */
        std::set<point_abs_om> known_non_existing;

        // Set of globally unique overmap specials that have already been placed
        std::unordered_set<overmap_special_id> placed_unique_specials;

        /**
         * Get a list of notes in the (loaded) overmaps.
         * @param z only this specific z-level is search for notes.
         * @param pattern only notes that contain this pattern are returned.
         * If the pattern is NULL, every note matches.
         */
        t_notes_vector get_notes( int z, const std::string *pattern );
        /**
         * Get a list of map extras in the (loaded) overmaps.
         * @param z only this specific z-level is search for map extras.
         * @param pattern only map extras that contain this pattern are returned.
         * If the pattern is NULL, every map extra matches.
         */
        t_extras_vector get_extras( int z, const std::string *pattern );
    public:
        /**
         * See overmap::check_ot, this uses global
         * overmap terrain coordinates.
         * This function may create a new overmap if needed.
         */
        bool check_ot( const std::string &otype, ot_match_type match_type,
                       const tripoint_abs_omt &p );
        bool check_overmap_special_type( const overmap_special_id &id, const tripoint_abs_omt &loc );
        std::optional<overmap_special_id> overmap_special_at( const tripoint_abs_omt & );

        /**
        * These versions of the check_* methods will only check existing overmaps, and
        * return false if the overmap doesn't exist. They do not create new overmaps.
        */
        bool check_ot_existing( const std::string &otype, ot_match_type match_type,
                                const tripoint_abs_omt &loc );
        bool check_overmap_special_type_existing( const overmap_special_id &id,
                const tripoint_abs_omt &loc );

        /**
         * Adds the given globally unique overmap special to the list of placed specials.
         */
        void add_unique_special( const overmap_special_id &id );
        /**
         * Returns true if the given globally unique overmap special has already been placed.
         */
        bool contains_unique_special( const overmap_special_id &id ) const;
        /**
         * Writes the placed unique specials as a JSON value.
         */
        void serialize_placed_unique_specials( JsonOut &json ) const;
        /**
         * Reads placed unique specials from JSON and overwrites the global value.
         */
        void deserialize_placed_unique_specials( JsonIn &jsin );

    private:
        /**
         * Go thorough the monster groups of the overmap and move out-of-bounds
         * groups to the correct overmap (if it exists), also removes empty groups.
         */
        void fix_mongroups( overmap &new_overmap );
        /**
         * Moves out-of-bounds NPCs to the overmaps they should be in.
         */
        void fix_npcs( overmap &new_overmap );
        /**
         * Retrieve overmaps that overlap the bounding box defined by the location and radius.
         * The location is in absolute submap coordinates, the radius is in the same system.
         * The overmaps are returned sorted by distance from the provided location (closest first).
         */
        std::vector<overmap *> get_overmaps_near( const point_abs_sm &p, int radius );
        std::vector<overmap *> get_overmaps_near( const tripoint_abs_sm &location, int radius );

    public:
        /**
         * Gets the electric grid to which a point belongs by walking along the connected tiles.
         * Will always return a non-empty set.
         * @param p Overmap coordinates of the point in the grid
         */
        std::set<tripoint_abs_omt> electric_grid_at( const tripoint_abs_omt &p );

        /**
         * Retrieve electric grid connections from given point.
         * Returned vector may be empty.
         */
        std::vector<tripoint_rel_omt> electric_grid_connectivity_at( const tripoint_abs_omt &p );

        /**
         * Adds a grid connection between two points. The points must be adjacent.
        */
        bool add_grid_connection( const tripoint_abs_omt &lhs, const tripoint_abs_omt &rhs );

        /**
         * Removes a grid connection between two points. The points must be adjacent.
         * Does not fully isolate the points, they can still be in the same grid
         * after this operation.
        */
        bool remove_grid_connection( const tripoint_abs_omt &lhs, const tripoint_abs_omt &rhs );
};

extern overmapbuffer overmap_buffer;


