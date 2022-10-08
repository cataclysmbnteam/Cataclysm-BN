#pragma once
#ifndef CATA_SRC_BASECAMP_H
#define CATA_SRC_BASECAMP_H

#include <cstddef>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "craft_command.h"
#include "coordinates.h"
#include "inventory.h"
#include "memory_fast.h"
#include "optional.h"
#include "point.h"
#include "requirements.h"
#include "translations.h"
#include "type_id.h"

class JsonIn;
class JsonOut;
class character_id;
class npc;
class time_duration;

enum class farm_ops;
class item;
class Item_group;
class mission_data;
class recipe;
class tinymap;

struct expansion_data {
    std::string type;
    std::map<std::string, int> provides;
    std::map<std::string, int> in_progress;
    tripoint_abs_omt pos;
    // legacy camp level, replaced by provides map and set to -1
    int cur_level;

};

using npc_ptr = shared_ptr_fast<npc>;
using comp_list = std::vector<npc_ptr>;

namespace catacurses
{
class window;
} // namespace catacurses

namespace base_camps
{

enum tab_mode : int {
    TAB_MAIN,
    TAB_N,
    TAB_NE,
    TAB_E,
    TAB_SE,
    TAB_S,
    TAB_SW,
    TAB_W,
    TAB_NW
};

struct direction_data {
    // used for composing mission ids
    std::string id;
    // tab order
    tab_mode tab_order;
    // such as [B], [NW], etc
    translation bracket_abbr;
    // MAIN, [NW], etc
    translation tab_title;
};

// base_dir and the eight directional points
extern const std::map<point, direction_data> all_directions;

auto direction_from_id( const std::string &id ) -> point;

const point base_dir;
const std::string prefix = "faction_base_";
const std::string id = "FACTION_CAMP";
const int prefix_len = 13;
auto faction_encode_short( const std::string &type ) -> std::string;
auto faction_encode_abs( const expansion_data &e, int number ) -> std::string;
auto faction_decode( const std::string &full_type ) -> std::string;
auto to_workdays( const time_duration &work_time ) -> time_duration;
auto max_upgrade_by_type( const std::string &type ) -> int;
} // namespace base_camps

// camp resource structures
struct basecamp_resource {
    itype_id fake_id;
    itype_id ammo_id;
    int available = 0;
    int consumed = 0;
};

struct basecamp_fuel {
    itype_id ammo_id;
    int available = 0;
};

struct basecamp_upgrade {
    std::string bldg;
    translation name;
    bool avail = false;
    bool in_progress = false;
};

class basecamp
{
    public:
        basecamp();
        basecamp( const std::string &name_, const tripoint_abs_omt &omt_pos );
        basecamp( const std::string &name_, const tripoint &bb_pos_,
                  const std::vector<point> &directions_,
                  const std::map<point, expansion_data> &expansions_ );

        inline auto is_valid() const -> bool {
            return !name.empty() && omt_pos != tripoint_abs_omt();
        }
        inline auto board_x() const -> int {
            return bb_pos.x;
        }
        inline auto board_y() const -> int {
            return bb_pos.y;
        }
        inline auto camp_omt_pos() const -> tripoint_abs_omt {
            return omt_pos;
        }
        inline auto camp_name() const -> const std::string & {
            return name;
        }
        auto get_bb_pos() const -> tripoint {
            return bb_pos;
        }
        void validate_bb_pos( const tripoint &new_abs_pos ) {
            if( bb_pos == tripoint_zero ) {
                bb_pos = new_abs_pos;
            }
        }
        void set_bb_pos( const tripoint &new_abs_pos ) {
            bb_pos = new_abs_pos;
        }
        void set_by_radio( bool access_by_radio );

        auto board_name() const -> std::string;
        std::vector<point> directions;
        std::vector<tripoint_abs_omt> fortifications;
        std::string name;
        void faction_display( const catacurses::window &fac_w, int width ) const;

        //change name of camp
        void set_name( const std::string &new_name );
        void query_new_name();
        void abandon_camp();
        void add_expansion( const std::string &terrain, const tripoint_abs_omt &new_pos );
        void add_expansion( const std::string &bldg, const tripoint_abs_omt &new_pos,
                            const point &dir );
        void define_camp( const tripoint_abs_omt &p, const std::string &camp_type = "default" );

        auto expansion_tab( const point &dir ) const -> std::string;
        // upgrade levels
        auto has_provides( const std::string &req, const expansion_data &e_data, int level = 0 ) const -> bool;
        auto has_provides( const std::string &req, const cata::optional<point> &dir = cata::nullopt,
                           int level = 0 ) const -> bool;
        void update_resources( const std::string &bldg );
        void update_provides( const std::string &bldg, expansion_data &e_data );
        void update_in_progress( const std::string &bldg, const point &dir );

        auto can_expand() -> bool;
        /// Returns the name of the building the current building @ref dir upgrades into,
        /// "null" if there isn't one
        auto next_upgrade( const point &dir, int offset = 1 ) const -> std::string;
        auto available_upgrades( const point &dir ) -> std::vector<basecamp_upgrade>;

        // camp utility functions
        auto recruit_evaluation() const -> int;
        auto recruit_evaluation( int &sbase, int &sexpansions, int &sfaction, int &sbonus ) const -> int;
        // confirm there is at least 1 loot destination and 1 unsorted loot zone in the camp
        auto validate_sort_points() -> bool;
        // Validates the expansion data
        auto parse_expansion( const std::string &terrain,
                                        const tripoint_abs_omt &new_pos ) -> expansion_data;
        /**
         * Invokes the zone manager and validates that the necessary sort zones exist.
         */
        auto set_sort_points() -> bool;

        // food utility
        /// Takes all the food from the camp_food zone and increases the faction
        /// food_supply
        auto distribute_food() -> bool;
        auto has_water() -> bool;

        // recipes, gathering, and craft support functions
        // from a direction
        auto recipe_deck( const point &dir ) const -> std::map<recipe_id, translation>;
        // from a building
        auto recipe_deck( const std::string &bldg ) const -> std::map<recipe_id, translation>;
        auto recipe_batch_max( const recipe &making ) const -> int;
        void form_crafting_inventory();
        void form_crafting_inventory( map &target_map );
        auto use_charges( const itype_id &fake_id, int &quantity ) -> std::list<item>;
        auto get_gatherlist() const -> item_group_id;
        /**
         * spawn items or corpses based on search attempts
         * @param skill skill level of the search
         * @param group_id name of the item_group that provides the items
         * @param attempts number of skill checks to make
         * @param difficulty a random number from 0 to difficulty is created for each attempt, and
         * if skill is higher, an item or corpse is spawned
         */
        void search_results( int skill, const item_group_id &group_id, int attempts, int difficulty );
        /**
         * spawn items or corpses based on search attempts
         * @param skill skill level of the search
         * @param task string to identify what types of corpses to provide ( _faction_camp_hunting
         * or _faction_camp_trapping )
         * @param attempts number of skill checks to make
         * @param difficulty a random number from 0 to difficulty is created for each attempt, and
         * if skill is higher, an item or corpse is spawned
         */
        void hunting_results( int skill, const std::string &task, int attempts, int difficulty );
        inline auto get_dumping_spot() const -> const tripoint & {
            return dumping_spot;
        }
        // dumping spot in absolute co-ords
        inline void set_dumping_spot( const tripoint &spot ) {
            dumping_spot = spot;
        }
        void place_results( item result );

        // mission description functions
        void add_available_recipes( mission_data &mission_key, const point &dir,
                                    const std::map<recipe_id, translation> &craft_recipes );

        auto recruit_description( int npc_count ) -> std::string;
        /// Provides a "guess" for some of the things your gatherers will return with
        /// to upgrade the camp
        auto gathering_description( const std::string &bldg ) -> std::string;
        /// Returns a string for the number of plants that are harvestable, plots ready to plant,
        /// and ground that needs tilling
        auto farm_description( const tripoint_abs_omt &farm_pos, size_t &plots_count,
                                      farm_ops operation ) -> std::string;
        /// Returns the description of a camp crafting options. converts fire charges to charcoal,
        /// allows dark crafting
        auto craft_description( const recipe_id &itm ) -> std::string;

        // main mission description collection
        void get_available_missions( mission_data &mission_key );
        void get_available_missions_by_dir( mission_data &mission_key, const point &dir );
        // available companion list manipulation
        void reset_camp_workers();
        auto get_mission_workers( const std::string &mission_id, bool contains = false ) -> comp_list;
        // main mission start/return dispatch function
        auto handle_mission( const std::string &miss_id, const cata::optional<point> &opt_miss_dir ) -> bool;

        // mission start functions
        /// generic mission start function that wraps individual mission
        auto start_mission( const std::string &miss_id, time_duration duration,
                               bool must_feed, const std::string &desc, bool group,
                               const std::vector<item *> &equipment,
                               const skill_id &skill_tested, int skill_level ) -> npc_ptr;
        auto start_mission( const std::string &miss_id, time_duration duration,
                               bool must_feed, const std::string &desc, bool group,
                               const std::vector<item *> &equipment,
                               const std::map<skill_id, int> &required_skills = {} ) -> npc_ptr;
        void start_upgrade( const std::string &bldg, const point &dir, const std::string &key );
        auto om_upgrade_description( const std::string &bldg, bool trunc = false ) const -> std::string;
        void start_menial_labor();
        void worker_assignment_ui();
        void job_assignment_ui();
        void start_crafting( const std::string &cur_id, const point &cur_dir,
                             const std::string &type, const std::string &miss_id );

        /// Called when a companion is sent to cut logs
        void start_cut_logs();
        void start_clearcut();
        void start_setup_hide_site();
        void start_relay_hide_site();
        /// Called when a compansion is sent to start fortifications
        void start_fortifications( std::string &bldg_exp );
        void start_combat_mission( const std::string &miss );
        /// Called when a companion starts a chop shop @ref task mission
        auto start_garage_chop( const point &dir, const tripoint_abs_omt &omt_tgt ) -> bool;
        void start_farm_op( const point &dir, const tripoint_abs_omt &omt_tgt, farm_ops op );
        ///Display items listed in @ref equipment to let the player pick what to give the departing
        ///NPC, loops until quit or empty.
        auto give_equipment( std::vector<item *> equipment, const std::string &msg ) -> std::vector<item *>;

        // mission return functions
        /// called to select a companion to return to the base
        auto companion_choose_return( const std::string &miss_id, time_duration min_duration ) -> npc_ptr;
        /// called with a companion @ref comp who is not the camp manager, finishes updating their
        /// skills, consuming food, and returning them to the base.
        void finish_return( npc &comp, bool fixed_time, const std::string &return_msg,
                            const std::string &skill, int difficulty, bool cancel = false );
        /// a wrapper function for @ref companion_choose_return and @ref finish_return
        auto mission_return( const std::string &miss_id, time_duration min_duration,
                                bool fixed_time, const std::string &return_msg,
                                const std::string &skill, int difficulty ) -> npc_ptr;
        /// select a companion for any mission to return to base
        auto emergency_recall() -> npc_ptr;

        /// Called to close upgrade missions, @ref miss is the name of the mission id
        /// and @ref dir is the direction of the location to be upgraded
        auto upgrade_return( const point &dir, const std::string &miss ) -> bool;
        /// As above, but with an explicit blueprint recipe to upgrade
        auto upgrade_return( const point &dir, const std::string &miss, const std::string &bldg ) -> bool;

        /// Choose which expansion you should start, called when a survey mission is completed
        auto survey_return() -> bool;
        auto menial_return() -> bool;
        /// Called when a companion completes a gathering @ref task mission
        auto gathering_return( const std::string &task, time_duration min_time ) -> bool;
        void recruit_return( const std::string &task, int score );
        /**
        * Perform any mix of the three farm tasks.
        * @param task
        * @param omt_tgt the overmap pos3 of the farm_ops
        * @param op whether to plow, plant, or harvest
        */
        auto farm_return( const std::string &task, const tripoint_abs_omt &omt_tgt, farm_ops op ) -> bool;
        void fortifications_return();

        void combat_mission_return( const std::string &miss );
        void validate_assignees();
        void add_assignee( character_id id );
        void remove_assignee( character_id id );
        auto get_npcs_assigned() -> std::vector<npc_ptr>;
        // Save/load
        void serialize( JsonOut &json ) const;
        void deserialize( JsonIn &jsin );
        void load_data( const std::string &data );

        static constexpr int inv_range = 20;
    private:
        friend class basecamp_action_components;

        // lazy re-evaluation of available camp resources
        void reset_camp_resources();
        void add_resource( const itype_id &camp_resource );
        bool resources_updated = false;
        // omt pos
        tripoint_abs_omt omt_pos;
        std::vector<npc_ptr> assigned_npcs;
        // location of associated bulletin board in abs coords
        tripoint bb_pos;
        std::map<point, expansion_data> expansions;
        comp_list camp_workers;
        tripoint dumping_spot;

        std::set<itype_id> fuel_types;
        std::vector<basecamp_fuel> fuels;
        std::vector<basecamp_resource> resources;
        inventory _inv;
        bool by_radio = false;
};

class basecamp_action_components
{
    public:
        basecamp_action_components( const recipe &making, int batch_size, basecamp & );

        // Returns true iff all necessary components were successfully chosen
        auto choose_components() -> bool;
        void consume_components();
    private:
        const recipe &making_;
        int batch_size_;
        basecamp &base_;
        std::vector<comp_selection<item_comp>> item_selections_;
        std::vector<comp_selection<tool_comp>> tool_selections_;
        std::unique_ptr<tinymap> map_; // Used for by-radio crafting
};

#endif // CATA_SRC_BASECAMP_H
