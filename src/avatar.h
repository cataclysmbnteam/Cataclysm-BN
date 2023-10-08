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
class diary;
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
        avatar( avatar && ) noexcept;
        ~avatar() override;
        avatar &operator=( const avatar & ) = delete;
        avatar &operator=( avatar && ) noexcept;

        void store( JsonOut &json ) const;
        void load( const JsonObject &data );
        void serialize( JsonOut &json ) const override;
        void deserialize( JsonIn &jsin ) override;
        bool save_map_memory();
        void load_map_memory();

        // newcharacter.cpp
        bool create( character_type type, const std::string &tempname = "" );
        void randomize( bool random_scenario, points_left &points, bool play_now = false );
        bool load_template( const std::string &template_name, points_left &points );
        void save_template( const std::string &name, const points_left &points );
        void character_to_template( const std::string &name );

        bool is_avatar() const override {
            return true;
        }
        avatar *as_avatar() override {
            return this;
        }
        const avatar *as_avatar() const override {
            return this;
        }

        std::string get_save_id() const {
            return save_id.empty() ? name : save_id;
        }
        void set_save_id( const std::string &id ) {
            save_id = id;
        }

        void toggle_map_memory();
        bool should_show_map_memory();
        void prepare_map_memory_region( const tripoint &p1, const tripoint &p2 );
        /** Memorizes a given tile in tiles mode; finalize_tile_memory needs to be called after it */
        void memorize_tile( const tripoint &pos, const std::string &ter, int subtile,
                            int rotation );
        /** Returns last stored map tile in given location in tiles mode */
        const memorized_terrain_tile &get_memorized_tile( const tripoint &p ) const;
        /** Memorizes a given tile in curses mode; finalize_terrain_memory_curses needs to be called after it */
        void memorize_symbol( const tripoint &pos, int symbol );
        /** Returns last stored map tile in given location in curses mode */
        int get_memorized_symbol( const tripoint &p ) const;
        void clear_memorized_tile( const tripoint &pos );
        /** Returns last stored map tile in given location in tiles mode */
        bool has_memorized_tile_for_autodrive( const tripoint &p ) const;

        /** Provides the window and detailed morale data */
        void disp_morale();
        /** Resets all missions before saving character to template */
        void reset_all_missions();

        std::vector<mission *> get_active_missions() const;
        std::vector<mission *> get_completed_missions() const;
        std::vector<mission *> get_failed_missions() const;
        /**
         * Returns the mission that is currently active. Returns null if mission is active.
         */
        mission *get_active_mission() const;
        /**
         * Returns the target of the active mission or @ref overmap::invalid_tripoint if there is
         * no active mission.
         */
        tripoint_abs_omt get_active_mission_target() const;
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

        // return avatar diary
        diary *get_avatar_diary();

        /**
         * Helper function for player::read.
         *
         * @param book Book to read
         * @param reasons Starting with g->u, for each player/NPC who cannot read, a message will be pushed back here.
         * @returns nullptr, if neither the player nor his followers can read to the player, otherwise the player/NPC
         * who can read and can read the fastest
         */
        const player *get_book_reader( const item &book, std::vector<std::string> &reasons ) const;
        /**
         * Helper function for get_book_reader
         * @warning This function assumes that the everyone is able to read
         *
         * @param book The book being read
         * @param reader the player/NPC who's reading to the caller
         * @param learner if not nullptr, assume that the caller and reader read at a pace that isn't too fast for him
         */
        int time_to_read( const item &book, const player &reader, const player *learner = nullptr ) const;
        /** Handles reading effects and returns true if activity started */
        bool read( item *loc, bool continuous = false );
        /** Completes book reading action. **/
        void do_read( item *loc );
        /** Note that we've read a book at least once. **/
        bool has_identified( const itype_id &item_id ) const;

        void wake_up();
        // Grab furniture / vehicle
        void grab( object_type grab_type, const tripoint &grab_point = tripoint_zero );
        object_type get_grab_type() const;
        /** Handles player vomiting effects */
        void vomit();

        bool is_hallucination() const override;

        pimpl<teleporter_list> translocators;

        int get_str_base() const override;
        int get_dex_base() const override;
        int get_int_base() const override;
        int get_per_base() const override;

        // how many points are available to upgrade via STK
        int free_upgrade_points() const;
        // how much "kill xp" you have
        int kill_xp() const;
        // how much "kill xp" needed for next point (empty if reached max level)
        std::optional<int> kill_xp_for_next_point() const;
        // upgrade stat from kills
        void upgrade_stat( character_stat stat );

        faction *get_faction() const override;
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

        bool wield( item &target ) override;
        detached_ptr<item> wield( detached_ptr<item> &&target ) override;

        /**
         * Add warning from faction.
         * @returns true if the warning is now beyond final and results in hostility
         */
        bool add_faction_warning( const faction_id &id );

        using Character::invoke_item;
        bool invoke_item( item *, const tripoint &pt ) override;
        bool invoke_item( item * ) override;
        bool invoke_item( item *, const std::string &, const tripoint &pt ) override;
        bool invoke_item( item *, const std::string & ) override;

        monster_visible_info &get_mon_visible() {
            return mon_visible;
        }

    private:
        // The name used to generate save filenames for this avatar. Not serialized in json.
        std::string save_id;

        std::unique_ptr<map_memory> player_map_memory;
        bool show_map_memory = true;

        friend class debug_menu::mission_debug;
        /**
         * Missions that the player has accepted and that are not finished (one
         * way or the other).
         */
        std::vector<mission *> active_missions;
        /**
        * Diary to track player progression and to write the player's story
        */
        std::unique_ptr <diary> a_diary;
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

        /** Warnings from factions about bad behavior */
        std::map<faction_id, std::pair<int, time_point>> warning_record;
};

avatar &get_avatar();

#endif // CATA_SRC_AVATAR_H
