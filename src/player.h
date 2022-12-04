#pragma once
#ifndef CATA_SRC_PLAYER_H
#define CATA_SRC_PLAYER_H

#include <climits>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "bodypart.h"
#include "calendar.h"
#include "character.h"
#include "character_id.h"
#include "color.h"
#include "creature.h"
#include "cursesdef.h"
#include "enums.h"
#include "game_constants.h"
#include "item.h"
#include "item_location.h"
#include "optional.h"
#include "pimpl.h"
#include "point.h"
#include "ret_val.h"
#include "safe_reference.h"
#include "string_id.h"
#include "type_id.h"

class basecamp;
class craft_command;
class effect;
class faction;
class inventory;
class map;
class npc;
class recipe;
struct damage_unit;
struct requirement_data;

enum class recipe_filter_flags : int;
struct itype;

static const std::string
DEFAULT_HOTKEYS( "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" );

class recipe_subset;

enum action_id : int;
class JsonIn;
class JsonObject;
class JsonOut;
class dispersion_sources;
struct bionic;
struct dealt_projectile_attack;
class profession;
struct trap;

nc_color encumb_color( int level );
enum game_message_type : int;
class vehicle;
struct item_comp;
struct tool_comp;
struct w_point;

using recipe_filter = std::function<bool( const recipe &r )>;

/** @relates ret_val */
template<>
struct ret_val<edible_rating>::default_success : public
    std::integral_constant<edible_rating, edible_rating::edible> {};

/** @relates ret_val */
template<>
struct ret_val<edible_rating>::default_failure : public
    std::integral_constant<edible_rating, edible_rating::inedible> {};

struct needs_rates {
    float thirst = 0.0f;
    float hunger = 0.0f;
    float fatigue = 0.0f;
    float recovery = 0.0f;
};

class player : public Character
{
    public:
        player();
        player( const player & ) = delete;
        player( player && );
        ~player() override;
        player &operator=( const player & ) = delete;
        player &operator=( player && );

        /** Calls Character::normalize()
         *  normalizes HP and body temperature
         */

        void normalize() override;

        bool is_player() const override {
            return true;
        }
        player *as_player() override {
            return this;
        }
        const player *as_player() const override {
            return this;
        }

        /** Processes human-specific effects of effects before calling Creature::process_effects(). */
        void process_effects_internal() override;
        /** Handles the still hard-coded effects. */
        void hardcoded_effects( effect &it );
        /** Returns the modifier value used for vomiting effects. */
        double vomit_mod();

        bool is_npc() const override {
            return false;    // Overloaded for NPCs in npc.h
        }

        /** Returns what color the player should be drawn as */
        nc_color basic_symbol_color() const override;

        // populate variables, inventory items, and misc from json object
        virtual void deserialize( JsonIn &jsin ) = 0;

        // by default save all contained info
        virtual void serialize( JsonOut &jsout ) const = 0;

        /** Resets movement points and applies other non-idempotent changes */
        void process_turn() override;
        /** Calculates the various speed bonuses we will get from mutations, etc. */
        void recalc_speed_bonus();

        /** Maintains body wetness and handles the rate at which the player dries */
        void update_body_wetness( const w_point &weather );

        /** Generates and handles the UI for player interaction with installed bionics */
        void power_bionics();
        void power_mutations();

        /** Returns the bionic with the given invlet, or NULL if no bionic has that invlet */
        bionic *bionic_by_invlet( int ch );

        /** Called when a player triggers a trap, returns true if they don't set it off */
        bool avoid_trap( const tripoint &pos, const trap &tr ) const override;

        void pause(); // '.' command; pauses & resets recoil

        // martialarts.cpp

        /** Returns true if the player can learn the entered martial art */
        bool can_autolearn( const matype_id &ma_id ) const;

        /** Returns value of player's stable footing */
        float stability_roll() const override;
        /** Returns true if the player has stealthy movement */
        bool is_stealthy() const;
        /** Returns true if the current martial art works with the player's current weapon */
        bool can_melee() const;
        /** Returns true if the player should be dead */
        bool is_dead_state() const override;

        /** Returns true if the player is able to use a grab breaking technique */
        bool can_grab_break( const item &weap ) const;
        // melee.cpp

        /**
         * Returns a weapon's modified dispersion value.
         * @param obj Weapon to check dispersion on
         */
        dispersion_sources get_weapon_dispersion( const item &obj ) const;

        /** How many moves does it take to aim gun to the target accuracy. */
        int gun_engagement_moves( const item &gun, int target = 0, int start = MAX_RECOIL ) const;

        /**
         *  Fires a gun or auxiliary gunmod (ignoring any current mode)
         *  @param target where the first shot is aimed at (may vary for later shots)
         *  @param shots maximum number of shots to fire (less may be fired in some circumstances)
         *  @return number of shots actually fired
         */

        int fire_gun( const tripoint &target, int shots = 1 );
        /**
         *  Fires a gun or auxiliary gunmod (ignoring any current mode)
         *  @param target where the first shot is aimed at (may vary for later shots)
         *  @param shots maximum number of shots to fire (less may be fired in some circumstances)
         *  @param gun item to fire (which does not necessary have to be in the players possession)
         *  @return number of shots actually fired
         */
        int fire_gun( const tripoint &target, int shots, item &gun );

        /** Handles reach melee attacks */
        void reach_attack( const tripoint &p );

        /** Called after the player has successfully dodged an attack */
        void on_dodge( Creature *source, float difficulty ) override;
        /** Handles special defenses from an attack that hit us (source can be null) */
        void on_hit( Creature *source, bodypart_id bp_hit,
                     float difficulty = INT_MIN, dealt_projectile_attack const *proj = nullptr ) override;


        /** NPC-related item rating functions */
        double weapon_value( const item &weap, int ammo = 10 ) const; // Evaluates item as a weapon
        double gun_value( const item &weap, int ammo = 10 ) const; // Evaluates item as a gun
        double melee_value( const item &weap ) const; // As above, but only as melee
        double unarmed_value() const; // Evaluate yourself!

        /** Returns the player's dodge_roll to be compared against an aggressor's hit_roll() */
        float dodge_roll() override;

        /** Returns melee skill level, to be used to throttle dodge practice. **/
        float get_melee() const override;

        /** Handles the uncanny dodge bionic and effects, returns true if the player successfully dodges */
        bool uncanny_dodge() override;

        // ranged.cpp
        /** Execute a throw */
        dealt_projectile_attack throw_item( const tripoint &target, const item &to_throw,
                                            const cata::optional<tripoint> &blind_throw_from_pos = cata::nullopt );

        // Mental skills and stats
        /** Returns a value used when attempting to convince NPC's of something */
        int talk_skill() const;
        /** Returns a value used when attempting to intimidate NPC's */
        int intimidation() const;

        /**
         * Check if a given body part is immune to a given damage type
         *
         * This function checks whether a given body part cannot be damaged by a given
         * damage_unit.  Note that this refers only to reduction of hp on that part. It
         * does not account for clothing damage, pain, status effects, etc.
         *
         * @param bp: Body part to perform the check on
         * @param dam: Type of damage to check for
         * @returns true if given damage can not reduce hp of given body part
         */
        bool immune_to( body_part bp, damage_unit dam ) const;
        /** Modifies a pain value by player traits before passing it to Creature::mod_pain() */
        void mod_pain( int npain ) override;
        /** Sets new intensity of pain an reacts to it */
        void set_pain( int npain ) override;
        /** Returns perceived pain (reduced with painkillers)*/
        int get_perceived_pain() const override;

        void add_pain_msg( int val, body_part bp ) const;

        /** Knocks the player to a specified tile */
        void knock_back_to( const tripoint &to ) override;

        /** Returns multiplier on fall damage at low velocity (knockback/pit/1 z-level, not 5 z-levels) */
        float fall_damage_mod() const override;
        /** Deals falling/collision damage with terrain/creature at pos */
        int impact( int force, const tripoint &pos ) override;

        /** Returns overall % of HP remaining */
        int hp_percentage() const override;

        /** Returns list of artifacts in player inventory. **/
        std::list<item *> get_artifact_items();

        /** used for drinking from hands, returns how many charges were consumed */
        int drink_from_hands( item &water );
        /** Used for eating object at pos, returns true if object is removed from inventory (last charge was consumed) */
        bool consume( item_location loc );
        /** Used for eating a particular item that doesn't need to be in inventory.
         *  Returns true if the item is to be removed (doesn't remove). */
        bool consume_item( item &target );

        /** Used for eating entered comestible, returns true if comestible is successfully eaten */
        bool eat( item &food, bool force = false );

        int get_lift_assist() const;

        bool list_ammo( const item &base, std::vector<item::reload_option> &ammo_list,
                        bool include_empty_mags = true, bool include_potential = false ) const;
        /**
         * Select suitable ammo with which to reload the item
         * @param base Item to select ammo for
         * @param prompt Force display of the menu even if only one choice
         * @param include_empty_mags Allow selection of empty magazines
         * @param include_potential Include ammo that can potentially be used, but not right now
         */
        item::reload_option select_ammo( const item &base, bool prompt = false,
                                         bool include_empty_mags = true, bool include_potential = false ) const;

        /** Select ammo from the provided options */
        item::reload_option select_ammo( const item &base, std::vector<item::reload_option> opts ) const;

        /** Check player strong enough to lift an object unaided by equipment (jacks, levers etc) */
        bool can_lift( int lift_strength_required ) const;

        /**
         * Check player capable of taking off an item.
         * @param it Thing to be taken off
         */
        ret_val<bool> can_takeoff( const item &it, const std::list<item> *res = nullptr ) const;

        /**
         * Check player capable of wielding an item.
         * @param it Thing to be wielded
         */
        ret_val<bool> can_wield( const item &it ) const;

        bool unwield();

        /**
         * Whether a tool or gun is potentially reloadable (optionally considering a specific ammo)
         * @param it Thing to be reloaded
         * @param ammo if set also check item currently compatible with this specific ammo or magazine
         * @note items currently loaded with a detachable magazine are considered reloadable
         * @note items with integral magazines are reloadable if free capacity permits (+/- ammo matches)
         */
        bool can_reload( const item &it, const itype_id &ammo = itype_id() ) const;

        /**
         * Attempt to mend an item (fix any current faults)
         * @param obj Object to mend
         * @param interactive if true prompts player when multiple faults, otherwise mends the first
         */
        void mend_item( item_location &&obj, bool interactive = true );

        /**
         * Calculate (but do not deduct) the number of moves required to reload an item with specified quantity of ammo
         * @param it Item to calculate reload cost for
         * @param ammo either ammo or magazine to use when reloading the item
         * @param qty maximum units of ammo to reload. Capped by remaining capacity and ignored if reloading using a magazine.
         */
        int item_reload_cost( const item &it, const item &ammo, int qty ) const;

        /** Wear item; returns false on fail. If interactive is false, don't alert the player or drain moves on completion. */
        cata::optional<std::list<item>::iterator>
        wear( int pos, bool interactive = true );
        cata::optional<std::list<item>::iterator>
        wear( item &to_wear, bool interactive = true );

        /** Takes off an item, returning false on fail. The taken off item is processed in the interact */
        bool takeoff( item &it, std::list<item> *res = nullptr );
        bool takeoff( int pos );

        /** So far only called by unload() from game.cpp */
        bool add_or_drop_with_msg( item &it, bool unloading = false );

        bool unload( item_location loc );

        /**
         * Try to wield a contained item consuming moves proportional to weapon skill and volume.
         * @param container Container containing the item to be wielded
         * @param internal_item reference to contained item to wield.
         * @param penalties Whether item volume and temporary effects (e.g. GRABBED, DOWNED) should be considered.
         * @param base_cost Cost due to storage type.
         */
        bool wield_contents( item &container, item *internal_item = nullptr, bool penalties = true,
                             int base_cost = INVENTORY_HANDLING_PENALTY );
        /**
         * Stores an item inside another consuming moves proportional to weapon skill and volume
         * @param container Container in which to store the item
         * @param put Item to add to the container
         * @param penalties Whether item volume and temporary effects (e.g. GRABBED, DOWNED) should be considered.
         * @param base_cost Cost due to storage type.
         */
        void store( item &container, item &put, bool penalties = true,
                    int base_cost = INVENTORY_HANDLING_PENALTY );
        /** Draws the UI and handles player input for the armor re-ordering window */
        void sort_armor();
        /** Uses a tool */
        void use( int inventory_position );
        /** Uses a tool at location */
        void use( item_location loc );
        /** Uses the current wielded weapon */
        void use_wielded();

        /** Reassign letter. */
        void reassign_item( item &it, int invlet );

        /** Removes gunmod after first unloading any contained ammo and returns true on success */
        bool gunmod_remove( item &gun, item &mod );

        /** Starts activity to install gunmod having warned user about any risk of failure or irremovable mods s*/
        void gunmod_add( item &gun, item &mod );

        /** @return Odds for success (pair.first) and gunmod damage (pair.second) */
        std::pair<int, int> gunmod_installation_odds( const item &gun, const item &mod ) const;

        /** Starts activity to install toolmod */
        void toolmod_add( item_location tool, item_location mod );

        /** Note that we've read a book at least once. **/
        virtual bool has_identified( const itype_id &item_id ) const = 0;

        /** Handles sleep attempts by the player, starts ACT_TRY_SLEEP activity */
        void try_to_sleep( const time_duration &dur = 30_minutes );
        /** Rate point's ability to serve as a bed. Takes all mutations, fatigue and stimulants into account. */
        int sleep_spot( const tripoint &p ) const;
        /** Checked each turn during "lying_down", returns true if the player falls asleep */
        bool can_sleep();

        /** Uses morale, pain and fatigue to return the player's focus target goto value */
        int calc_focus_equilibrium() const;
        /** Calculates actual focus gain/loss value from focus equilibrium*/
        int calc_focus_change() const;
        /** Uses calc_focus_change to update the player's current focus */
        void update_mental_focus();
        /** Resets stats, and applies effects in an idempotent manner */
        void reset_stats() override;

    private:
        safe_reference_anchor anchor;
        enum class power_mut_ui_cmd {
            Exit,
            Activate,
            Deactivate,
        };
        struct power_mut_ui_result {
            power_mut_ui_cmd cmd;
            trait_id mut;
        };
        power_mut_ui_result power_mutations_ui();

        /** last time we checked for sleep */
        time_point last_sleep_check = calendar::turn_zero;
        bool bio_soporific_powered_at_last_sleep_check = false;

    public:
        safe_reference<player> get_safe_reference();
        //returns true if the warning is now beyond final and results in hostility.
        bool add_faction_warning( const faction_id &id );
        int current_warnings_fac( const faction_id &id );
        bool beyond_final_warning( const faction_id &id );

        /** This handles giving xp for a skill */
        void practice( const skill_id &id, int amount, int cap = 99, bool suppress_warning = false );
        /** This handles warning the player that there current activity will not give them xp */
        void handle_skill_warning( const skill_id &id, bool force_warning = false );

        void on_worn_item_transform( const item &old_it, const item &new_it );

        /** Get the formatted name of the currently wielded item (if any) with current gun mode (if gun) */
        std::string weapname() const;

        void process_items();
        /**
         * Remove charges from a specific item (given by its item position).
         * The item must exist and it must be counted by charges.
         * @param position Item position of the item.
         * @param quantity The number of charges to remove, must not be larger than
         * the current charges of the item.
         * @return An item that contains the removed charges, it's effectively a
         * copy of the item with the proper charges.
         */
        item reduce_charges( int position, int quantity );
        /**
         * Remove charges from a specific item (given by a pointer to it).
         * Otherwise identical to @ref reduce_charges(int,int)
         * @param it A pointer to the item, it *must* exist.
         * @param quantity How many charges to remove
         */
        item reduce_charges( item *it, int quantity );

        /**
        * Check whether player has a bionic power armor interface.
        * @return true if player has an active bionic capable of powering armor, false otherwise.
        */
        bool can_interface_armor() const;

        bool has_mission_item( int mission_id ) const; // Has item with mission_id
        /**
         * Check whether the player has a gun that uses the given type of ammo.
         */
        bool has_gun_for_ammo( const ammotype &at ) const;
        bool has_magazine_for_ammo( const ammotype &at ) const;

        // Checks crafting inventory for books providing the requested recipe.
        // Then checks nearby NPCs who could provide it too.
        // Returns -1 to indicate recipe not found, otherwise difficulty to learn.
        int has_recipe( const recipe *r, const inventory &crafting_inv,
                        const std::vector<npc *> &helpers ) const;
        bool has_recipe_requirements( const recipe &rec ) const;

        bool studied_all_recipes( const itype &book ) const;

        /** Returns all recipes that are known from the books (either in inventory or nearby). */
        recipe_subset get_recipes_from_books( const inventory &crafting_inv,
                                              recipe_filter filter = nullptr ) const;
        /**
          * Returns all available recipes (from books and npc companions)
          * @param crafting_inv Current available items to craft
          * @param helpers List of NPCs that could help with crafting.
          * @param filter If set, will return only recipes that match the filter (should be much faster).
          */
        recipe_subset get_available_recipes( const inventory &crafting_inv,
                                             const std::vector<npc *> *helpers = nullptr,
                                             recipe_filter filter = nullptr ) const;

        /** For use with in progress crafts */
        int available_assistant_count( const recipe &rec ) const;
        /**
         * Time to craft not including speed multiplier
         */
        int base_time_to_craft( const recipe &rec, int batch_size = 1 ) const;
        /**
         * Expected time to craft a recipe, with assumption that multipliers stay constant.
         */
        int expected_time_to_craft( const recipe &rec, int batch_size = 1, bool in_progress = false ) const;
        std::vector<const item *> get_eligible_containers_for_crafting() const;
        bool check_eligible_containers_for_crafting( const recipe &rec, int batch_size = 1 ) const;
        bool can_make( const recipe *r, int batch_size = 1 );  // have components?
        /**
         * Returns true if the player can start crafting the recipe with the given batch size
         * The player is not required to have enough tool charges to finish crafting, only to
         * complete the first step (total / 20 + total % 20 charges)
         */
        bool can_start_craft( const recipe *rec, recipe_filter_flags, int batch_size = 1 );
        bool making_would_work( const recipe_id &id_to_make, int batch_size );

        /**
         * Start various types of crafts
         * @param loc the location of the workbench. tripoint_zero indicates crafting from inventory.
         */
        void craft( const tripoint &loc = tripoint_zero );
        void recraft( const tripoint &loc = tripoint_zero );
        void long_craft( const tripoint &loc = tripoint_zero );
        void make_craft( const recipe_id &id, int batch_size, const tripoint &loc = tripoint_zero );
        void make_all_craft( const recipe_id &id, int batch_size, const tripoint &loc = tripoint_zero );
        /** consume components and create an active, in progress craft containing them */
        item_location start_craft( craft_command &command, const tripoint &loc );
        /**
         * Calculate a value representing the success of the player at crafting the given recipe,
         * taking player skill, recipe difficulty, npc helpers, and player mutations into account.
         * @param making the recipe for which to calculate
         * @return a value >= 0.0 with >= 1.0 representing unequivocal success
         */
        double crafting_success_roll( const recipe &making ) const;
        /**
         * Check if the player meets the requirements to continue the in progress craft and if
         * unable to continue print messages explaining the reason.
         * If the craft is missing components due to messing up, prompt to consume new ones to
         * allow the craft to be continued.
         * @param craft the currently in progress craft
         * @return if the craft can be continued
         */
        bool can_continue_craft( item &craft );
        /**
         * Returns nearby NPCs ready and willing to help with crafting or some other manual task.
         * @param max If set, limits number of helpers to that value
         */
        std::vector<npc *> get_crafting_helpers( size_t max = 0 ) const;
        /**
         * Handle skill gain for player and followers during crafting
         * @param craft the currently in progress craft
         * @param multiplier what factor to multiply the base skill gain by.  This is used to apply
         * multiple steps of incremental skill gain simultaneously if needed.
         */
        void craft_skill_gain( const item &craft, const int &multiplier );

        const requirement_data *select_requirements(
            const std::vector<const requirement_data *> &, int batch, const inventory &,
            const std::function<bool( const item & )> &filter ) const;
        comp_selection<item_comp>
        select_item_component( const std::vector<item_comp> &components,
                               int batch, inventory &map_inv, bool can_cancel = false,
                               const std::function<bool( const item & )> &filter = return_true<item>, bool player_inv = true );
        std::list<item> consume_items( const comp_selection<item_comp> &is, int batch,
                                       const std::function<bool( const item & )> &filter = return_true<item> );
        std::list<item> consume_items( map &m, const comp_selection<item_comp> &is, int batch,
                                       const std::function<bool( const item & )> &filter = return_true<item>,
                                       const tripoint &origin = tripoint_zero, int radius = PICKUP_RANGE );
        std::list<item> consume_items( const std::vector<item_comp> &components, int batch = 1,
                                       const std::function<bool( const item & )> &filter = return_true<item> );
        /** Consume tools for the next multiplier * 5% progress of the craft */
        bool craft_consume_tools( item &craft, int mulitplier, bool start_craft );
        void consume_tools( const comp_selection<tool_comp> &tool, int batch );
        void consume_tools( map &m, const comp_selection<tool_comp> &tool, int batch,
                            const tripoint &origin = tripoint_zero, int radius = PICKUP_RANGE,
                            basecamp *bcp = nullptr );
        void consume_tools( const std::vector<tool_comp> &tools, int batch = 1,
                            const std::string &hotkeys = DEFAULT_HOTKEYS );

        // ---------------VALUES-----------------
        tripoint view_offset;
        // Relative direction of a grab, add to posx, posy to get the coordinates of the grabbed thing.
        tripoint grab_point;
        int volume = 0;

        bool random_start_location = false;
        start_location_id start_location;

        weak_ptr_fast<Creature> last_target;
        cata::optional<tripoint> last_target_pos;
        // Save favorite ammo location
        item_location ammo_location;
        int scent = 0;
        int cash = 0;
        int movecounter = 0;

        bool manual_examine = false;
        vproto_id starting_vehicle;
        std::vector<mtype_id> starting_pets;

        void make_craft_with_command( const recipe_id &id_to_make, int batch_size, bool is_long = false,
                                      const tripoint &loc = tripoint_zero );
        pimpl<craft_command> last_craft;

        recipe_id lastrecipe;
        int last_batch = 0;
        itype_id lastconsumed;        //used in crafting.cpp and construction.cpp

        std::set<character_id> follower_ids;
        void mod_stat( const std::string &stat, float modifier ) override;

        void set_underwater( bool );
        bool is_hallucination() const override;
        void environmental_revert_effect();

        //message related stuff
        using Character::add_msg_if_player;
        void add_msg_if_player( const std::string &msg ) const override;
        void add_msg_if_player( const game_message_params &params, const std::string &msg ) const override;
        using Character::add_msg_player_or_npc;
        void add_msg_player_or_npc( const std::string &player_msg,
                                    const std::string &npc_str ) const override;
        void add_msg_player_or_npc( const game_message_params &params, const std::string &player_msg,
                                    const std::string &npc_msg ) const override;
        using Character::add_msg_player_or_say;
        void add_msg_player_or_say( const std::string &player_msg,
                                    const std::string &npc_speech ) const override;
        void add_msg_player_or_say( const game_message_params &params, const std::string &player_msg,
                                    const std::string &npc_speech ) const override;

        /** Search surrounding squares for traps (and maybe other things in the future). */
        void search_surroundings();

        using Character::query_yn;
        bool query_yn( const std::string &mes ) const override;


        /**
         * Try to disarm the NPC. May result in fail attempt, you receiving the wepon and instantly wielding it,
         * or the weapon falling down on the floor nearby. NPC is always getting angry with you.
         * @param target Target NPC to disarm
         */
        void disarm( npc &target );

    protected:

        void store( JsonOut &json ) const;
        void load( const JsonObject &data );

        /** Processes human-specific effects of an effect. */
        void process_one_effect( effect &it, bool is_new ) override;

    private:

        /**
         * Consumes an item as medication.
         * @param target Item consumed. Must be a medication or a container of medication.
         * @return Whether the target was fully consumed.
         */
        bool consume_med( item &target );

    private:

        /** warnings from a faction about bad behavior */
        std::map<faction_id, std::pair<int, time_point>> warning_record;
};

/** Calculates the player's morale cap due to fatigue */
int calc_fatigue_cap( int fatigue );

#endif // CATA_SRC_PLAYER_H
