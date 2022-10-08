#pragma once
#ifndef CATA_SRC_AVATAR_H
#define CATA_SRC_AVATAR_H

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "calendar.h"
#include "character.h"
#include "enums.h"
#include "item.h"
#include "player.h"
#include "pldata.h"
#include "point.h"

class JsonIn;
class JsonObject;
class JsonOut;
class faction;
class mission;
class monster;
class npc;
class map_memory;
struct memorized_terrain_tile;

namespace debug_menu
{
class mission_debug;
}  // namespace debug_menu
struct mtype;
struct points_left;
class teleporter_list;

// Monster visible in different directions (safe mode & compass)
struct monster_visible_info {
    // New monsters visible from last update
    std::vector<shared_ptr_fast<monster>> new_seen_mon;

    // Unique monsters (and types of monsters) visible in different directions
    // 7 0 1    unique_types uses these indices;
    // 6 8 2    0-7 are provide by direction_from()
    // 5 4 3    8 is used for local monsters (for when we explain them below)
    std::vector<npc *> unique_types[9];
    std::vector<std::pair<const mtype *, int>> unique_mons[9];

    // If the moster visible in this direction is dangerous
    bool dangerous[8] = {};
};

class avatar : public player
{
    public:
        avatar();
        avatar( const avatar & ) = delete;
        avatar( avatar && );
        ~avatar() override;
        auto operator=( const avatar & ) -> avatar & = delete;
        auto operator=( avatar && ) -> avatar &;

        void store( JsonOut &json ) const;
        void load( const JsonObject &data );
        void serialize( JsonOut &json ) const override;
        void deserialize( JsonIn &jsin ) override;
        auto save_map_memory() -> bool;
        void load_map_memory();

        // newcharacter.cpp
        auto create( character_type type, const std::string &tempname = "" ) -> bool;
        void randomize( bool random_scenario, points_left &points, bool play_now = false );
        auto load_template( const std::string &template_name, points_left &points ) -> bool;
        void save_template( const std::string &name, const points_left &points );

        auto is_avatar() const -> bool override {
            return true;
        }
        auto as_avatar() -> avatar * override {
            return this;
        }
        auto as_avatar() const -> const avatar * override {
            return this;
        }

        void toggle_map_memory();
        auto should_show_map_memory() -> bool;
        void prepare_map_memory_region( const tripoint &p1, const tripoint &p2 );
        /** Memorizes a given tile in tiles mode; finalize_tile_memory needs to be called after it */
        void memorize_tile( const tripoint &pos, const std::string &ter, int subtile,
                            int rotation );
        /** Returns last stored map tile in given location in tiles mode */
        auto get_memorized_tile( const tripoint &p ) const -> const memorized_terrain_tile &;
        /** Memorizes a given tile in curses mode; finalize_terrain_memory_curses needs to be called after it */
        void memorize_symbol( const tripoint &pos, int symbol );
        /** Returns last stored map tile in given location in curses mode */
        auto get_memorized_symbol( const tripoint &p ) const -> int;
        void clear_memorized_tile( const tripoint &pos );
        /** Returns last stored map tile in given location in tiles mode */
        auto has_memorized_tile_for_autodrive( const tripoint &p ) const -> bool;

        /** Provides the window and detailed morale data */
        void disp_morale();
        /** Resets all missions before saving character to template */
        void reset_all_misions();

        auto get_active_missions() const -> std::vector<mission *>;
        auto get_completed_missions() const -> std::vector<mission *>;
        auto get_failed_missions() const -> std::vector<mission *>;
        /**
         * Returns the mission that is currently active. Returns null if mission is active.
         */
        auto get_active_mission() const -> mission *;
        /**
         * Returns the target of the active mission or @ref overmap::invalid_tripoint if there is
         * no active mission.
         */
        auto get_active_mission_target() const -> tripoint_abs_omt;
        /**
         * Set which mission is active. The mission must be listed in @ref active_missions.
         */
        void set_active_mission( mission &cur_mission );
        /**
         * Called when a mission has been assigned to the player.
         */
        void on_mission_assignment( mission &new_mission );
        /**
         * Called when a mission has been completed or failed. Either way it's finished.
         * Check @ref mission::has_failed to see which case it is.
         */
        void on_mission_finished( mission &cur_mission );

        /**
         * Helper function for player::read.
         *
         * @param book Book to read
         * @param reasons Starting with g->u, for each player/NPC who cannot read, a message will be pushed back here.
         * @returns nullptr, if neither the player nor his followers can read to the player, otherwise the player/NPC
         * who can read and can read the fastest
         */
        auto get_book_reader( const item &book, std::vector<std::string> &reasons ) const -> const player *;
        /**
         * Helper function for get_book_reader
         * @warning This function assumes that the everyone is able to read
         *
         * @param book The book being read
         * @param reader the player/NPC who's reading to the caller
         * @param learner if not nullptr, assume that the caller and reader read at a pace that isn't too fast for him
         */
        auto time_to_read( const item &book, const player &reader, const player *learner = nullptr ) const -> int;
        /** Handles reading effects and returns true if activity started */
        auto read( item_location loc, bool continuous = false ) -> bool;
        /** Completes book reading action. **/
        void do_read( item_location loc );
        /** Note that we've read a book at least once. **/
        auto has_identified( const itype_id &item_id ) const -> bool override;

        void wake_up();
        // Grab furniture / vehicle
        void grab( object_type grab_type, const tripoint &grab_point = tripoint_zero );
        auto get_grab_type() const -> object_type;
        /** Handles player vomiting effects */
        void vomit();

        /**
         * Try to steal an item from the NPC's inventory. May result in fail attempt, when NPC not notices you,
         * notices your steal attempt and getting angry with you, and you successfully stealing the item.
         * @param target Target NPC to steal from
         */
        void steal( npc &target );

        pimpl<teleporter_list> translocators;

        auto get_str_base() const -> int override;
        auto get_dex_base() const -> int override;
        auto get_int_base() const -> int override;
        auto get_per_base() const -> int override;

        // how many points are available to upgrade via STK
        auto free_upgrade_points() const -> int;
        // how much "kill xp" you have
        auto kill_xp() const -> int;
        // how much "kill xp" needed for next point (empty if reached max level)
        auto kill_xp_for_next_point() const -> cata::optional<int>;
        // upgrade stat from kills
        void upgrade_stat( character_stat stat );

        auto get_faction() const -> faction * override;
        // Set in npc::talk_to_you for use in further NPC interactions
        bool dialogue_by_radio = false;
        // Preferred aim mode - ranged.cpp aim mode defaults to this if possible
        std::string preferred_aiming_mode;

        void set_movement_mode( character_movemode mode ) override;

        // Cycles to the next move mode.
        void cycle_move_mode();
        // Resets to walking.
        void reset_move_mode();
        // Toggles running on/off.
        void toggle_run_mode();
        // Toggles crouching on/off.
        void toggle_crouch_mode();

        auto wield( item &target ) -> bool override;

        using Character::invoke_item;
        auto invoke_item( item *, const tripoint &pt ) -> bool override;
        auto invoke_item( item * ) -> bool override;
        auto invoke_item( item *, const std::string &, const tripoint &pt ) -> bool override;
        auto invoke_item( item *, const std::string & ) -> bool override;

        auto get_mon_visible() -> monster_visible_info & {
            return mon_visible;
        }

    private:
        std::unique_ptr<map_memory> player_map_memory;
        bool show_map_memory = true;

        friend class debug_menu::mission_debug;
        /**
         * Missions that the player has accepted and that are not finished (one
         * way or the other).
         */
        std::vector<mission *> active_missions;
        /**
         * Missions that the player has successfully completed.
         */
        std::vector<mission *> completed_missions;
        /**
         * Missions that have failed while being assigned to the player.
         */
        std::vector<mission *> failed_missions;
        /**
         * The currently active mission, or null if no mission is currently in progress.
         */
        mission *active_mission = nullptr;

        // Items the player has identified.
        std::unordered_set<itype_id> items_identified;

        object_type grab_type = OBJECT_NONE;

        // these are the stat upgrades from stats through kills

        int str_upgrade = 0;
        int dex_upgrade = 0;
        int int_upgrade = 0;
        int per_upgrade = 0;

        monster_visible_info mon_visible;
};

auto get_avatar() -> avatar &;

#endif // CATA_SRC_AVATAR_H
