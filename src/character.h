#pragma once
#ifndef CATA_SRC_CHARACTER_H
#define CATA_SRC_CHARACTER_H

#include <array>
#include <bitset>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "action.h"
#include "bodypart.h"
#include "calendar.h"
#include "character_id.h"
#include "color.h"
#include "coordinates.h"
#include "creature.h"
#include "cursesdef.h"
#include "damage.h"
#include "enums.h"
#include "enum_int_operators.h"
#include "flat_set.h"
#include "game_constants.h"
#include "inventory.h"
#include "item_handling_util.h"
#include "item.h"
#include "item_location.h"
#include "memory_fast.h"
#include "optional.h"
#include "pimpl.h"
#include "player_activity.h"
#include "pldata.h"
#include "point.h"
#include "ret_val.h"
#include "stomach.h"
#include "string_formatter.h"
#include "type_id.h"
#include "units.h"
#include "visitable.h"
#include "weighted_list.h"

class JsonIn;
class JsonObject;
class JsonOut;
class SkillLevel;
class SkillLevelMap;
class bionic_collection;
class character_martial_arts;
class faction;
class ma_technique;
class known_magic;
class player;
class player_morale;
class recipe_subset;
class vehicle;
class monster;
class weather_manager;
struct bionic;
struct char_encumbrance_data;
struct construction;
struct consumption_history_t;
struct dealt_projectile_attack;
struct islot_comestible;
struct itype;
struct mutation_branch;
struct needs_rates;
struct pathfinding_settings;
struct points_left;
struct trap;
template <typename E> struct enum_traits;

enum class character_stat : char;

#define MAX_CLAIRVOYANCE 40

enum vision_modes {
    DEBUG_NIGHTVISION,
    NV_GOGGLES,
    BIRD_EYE,
    URSINE_VISION,
    BOOMERED,
    DARKNESS,
    IR_VISION,
    VISION_CLAIRVOYANCE,
    VISION_CLAIRVOYANCE_PLUS,
    VISION_CLAIRVOYANCE_SUPER,
    NUM_VISION_MODES
};

enum character_movemode : int {
    CMM_WALK = 0,
    CMM_RUN,
    CMM_CROUCH,
    CMM_COUNT
};

template<>
struct enum_traits<character_movemode> {
    static constexpr auto last = character_movemode::CMM_COUNT;
};

enum class fatigue_levels : int {
    tired = 191,
    dead_tired = 383,
    exhausted = 575,
    massive = 1000
};

DEFINE_INTEGER_OPERATORS( fatigue_levels )

const std::unordered_map<std::string, fatigue_levels> fatigue_level_strs = { {
        { "TIRED", fatigue_levels::tired },
        { "DEAD_TIRED", fatigue_levels::dead_tired },
        { "EXHAUSTED", fatigue_levels::exhausted },
        { "MASSIVE_FATIGUE", fatigue_levels::massive }
    }
};

// Sleep deprivation is defined in minutes, and although most calculations scale linearly,
// maluses are bestowed only upon reaching the tiers defined below.
enum class sleep_deprivation_levels : int {
    harmless = 2 * 24 * 60,
    minor = 3 * 24 * 60,
    serious = 4 * 24 * 60,
    major = 5 * 24 * 60,
    massive = 6 * 24 * 60
};

DEFINE_INTEGER_OPERATORS( sleep_deprivation_levels )

enum class thirst_levels : int {
    turgid = INT_MIN,
    hydrated = 0,
    slaked = 60,
    thirsty = 120,
    very_thirsty = 240,
    dehydrated = 480,
    parched = 600,
    dead = 1200
};

DEFINE_INTEGER_OPERATORS( thirst_levels )

// This tries to represent both rating and
// character's decision to respect said rating
enum class edible_rating {
    // Edible or we pretend it is
    edible,
    // Not food at all
    inedible,
    // Not food because mutated mouth/system
    inedible_mutation,
    // You can eat it, but it will hurt morale
    allergy,
    // Smaller allergy penalty
    allergy_weak,
    // Cannibalism (unless psycho/cannibal)
    cannibalism,
    // Rotten or not rotten enough (for saprophages)
    rotten,
    // Can provoke vomiting if you already feel nauseous.
    nausea,
    // We did overeat, cramming more will surely cause vomiting.
    bloated,
    // We can eat this, but we'll overeat
    too_full,
    // Some weird stuff that requires a tool we don't have
    no_tool
};

enum class rechargeable_cbm {
    none = 0,
    reactor,
    furnace,
    other
};

struct aim_type {
    std::string name;
    std::string action;
    std::string help;
    bool has_threshold;
    int threshold;
};

struct special_attack {
    std::string text;
    damage_instance damage;
};

class Character : public Creature, public visitable<Character>
{
    public:
        Character( const Character & ) = delete;
        auto operator=( const Character & ) -> Character & = delete;
        ~Character() override;

        auto as_character() -> Character * override {
            return this;
        }
        auto as_character() const -> const Character * override {
            return this;
        }

        auto getID() const -> character_id;
        // sets the ID, will *only* succeed when the current id is not valid
        // allows forcing a -1 id which is required for templates to not throw errors
        void setID( character_id i, bool force = false );

        auto bloodType() const -> field_type_id override;
        auto gibType() const -> field_type_id override;
        auto is_warm() const -> bool override;
        auto in_species( const species_id &spec ) const -> bool override;
        // Turned to false for simulating NPCs on distant missions so they don't drop all their gear in sight
        bool death_drops = true;
        // Is currently in control of a vehicle
        bool controlling_vehicle = false;
        auto symbol() const -> const std::string & override;

        enum class comfort_level {
            impossible = -999,
            uncomfortable = -7,
            neutral = 0,
            slightly_comfortable = 3,
            comfortable = 5,
            very_comfortable = 10
        };

        // Character stats
        // TODO: Make those protected
        int str_max = 0;
        int dex_max = 0;
        int int_max = 0;
        int per_max = 0;

        int str_cur = 0;
        int dex_cur = 0;
        int int_cur = 0;
        int per_cur = 0;

        // The prevalence of getter, setter, and mutator functions here is partially
        // a result of the slow, piece-wise migration of the player class upwards into
        // the character class. As enough logic is moved upwards to fully separate
        // utility upwards out of the player class, as many of these as possible should
        // be eliminated to allow for proper code separation. (Note: Not "all", many").
        /** Getters for stats exclusive to characters */
        virtual auto get_str() const -> int;
        virtual auto get_dex() const -> int;
        virtual auto get_per() const -> int;
        virtual auto get_int() const -> int;

        virtual auto get_str_base() const -> int;
        virtual auto get_dex_base() const -> int;
        virtual auto get_per_base() const -> int;
        virtual auto get_int_base() const -> int;

        virtual auto get_str_bonus() const -> int;
        virtual auto get_dex_bonus() const -> int;
        virtual auto get_per_bonus() const -> int;
        virtual auto get_int_bonus() const -> int;

        auto get_speed() const -> int override;

        // Penalty modifiers applied for ranged attacks due to low stats
        virtual auto ranged_dex_mod() const -> int;
        virtual auto ranged_per_mod() const -> int;

        /** Setters for stats exclusive to characters */
        virtual void set_str_bonus( int nstr );
        virtual void set_dex_bonus( int ndex );
        virtual void set_per_bonus( int nper );
        virtual void set_int_bonus( int nint );
        virtual void mod_str_bonus( int nstr );
        virtual void mod_dex_bonus( int ndex );
        virtual void mod_per_bonus( int nper );
        virtual void mod_int_bonus( int nint );

        // Prints message(s) about current health
        void print_health() const;

        /** Getters for health values exclusive to characters */
        virtual auto get_healthy() const -> int;
        virtual auto get_healthy_mod() const -> int;

        /** Modifiers for health values exclusive to characters */
        virtual void mod_healthy( int nhealthy );
        virtual void mod_healthy_mod( int nhealthy_mod, int cap );

        /** Setters for health values exclusive to characters */
        virtual void set_healthy( int nhealthy );
        virtual void set_healthy_mod( int nhealthy_mod );

        /** Getter for need values exclusive to characters */
        auto get_stored_kcal() const -> int;
        // Maximum stored calories, excluding stomach.
        // If more would be digested, it is instead wasted.
        auto max_stored_kcal() const -> int;
        auto get_kcal_percent() const -> float;
        auto get_thirst() const -> int;
        auto get_thirst_description() const -> std::pair<std::string, nc_color>;
        auto get_hunger_description() const -> std::pair<std::string, nc_color>;
        auto get_fatigue_description() const -> std::pair<std::string, nc_color>;
        auto get_fatigue() const -> int;
        auto get_sleep_deprivation() const -> int;

        auto get_pain_description() const -> std::pair<std::string, nc_color> override;

        /** Modifiers for need values exclusive to characters */
        virtual void mod_stored_kcal( int nkcal );
        virtual void mod_stored_nutr( int nnutr );
        virtual void mod_thirst( int nthirst );
        virtual void mod_fatigue( int nfatigue );
        virtual void mod_sleep_deprivation( int nsleep_deprivation );

        /** Setters for need values exclusive to characters */
        virtual void set_stored_kcal( int kcal );
        virtual void set_thirst( int nthirst );
        virtual void set_fatigue( int nfatigue );
        virtual void set_sleep_deprivation( int nsleep_deprivation );

        void mod_stat( const std::string &stat, float modifier ) override;

        /** Get size class of character **/
        auto get_size() const -> m_size override;
        /** Recalculate size class of character **/
        void recalculate_size();

        /** Returns either "you" or the player's name. capitalize_first assumes
            that the character's name is already upper case and uses it only for
            possessive "your" and "you"
        **/
        auto disp_name( bool possessive = false, bool capitalize_first = false ) const -> std::string override;
        /** Returns the name of the player's outer layer, e.g. "armor plates" */
        auto skin_name() const -> std::string override;

        /* returns the character's faction */
        virtual auto get_faction() const -> faction * {
            return nullptr;
        }
        void set_fac_id( const std::string &my_fac_id );

        /* Adjusts provided sight dispersion to account for player stats */
        auto effective_dispersion( int dispersion ) const -> int;

        /* Accessors for aspects of aim speed. */
        auto get_aim_types( const item &gun ) const -> std::vector<aim_type>;
        auto get_fastest_sight( const item &gun, double recoil ) const -> std::pair<int, int>;
        auto get_most_accurate_sight( const item &gun ) const -> int;
        auto aim_speed_skill_modifier( const skill_id &gun_skill ) const -> double;
        auto aim_speed_dex_modifier() const -> double;
        auto aim_speed_encumbrance_modifier() const -> double;
        auto aim_cap_from_volume( const item &gun ) const -> double;

        /* Calculate aim improvement per move spent aiming at a given @ref recoil */
        auto aim_per_move( const item &gun, double recoil ) const -> double;

        /** Get maximum recoil penalty due to vehicle motion */
        auto recoil_vehicle() const -> double;

        auto recoil_mode() const -> double;

        /** Current total maximum recoil penalty from all sources */
        auto recoil_total() const -> double;

        /** Combat getters */
        auto get_dodge_base() const -> float override;
        auto get_hit_base() const -> float override;
        auto get_dodge() const -> float override;

        auto pos() const -> const tripoint & override;
        /** Returns the player's sight range */
        auto sight_range( int light_level ) const -> int override;
        /** Returns the player maximum vision range factoring in mutations, diseases, and other effects */
        auto  unimpaired_range() const -> int;
        /** Returns true if overmap tile is within player line-of-sight */
        auto overmap_los( const tripoint_abs_omt &omt, int sight_points ) -> bool;
        /** Returns the distance the player can see on the overmap */
        auto  overmap_sight_range( int light_level ) const -> int;
        /** Returns the distance the player can see through walls */
        auto  clairvoyance() const -> int;
        /** Returns true if the player has some form of impaired sight */
        auto sight_impaired() const -> bool;
        /** Returns true if the player or their vehicle has an alarm clock */
        auto has_alarm_clock() const -> bool;
        /** Returns true if the player or their vehicle has a watch */
        auto has_watch() const -> bool;
        /** Called after every action, invalidates player caches */
        void action_taken();
        /** Returns true if the player is knocked over or has broken legs */
        auto is_on_ground() const -> bool override;
        /** Returns the player's speed for swimming across water tiles */
        auto  swim_speed() const -> int;
        /**
         * Adds a reason for why the player would miss a melee attack.
         *
         * To possibly be messaged to the player when he misses a melee attack.
         * @param reason A message for the player that gives a reason for him missing.
         * @param weight The weight used when choosing what reason to pick when the
         * player misses.
         */
        void add_miss_reason( const std::string &reason, unsigned int weight );
        /** Clears the list of reasons for why the player would miss a melee attack. */
        void clear_miss_reasons();
        /**
         * Returns an explanation for why the player would miss a melee attack.
         */
        auto get_miss_reason() -> std::string;

        /**
          * Handles passive regeneration of pain and maybe hp.
          */
        void regen( int rate_multiplier );
        // called once per 24 hours to enforce the minimum of 1 hp healed per day
        // TODO: Move to Character once heal() is moved
        void enforce_minimum_healing();
        /** get best quality item that this character has */
        auto best_quality_item( const quality_id &qual ) -> item *;
        /** Handles health fluctuations over time */
        virtual void update_health( int external_modifiers = 0 );
        /** Updates all "biology" by one turn. Should be called once every turn. */
        void update_body();
        /** Updates all "biology" as if time between `from` and `to` passed. */
        void update_body( const time_point &from, const time_point &to );
        /** Updates the stomach to give accurate hunger messages */
        void update_stomach( const time_point &from, const time_point &to );
        /** Increases hunger, thirst, fatigue and stimulants wearing off. `rate_multiplier` is for retroactive updates. */
        void update_needs( int rate_multiplier );
        auto calc_needs_rates() const -> needs_rates;
        /** Kills the player if too hungry, stimmed up etc., forces tired player to sleep and prints warnings. */
        void check_needs_extremes();
        /** Returns if the player has hibernation mutation and is asleep and well fed */
        auto is_hibernating() const -> bool;
        /** Maintains body temperature */
        void update_bodytemp( const map &m, const weather_manager &weather );

        /** Equalizes heat between body parts */
        void temp_equalizer( const bodypart_id &bp1, const bodypart_id &bp2 );

        struct comfort_response_t {
            comfort_level level = comfort_level::neutral;
            const item *aid = nullptr;
        };
        /** Rate point's ability to serve as a bed. Only takes certain mutations into account, and not fatigue nor stimulants. */
        auto base_comfort_value( const tripoint &p ) const -> comfort_response_t;

        /** Define blood loss (in percents) */
        auto blood_loss( const bodypart_id &bp ) const -> int;

        /** Resets the value of all bonus fields to 0. */
        void reset_bonuses() override;
        /** Resets stats, and applies effects in an idempotent manner */
        void reset_stats() override;
        /** Handles stat and bonus reset. */
        void reset() override;

        /** Recalculates encumbrance cache. */
        void reset_encumbrance();
        /** Returns ENC provided by armor, etc. */
        auto encumb( body_part bp ) const -> int;

        /** Returns body weight plus weight of inventory and worn/wielded items */
        auto get_weight() const -> units::mass override;
        /** Get encumbrance for all body parts. */
        auto get_encumbrance() const -> char_encumbrance_data;
        /** Get encumbrance for all body parts as if `new_item` was also worn. */
        auto get_encumbrance( const item &new_item ) const -> char_encumbrance_data;
        /** Get encumbrance penalty per layer & body part */
        auto extraEncumbrance( layer_level level, int bp ) const -> int;

        /** Returns true if the character is wearing power armor */
        auto is_wearing_power_armor( bool *hasHelmet = nullptr ) const -> bool;
        /** Returns true if the character is wearing active power */
        auto is_wearing_active_power_armor() const -> bool;
        /** Returns true if the player is wearing an active optical cloak */
        auto is_wearing_active_optcloak() const -> bool;

        /** Returns true if the player is in a climate controlled area or armor */
        auto in_climate_control() -> bool;

        /** Returns true if the player isn't able to see */
        auto is_blind() const -> bool;

        auto is_invisible() const -> bool;
        /** Checks is_invisible() as well as other factors */
        auto visibility( bool check_color = false, int stillness = 0 ) const -> int;

        /** Returns character luminosity based on the brightest active item they are carrying */
        auto active_light() const -> float;

        auto sees_with_specials( const Creature &critter ) const -> bool;

        /** Bitset of all the body parts covered only with items with `flag` (or nothing) */
        auto exclusive_flag_coverage( const std::string &flag ) const -> body_part_set;

        /** Processes effects which may prevent the Character from moving (bear traps, crushed, etc.).
         *  Returns false if movement is stopped. */
        auto move_effects( bool attacking ) -> bool override;

        void wait_effects();

        /** Check against the character's current movement mode */
        auto movement_mode_is( character_movemode mode ) const -> bool;
        auto get_movement_mode() const -> character_movemode;

        virtual void set_movement_mode( character_movemode mode ) = 0;

        /**Determine if character is susceptible to dis_type and if so apply the symptoms*/
        void expose_to_disease( diseasetype_id dis_type );
        /**
         * Handles end-of-turn processing.
         */
        void process_turn() override;

        /** Recalculates HP after a change to max strength */
        void recalc_hp();
        /** Sets hp for all body parts */
        void calc_all_parts_hp( float hp_mod = 0.0, float hp_adjust = 0.0, int str_max = 0 );
        /** Modifies the player's sight values
         *  Must be called when any of the following change:
         *  This must be called when any of the following change:
         * - effects
         * - bionics
         * - traits
         * - underwater
         * - clothes
         */
        void recalc_sight_limits();
        /**
         * Returns the apparent light level at which the player can see.
         * This is adjusted by the light level at the *character's* position
         * to simulate glare, etc, night vision only works if you are in the dark.
         */
        auto get_vision_threshold( float light_level ) const -> float;
        /**
         * Flag encumbrance for updating.
        */
        void flag_encumbrance();
        /**
         * Checks worn items for the "RESET_ENCUMBRANCE" flag, which indicates
         * that encumbrance may have changed and require recalculating.
         */
        void check_item_encumbrance_flag();

        /** Returns true if the character is wearing something on the entered body_part, ignoring items with the ALLOWS_NATURAL_ATTACKS flag */
        auto natural_attack_restricted_on( const bodypart_id &bp ) const -> bool;

        int blocks_left = 0;
        int dodges_left = 0;

        double recoil = MAX_RECOIL;

        profession_id prof;
        std::string custom_profession;

        /** Returns true if the player is able to use a miss recovery technique */
        auto can_miss_recovery( const item &weap ) const -> bool;
        /** Returns true if the player has quiet melee attacks */
        auto is_quiet() const -> bool;

        // melee.cpp
        /** Checks for valid block abilities and reduces damage accordingly. Returns true if the player blocks */
        auto block_hit( Creature *source, bodypart_id &bp_hit, damage_instance &dam ) -> bool override;
        /** Returns the best item for blocking with */
        auto best_shield() -> item &;
        /** Calculates melee weapon wear-and-tear through use, returns true if item is destroyed. */
        auto handle_melee_wear( item &shield, float wear_multiplier = 1.0f ) -> bool;
        /** Returns a random valid technique */
        auto pick_technique( Creature &t, const item &weap,
                                 bool crit, bool dodge_counter, bool block_counter ) -> matec_id;
        void perform_technique( const ma_technique &technique, Creature &t, damage_instance &di,
                                int &move_cost );
        /**
         * Sets up a melee attack and handles melee attack function calls
         * @param t Creature to attack
         * @param allow_special whether non-forced martial art technique or mutation attack should be
         *   possible with this attack.
         * @param force_technique special technique to use in attack.
         * @param allow_unarmed always uses the wielded weapon regardless of martialarts style
         */
        void melee_attack( Creature &t, bool allow_special, const matec_id &force_technique,
                           bool allow_unarmed = true );
        /**
         * Calls the to other melee_attack function with an empty technique id (meaning no specific
         * technique should be used).
         */
        void melee_attack( Creature &t, bool allow_special );
        /** Handles combat effects, returns a string of any valid combat effect messages */
        auto melee_special_effects( Creature &t, damage_instance &d, item &weap ) -> std::string;
        /** Performs special attacks and their effects (poisonous, stinger, etc.) */
        void perform_special_attacks( Creature &t, dealt_damage_instance &dealt_dam );
        bool reach_attacking = false;

        /** Returns a vector of valid mutation attacks */
        auto mutation_attacks( Creature &t ) const -> std::vector<special_attack>;
        /** Returns the bonus bashing damage the player deals based on their stats */
        auto bonus_damage( bool random ) const -> float;
        /** Returns weapon skill */
        auto get_melee_hit_base() const -> float;
        /** Returns the player's basic hit roll that is compared to the target's dodge roll */
        auto hit_roll() const -> float override;
        /** Returns the chance to critical given a hit roll and target's dodge roll */
        auto crit_chance( float roll_hit, float target_dodge, const item &weap ) const -> double;
        /** Returns true if the player scores a critical hit */
        auto scored_crit( float target_dodge, const item &weap ) const -> bool;
        /** Returns cost (in moves) of attacking with given item (no modifiers, like stuck) */
        auto attack_cost( const item &weap ) const -> int;
        /** Gets melee accuracy component from weapon+skills */
        auto get_hit_weapon( const item &weap ) const -> float;

        // If average == true, adds expected values of random rolls instead of rolling.
        /** Adds all 3 types of physical damage to instance */
        void roll_all_damage( bool crit, damage_instance &di, bool average, const item &weap ) const;
        /** Adds player's total bash damage to the damage instance */
        void roll_bash_damage( bool crit, damage_instance &di, bool average, const item &weap ) const;
        /** Adds player's total cut damage to the damage instance */
        void roll_cut_damage( bool crit, damage_instance &di, bool average, const item &weap ) const;
        /** Adds player's total stab damage to the damage instance */
        void roll_stab_damage( bool crit, damage_instance &di, bool average, const item &weap ) const;

    private:
        /** Check if an area-of-effect technique has valid targets */
        auto valid_aoe_technique( Creature &t, const ma_technique &technique ) -> bool;
        auto valid_aoe_technique( Creature &t, const ma_technique &technique,
                                  std::vector<Creature *> &targets ) -> bool;
    public:

        // any side effects that might happen when the Character is hit
        void on_hit( Creature *source, bodypart_id /*bp_hit*/,
                     float /*difficulty*/, dealt_projectile_attack const * /*proj*/ ) override;
        // any side effects that might happen when the Character hits a Creature
        void did_hit( Creature &target );

        /** Actually hurt the player, hurts a body_part directly, no armor reduction */
        void apply_damage( Creature *source, bodypart_id hurt, int dam,
                           bool bypass_med = false ) override;
        /** Calls Creature::deal_damage and handles damaged effects (waking up, etc.) */
        auto deal_damage( Creature *source, bodypart_id bp,
                                           const damage_instance &d ) -> dealt_damage_instance override;
        /** Reduce healing effect intensity, return initial intensity of the effect */
        auto reduce_healing_effect( const efftype_id &eff_id, int remove_med, const bodypart_id &hurt ) -> int;

        void cough( bool harmful = false, int loudness = 4 );
        /**
         * Check for relevant passive, non-clothing that can absorb damage, and reduce by specified
         * damage unit.  Only flat bonuses are checked here.  Multiplicative ones are checked in
         * @ref player::absorb_hit.  The damage amount will never be reduced to less than 0.
         * This is called from @ref player::absorb_hit
         */
        void passive_absorb_hit( const bodypart_id &bp, damage_unit &du ) const;
        /** Runs through all bionics and armor on a part and reduces damage through their armor_absorb */
        void absorb_hit( const bodypart_id &bp, damage_instance &dam ) override;
        /**
         * Reduces and mutates du, prints messages about armor taking damage.
         * @return true if the armor was completely destroyed (and the item must be deleted).
         */
        auto armor_absorb( damage_unit &du, item &armor ) -> bool;
        /**
         * Check for passive bionics that provide armor, and returns the armor bonus
         * This is called from player::passive_absorb_hit
         */
        auto bionic_armor_bonus( const bodypart_id &bp, damage_type dt ) const -> float;
        /** Returns the armor bonus against given type from martial arts buffs */
        auto mabuff_armor_bonus( damage_type type ) const -> int;
        /** Returns overall fire resistance */
        auto get_armor_fire( const std::map<bodypart_id, std::vector<const item *>>
                &clothing_map ) const -> std::map<bodypart_id, int>;
        // --------------- Mutation Stuff ---------------

        // In mutation.cpp
        /** Returns true if the player has the entered trait */
        auto has_trait( const trait_id &b ) const -> bool override;
        /** Returns true if the player has the entered starting trait */
        auto has_base_trait( const trait_id &b ) const -> bool;
        /** Returns true if player has a trait with a flag */
        auto has_trait_flag( const std::string &b ) const -> bool;
        /** Returns true if character has a trait which cancels the entered trait. */
        auto has_opposite_trait( const trait_id &flag ) const -> bool;
        /** Returns the trait id with the given invlet, or an empty string if no trait has that invlet */
        auto trait_by_invlet( int ch ) const -> trait_id;

        /** Toggles a trait on the player and in their mutation list */
        void toggle_trait( const trait_id & );
        /** Add or removes a mutation on the player, but does not trigger mutation loss/gain effects. */
        void set_mutation( const trait_id & );
        void unset_mutation( const trait_id & );
        /**Unset switched mutation and set target mutation instead*/
        void switch_mutations( const trait_id &switched, const trait_id &target, bool start_powered );

        // Trigger and disable mutations that can be so toggled.
        void activate_mutation( const trait_id &mutation );
        void deactivate_mutation( const trait_id &mut );

        /** Removes the appropriate costs (NOTE: will reapply mods & recalc sightlines in case of newly activated mutation). */
        void mutation_spend_resources( const trait_id &mut );

        /** Converts a body_part to an hp_part */
        static auto bp_to_hp( body_part bp ) -> hp_part;
        /** Converts an hp_part to a body_part */
        static auto hp_to_bp( hp_part hpart ) -> body_part;

        auto can_mount( const monster &critter ) const -> bool;
        void mount_creature( monster &z );
        auto is_mounted() const -> bool;
        auto check_mount_will_move( const tripoint &dest_loc ) -> bool;
        auto check_mount_is_spooked() -> bool;
        void dismount();
        void forced_dismount();

        auto is_deaf() const -> bool;
        /** Returns true if the player has two functioning arms */
        auto has_two_arms() const -> bool;
        /** Returns the number of functioning arms */
        auto get_working_arm_count() const -> int;
        /** Returns the number of functioning legs */
        auto get_working_leg_count() const -> int;
        /** Returns true if the limb is disabled(12.5% or less hp)*/
        auto is_limb_disabled( const bodypart_id &limb ) const -> bool;
        /** Returns true if the limb is hindered(40% or less hp) */
        auto is_limb_hindered( hp_part limb ) const -> bool;
        /** Returns true if the limb is broken */
        auto is_limb_broken( const bodypart_id &limb ) const -> bool;
        /** source of truth of whether a Character can run */
        auto can_run() -> bool;
        /** Hurts all body parts for dam, no armor reduction */
        void hurtall( int dam, Creature *source, bool disturb = true );
        /** Harms all body parts for dam, with armor reduction. If vary > 0 damage to parts are random within vary % (1-100) */
        auto hitall( int dam, int vary, Creature *source ) -> int;
        /** Handles effects that happen when the player is damaged and aware of the fact. */
        void on_hurt( Creature *source, bool disturb = true );
        /** Heals a body_part for dam */
        void heal( const bodypart_id &healed, int dam );
        /** Heals all body parts for dam */
        void healall( int dam );
        /**
         * Displays menu with body part hp, optionally with hp estimation after healing.
         * Returns selected part.
         * menu_header - name of item that triggers this menu
         * show_all - show and enable choice of all limbs, not only healable
         * precise - show numerical hp
         * normal_bonus - heal normal limb
         * head_bonus - heal head
         * torso_bonus - heal torso
         * bleed - chance to stop bleeding
         * bite - chance to remove bite
         * infect - chance to remove infection
         * bandage_power - quality of bandage
         * disinfectant_power - quality of disinfectant
         */
        auto body_window( const std::string &menu_header,
                             bool show_all, bool precise,
                             int normal_bonus, int head_bonus, int torso_bonus,
                             float bleed, float bite, float infect, float bandage_power, float disinfectant_power ) const -> hp_part;

        // Returns color which this limb would have in healing menus
        auto limb_color( const bodypart_id &bp, bool bleed, bool bite, bool infect ) const -> nc_color;

        static const std::vector<material_id> fleshy;
        auto made_of( const material_id &m ) const -> bool override;
        auto made_of_any( const std::set<material_id> &ms ) const -> bool override;

        // Drench cache
        enum water_tolerance {
            WT_IGNORED = 0,
            WT_NEUTRAL,
            WT_GOOD,
            NUM_WATER_TOLERANCE
        };
        inline auto posx() const -> int override {
            return position.x;
        }
        inline auto posy() const -> int override {
            return position.y;
        }
        inline auto posz() const -> int override {
            return position.z;
        }
        inline void setx( int x ) {
            setpos( tripoint( x, position.y, position.z ) );
        }
        inline void sety( int y ) {
            setpos( tripoint( position.x, y, position.z ) );
        }
        inline void setz( int z ) {
            setpos( tripoint( position.xy(), z ) );
        }
        inline void setpos( const tripoint &p ) override {
            position = p;
        }

        /**
         * Global position, expressed in map square coordinate system
         * (the most detailed coordinate system), used by the @ref map.
         */
        virtual auto global_square_location() const -> tripoint;
        /**
        * Returns the location of the player in global submap coordinates.
        */
        auto global_sm_location() const -> tripoint;
        /**
        * Returns the location of the player in global overmap terrain coordinates.
        */
        auto global_omt_location() const -> tripoint_abs_omt;

    private:
        /** Retrieves a stat mod of a mutation. */
        auto get_mod( const trait_id &mut, const std::string &arg ) const -> int;
        /** Applies skill-based boosts to stats **/
        void apply_skill_boost();
    protected:
        void do_skill_rust();
        /** Applies stat mods to character. */
        void apply_mods( const trait_id &mut, bool add_remove );

        /** Recalculate encumbrance for all body parts. */
        auto calc_encumbrance() const -> char_encumbrance_data;
        /** Recalculate encumbrance for all body parts as if `new_item` was also worn. */
        auto calc_encumbrance( const item &new_item ) const -> char_encumbrance_data;

        /** Applies encumbrance from mutations and bionics only */
        void mut_cbm_encumb( char_encumbrance_data &vals ) const;

        /** Return the position in the worn list where new_item would be
         * put by default */
        auto position_to_wear_new_item( const item &new_item ) -> std::list<item>::iterator;

        /** Applies encumbrance from items only
         * If new_item is not null, then calculate under the asumption that it
         * is added to existing work items. */
        void item_encumb( char_encumbrance_data &vals, const item &new_item ) const;

        std::array<std::array<int, NUM_WATER_TOLERANCE>, num_bp> mut_drench;

    public:
        // recalculates enchantment cache by iterating through all held, worn, and wielded items
        void recalculate_enchantment_cache();
        void rebuild_mutation_cache();

        /**
         * Calculate bonus from enchantments for given base value.
         */
        auto bonus_from_enchantments( double base, enchant_vals::mod value, bool round = false ) const -> double;

        /** Returns true if the player has any martial arts buffs attached */
        auto has_mabuff( const mabuff_id &buff_id ) const -> bool;
        /** Returns true if the player has a grab breaking technique available */
        auto has_grab_break_tec() const -> bool override;

        /** Returns the to hit bonus from martial arts buffs */
        auto mabuff_tohit_bonus() const -> float;
        /** Returns the dodge bonus from martial arts buffs */
        auto mabuff_dodge_bonus() const -> float;
        /** Returns the block bonus from martial arts buffs */
        auto mabuff_block_bonus() const -> int;
        /** Returns the speed bonus from martial arts buffs */
        auto mabuff_speed_bonus() const -> int;
        /** Returns the arpen bonus from martial arts buffs*/
        auto mabuff_arpen_bonus( damage_type type ) const -> int;
        /** Returns the damage multiplier to given type from martial arts buffs */
        auto mabuff_damage_mult( damage_type type ) const -> float;
        /** Returns the flat damage bonus to given type from martial arts buffs, applied after the multiplier */
        auto mabuff_damage_bonus( damage_type type ) const -> int;
        /** Returns the flat penalty to move cost of attacks. If negative, that's a bonus. Applied after multiplier. */
        auto mabuff_attack_cost_penalty() const -> int;
        /** Returns the multiplier on move cost of attacks. */
        auto mabuff_attack_cost_mult() const -> float;

        /** Handles things like removal of armor, etc. */
        void mutation_effect( const trait_id &mut );
        /** Handles what happens when you lose a mutation. */
        void mutation_loss_effect( const trait_id &mut );

        auto has_active_mutation( const trait_id &b ) const -> bool;
    private:
        // The old mutation algorithm
        void old_mutate();
    public:
        /** Picks a random valid mutation and gives it to the Character, possibly removing/changing others along the way */
        void mutate();
        /** Returns true if the player doesn't have the mutation or a conflicting one and it complies with the force typing */
        auto mutation_ok( const trait_id &mutation, bool force_good, bool force_bad ) const -> bool;
        /** Picks a random valid mutation in a category and mutate_towards() it */
        void mutate_category( const std::string &mut_cat );
        /** Mutates toward one of the given mutations, upgrading or removing conflicts if necessary */
        auto mutate_towards( std::vector<trait_id> muts, int num_tries = INT_MAX ) -> bool;
        /** Mutates toward the entered mutation, upgrading or removing conflicts if necessary */
        auto mutate_towards( const trait_id &mut ) -> bool;
        /** Removes a mutation, downgrading to the previous level if possible */
        void remove_mutation( const trait_id &mut, bool silent = false );
        /** Calculate percentage chances for mutations */
        auto mutation_chances() const -> std::map<trait_id, float>;
        /** Returns true if the player has the entered mutation child flag */
        auto has_child_flag( const trait_id &flag ) const -> bool;
        /** Removes the mutation's child flag from the player's list */
        void remove_child_flag( const trait_id &flag );
        /** Recalculates mutation_category_level[] values for the player */
        void set_highest_cat_level();
        /** Returns the highest mutation category */
        auto get_highest_category() const -> std::string;
        /** Recalculates mutation drench protection for all bodyparts (ignored/good/neutral stats) */
        void drench_mut_calc();
        /** Recursively traverses the mutation's prerequisites and replacements, building up a map */
        void build_mut_dependency_map( const trait_id &mut,
                                       std::unordered_map<trait_id, int> &dependency_map, int distance );

        /**
        * Returns true if this category of mutation is allowed.
        */
        auto is_category_allowed( const std::vector<std::string> &category ) const -> bool;
        auto is_category_allowed( const std::string &category ) const -> bool;

        auto is_weak_to_water() const -> bool;

        /**Check for mutation disallowing the use of an healing item*/
        auto can_use_heal_item( const item &med ) const -> bool;

        auto can_install_cbm_on_bp( const std::vector<bodypart_id> &bps ) const -> bool;

        /**
         * Returns resistances on a body part provided by mutations
         */
        // TODO: Cache this, it's kinda expensive to compute
        auto mutation_armor( bodypart_id bp ) const -> resistances;
        auto mutation_armor( bodypart_id bp, damage_type dt ) const -> float;
        auto mutation_armor( bodypart_id bp, const damage_unit &du ) const -> float;

        // --------------- Bionic Stuff ---------------
        /** Handles bionic activation effects of the entered bionic, returns if anything activated */
        auto activate_bionic( int b, bool eff_only = false ) -> bool;
        auto get_bionics() const -> std::vector<bionic_id>;
        /** Returns amount of Storage CBMs in the corpse **/
        auto amount_of_storage_bionics() const -> std::pair<int, int>;
        /** Returns true if the player has the entered bionic id */
        auto has_bionic( const bionic_id &b ) const -> bool;
        /** Returns true if the player has the entered bionic id and it is powered on */
        auto has_active_bionic( const bionic_id &b ) const -> bool;
        /**Returns true if the player has any bionic*/
        auto has_any_bionic() const -> bool;
        /**Returns true if the character can fuel a bionic with the item*/
        auto can_fuel_bionic_with( const item &it ) const -> bool;
        /**Return bionic_id of bionics able to use it as fuel*/
        auto get_bionic_fueled_with( const item &it ) const -> std::vector<bionic_id>;
        /**Return bionic_id of fueled bionics*/
        auto get_fueled_bionics() const -> std::vector<bionic_id>;
        /**Returns bionic_id of first remote fueled bionic found*/
        auto get_remote_fueled_bionic() const -> bionic_id;
        /**Return bionic_id of bionic of most fuel efficient bionic*/
        auto get_most_efficient_bionic( const std::vector<bionic_id> &bids ) const -> bionic_id;
        /**Return list of available fuel for this bionic*/
        auto get_fuel_available( const bionic_id &bio ) const -> std::vector<itype_id>;
        /**Return available space to store specified fuel*/
        auto get_fuel_capacity( const itype_id &fuel ) const -> int;
        /**Return total space to store specified fuel*/
        auto get_total_fuel_capacity( const itype_id &fuel ) const -> int;
        /**Updates which bionic contain fuel and which is empty*/
        void update_fuel_storage( const itype_id &fuel );
        /**Get stat bonus from bionic*/
        auto get_mod_stat_from_bionic( const character_stat &Stat ) const -> int;
        // route for overmap-scale traveling
        std::vector<tripoint_abs_omt> omt_path;

        /** Handles bionic effects over time of the entered bionic */
        void process_bionic( int b );
        /** Handles bionic deactivation effects of the entered bionic, returns if anything
         *  deactivated */
        auto deactivate_bionic( int b, bool eff_only = false ) -> bool;
        /** Returns the size of my_bionics[] */
        auto num_bionics() const -> int;
        /** Returns the bionic at a given index in my_bionics[] */
        auto bionic_at_index( int i ) -> bionic &;
        /** Remove all bionics */
        void clear_bionics();
        auto get_used_bionics_slots( const bodypart_id &bp ) const -> int;
        auto get_total_bionics_slots( const bodypart_id &bp ) const -> int;
        auto get_free_bionics_slots( const bodypart_id &bp ) const -> int;

        /**Has enough anesthetic for surgery*/
        auto has_enough_anesth( const itype *cbm, player &patient ) -> bool;
        /** Handles process of introducing patient into anesthesia during Autodoc operations. Requires anesthesia kits or NOPAIN mutation */
        void introduce_into_anesthesia( const time_duration &duration, player &installer,
                                        bool needs_anesthesia );
        /** Removes a bionic from my_bionics[] */
        void remove_bionic( const bionic_id &b );
        /** Adds a bionic to my_bionics[] */
        void add_bionic( const bionic_id &b );
        /**Calculate skill bonus from tiles in radius*/
        auto env_surgery_bonus( int radius ) -> float;
        /** Calculate skill for (un)installing bionics */
        auto bionics_adjusted_skill( const skill_id &most_important_skill,
                                      const skill_id &important_skill,
                                      const skill_id &least_important_skill,
                                      int skill_level = -1 ) -> float;
        /** Calculate non adjusted skill for (un)installing bionics */
        auto bionics_pl_skill( const skill_id &most_important_skill,
                              const skill_id &important_skill,
                              const skill_id &least_important_skill,
                              int skill_level = -1 ) -> int;
        /**Is the installation possible*/
        auto can_install_bionics( const itype &type, player &installer, bool autodoc = false,
                                  int skill_level = -1 ) -> bool;
        auto bionic_installation_issues( const bionic_id &bioid ) const -> std::map<bodypart_id, int>;
        /** Initialize all the values needed to start the operation player_activity */
        auto install_bionics( const itype &type, player &installer, bool autodoc = false,
                              int skill_level = -1 ) -> bool;
        /**Success or failure of installation happens here*/
        void perform_install( bionic_id bid, bionic_id upbid, int difficulty, int success,
                              int pl_skill, const std::string &installer_name,
                              const std::vector<trait_id> &trait_to_rem );
        void do_damage_for_bionic_failure( int min_damage, int max_damage );
        void bionics_install_failure( const std::string &installer, int difficulty,
                                      int success, float adjusted_skill );

        /**Is The uninstallation possible*/
        auto can_uninstall_bionic( const bionic_id &b_id, player &installer, bool autodoc = false,
                                   int skill_level = -1 ) -> bool;
        /** Initialize all the values needed to start the operation player_activity */
        auto uninstall_bionic( const bionic_id &b_id, player &installer, bool autodoc = false,
                               int skill_level = -1 ) -> bool;
        /**Succes or failure of removal happens here*/
        void perform_uninstall( bionic_id bid, int difficulty, int success, const units::energy &power_lvl,
                                int pl_skill );
        /**When a player fails the surgery*/
        void bionics_uninstall_failure( int difficulty, int success, float adjusted_skill );

        /**Used by monster to perform surgery*/
        auto uninstall_bionic( const bionic &target_cbm, monster &installer, player &patient,
                               float adjusted_skill ) -> bool;
        /**When a monster fails the surgery*/
        void bionics_uninstall_failure( monster &installer, player &patient, int difficulty, int success,
                                        float adjusted_skill );

        /**Convert fuel to bionic power*/
        auto burn_fuel( int b, bool start = false ) -> bool;
        /**Passively produce power from PERPETUAL fuel*/
        void passive_power_gen( int b );
        /**Find fuel used by remote powered bionic*/
        auto find_remote_fuel( bool look_only = false ) -> itype_id;
        /**Consume fuel used by remote powered bionic, return amount of request unfulfilled (0 if totally successful).*/
        auto consume_remote_fuel( int amount ) -> int;
        void reset_remote_fuel();
        /**Handle heat from exothermic power generation*/
        void heat_emission( int b, int fuel_energy );
        /**Applies modifier to fuel_efficiency and returns the resulting efficiency*/
        auto get_effective_efficiency( int b, float fuel_efficiency ) -> float;

        auto get_power_level() const -> units::energy;
        auto get_max_power_level() const -> units::energy;
        void mod_power_level( const units::energy &npower );
        void mod_max_power_level( const units::energy &npower_max );
        void set_power_level( const units::energy &npower );
        void set_max_power_level( const units::energy &npower_max );
        auto is_max_power() const -> bool;
        auto has_power() const -> bool;
        auto has_max_power() const -> bool;
        auto enough_power_for( const bionic_id &bid ) const -> bool;
        void conduct_blood_analysis() const;
        // --------------- Generic Item Stuff ---------------

        struct has_mission_item_filter {
            int mission_id;
            auto operator()( const item &it ) -> bool {
                return it.mission_id == mission_id;
            }
        };

        // -2 position is 0 worn index, -3 position is 1 worn index, etc
        static auto worn_position_to_index( int position ) -> int {
            return -2 - position;
        }

        // checks to see if an item is worn
        auto is_worn( const item &thing ) const -> bool {
            for( const auto &elem : worn ) {
                if( &thing == &elem ) {
                    return true;
                }
            }
            return false;
        }

        /**
         * Asks how to use the item (if it has more than one use_method) and uses it.
         * Returns true if it destroys the item. Consumes charges from the item.
         * Multi-use items are ONLY supported when all use_methods are iuse_actor!
         */
        virtual auto invoke_item( item *, const tripoint &pt ) -> bool;
        /** As above, but with a pre-selected method. Debugmsg if this item doesn't have this method. */
        virtual auto invoke_item( item *, const std::string &, const tripoint &pt ) -> bool;
        /** As above two, but with position equal to current position */
        virtual auto invoke_item( item * ) -> bool;
        virtual auto invoke_item( item *, const std::string & ) -> bool;

        /**
         * Drop, wear, stash or otherwise try to dispose of an item consuming appropriate moves
         * @param obj item to dispose of
         * @param prompt optional message to display in any menu
         * @return whether the item was successfully disposed of
         */
        virtual auto dispose_item( item_location &&obj, const std::string &prompt = std::string() ) -> bool;

        /**
         * Has the item enough charges to invoke its use function?
         * Also checks if UPS from this player is used instead of item charges.
         */
        auto has_enough_charges( const item &it, bool show_msg ) const -> bool;

        /** Consume charges of a tool or comestible item, potentially destroying it in the process
         *  @param used item consuming the charges
         *  @param qty number of charges to consume which must be non-zero
         *  @return true if item was destroyed */
        auto consume_charges( item &used, int qty ) -> bool;

        /**
         * Calculate (but do not deduct) the number of moves required when handling (e.g. storing, drawing etc.) an item
         * @param it Item to calculate handling cost for
         * @param penalties Whether item volume and temporary effects (e.g. GRABBED, DOWNED) should be considered.
         * @param base_cost Cost due to storage type.
         * @return cost in moves ranging from 0 to MAX_HANDLING_COST
         */
        auto item_handling_cost( const item &it, bool penalties = true,
                                int base_cost = INVENTORY_HANDLING_PENALTY ) const -> int;

        /**
         * Calculate (but do not deduct) the number of moves required when storing an item in a container
         * @param it Item to calculate storage cost for
         * @param container Container to store item in
         * @param penalties Whether item volume and temporary effects (e.g. GRABBED, DOWNED) should be considered.
         * @param base_cost Cost due to storage type.
         * @return cost in moves ranging from 0 to MAX_HANDLING_COST
         */
        auto item_store_cost( const item &it, const item &container, bool penalties = true,
                             int base_cost = INVENTORY_HANDLING_PENALTY ) const -> int;

        /** Calculate (but do not deduct) the number of moves required to wear an item */
        auto item_wear_cost( const item &it ) const -> int;

        /** Wear item; returns nullopt on fail, or pointer to newly worn item on success.
         * If interactive is false, don't alert the player or drain moves on completion.
         */
        auto
        wear_item( const item &to_wear, bool interactive = true ) -> cata::optional<std::list<item>::iterator>;

        /** Returns the amount of item `type' that is currently worn */
        auto  amount_worn( const itype_id &id ) const -> int;

        /** Returns nearby items which match the provided predicate */
        auto nearby( const std::function<bool( const item *, const item * )> &func,
                                           int radius = 1 ) const -> std::vector<item_location>;

        /**
         * Similar to @ref remove_items_with, but considers only worn items and not their
         * content (@ref item::contents is not checked).
         * If the filter function returns true, the item is removed.
         */
        auto remove_worn_items_with( std::function<bool( item & )> filter ) -> std::list<item>;

        /** Return the item pointer of the item with given invlet, return nullptr if
         * the player does not have such an item with that invlet. Don't use this on npcs.
         * Only use the invlet in the user interface, otherwise always use the item position. */
        auto invlet_to_item( int invlet ) -> item *;

        // Returns the item with a given inventory position.
        auto i_at( int position ) -> item &;
        auto i_at( int position ) const -> const item &;
        /**
         * Returns the item position (suitable for @ref i_at or similar) of a
         * specific item. Returns INT_MIN if the item is not found.
         * Note that this may lose some information, for example the returned position is the
         * same when the given item points to the container and when it points to the item inside
         * the container. All items that are part of the same stack have the same item position.
         */
        auto get_item_position( const item *it ) const -> int;

        /**
         * Returns a reference to the item which will be used to make attacks.
         * At the moment it's always @ref weapon or a reference to a null item.
         */
        /*@{*/
        auto used_weapon() const -> const item &;
        auto used_weapon() -> item &;
        /*@}*/

        /**
         * Try to find a container/s on character containing ammo of type it.typeId() and
         * add charges until the container is full.
         * @param unloading Do not try to add to a container when the item was intentionally unloaded.
         * @return Remaining charges which could not be stored in a container.
         */
        auto i_add_to_container( const item &it, bool unloading ) -> int;
        auto i_add( item it, bool should_stack = true ) -> item &;

        /**
         * Try to pour the given liquid into the given container/vehicle. The transferred charges are
         * removed from the liquid item. Check the charges of afterwards to see if anything has
         * been transferred at all.
         * The functions do not consume any move points.
         * @return Whether anything has been moved at all. `false` indicates the transfer is not
         * possible at all. `true` indicates at least some of the liquid has been moved.
         */
        /**@{*/
        auto pour_into( item &container, item &liquid ) -> bool;
        auto pour_into( vehicle &veh, item &liquid ) -> bool;
        /**@}*/

        /**
         * Remove a specific item from player possession. The item is compared
         * by pointer. Contents of the item are removed as well.
         * @param pos The item position of the item to be removed. The item *must*
         * exists, use @ref has_item to check this.
         * @return A copy of the removed item.
         */
        auto i_rem( int pos ) -> item;
        /**
         * Remove a specific item from player possession. The item is compared
         * by pointer. Contents of the item are removed as well.
         * @param it A pointer to the item to be removed. The item *must* exists
         * in the players possession (one can use @ref has_item to check for this).
         * @return A copy of the removed item.
         */
        auto i_rem( const item *it ) -> item;
        void i_rem_keep_contents( int idx );
        /** Sets invlet and adds to inventory if possible, drops otherwise, returns true if either succeeded.
         *  An optional qty can be provided (and will perform better than separate calls). */
        auto i_add_or_drop( item &it, int qty = 1 ) -> bool;

        /** Only use for UI things. Returns all invlets that are currently used in
         * the player inventory, the weapon slot and the worn items. */
        auto allocated_invlets() const -> std::bitset<std::numeric_limits<char>::max()>;

        /**
         * Whether the player carries an active item of the given item type.
         */
        auto has_active_item( const itype_id &id ) const -> bool;
        auto remove_weapon() -> item;
        void remove_mission_items( int mission_id );

        /**
         * Returns the items that are ammo and have the matching ammo type.
         */
        auto get_ammo( const ammotype &at ) const -> std::vector<const item *>;

        /**
         * Searches for ammo or magazines that can be used to reload obj
         * @param obj item to be reloaded. By design any currently loaded ammunition or magazine is ignored
         * @param empty whether empty magazines should be considered as possible ammo
         * @param radius adjacent map/vehicle tiles to search. 0 for only player tile, -1 for only inventory
         */
        auto find_ammo( const item &obj, bool empty = true, int radius = 1 ) const -> std::vector<item_location>;

        /**
         * Searches for weapons and magazines that can be reloaded.
         */
        auto find_reloadables() -> std::vector<item_location>;
        /**
         * Counts ammo and UPS charges (lower of) for a given gun on the character.
         */
        auto ammo_count_for( const item &gun ) -> int;

        /** Maximum thrown range with a given item, taking all active effects into account. */
        auto throw_range( const item & ) const -> int;

        /** True if unarmed or wielding a weapon with the UNARMED_WEAPON flag */
        auto unarmed_attack() const -> bool;
        /// Checks for items, tools, and vehicles with the Lifting quality near the character
        /// returning the highest quality in range.
        auto best_nearby_lifting_assist() const -> int;

        /// Alternate version if you need to specify a different orign point for nearby vehicle sources of lifting
        /// used for operations on distant objects (e.g. vehicle installation/uninstallation)
        auto best_nearby_lifting_assist( const tripoint &world_pos ) const -> int;

        // Inventory + weapon + worn (for death, etc)
        auto inv_dump() -> std::vector<item *>;

        auto weight_carried() const -> units::mass;
        auto volume_carried() const -> units::volume;

        auto weight_carried_reduced_by( const excluded_stacks &without ) const -> units::mass;
        auto volume_carried_reduced_by( const excluded_stacks &without ) const -> units::volume;
        auto weight_capacity() const -> units::mass override;
        auto volume_capacity() const -> units::volume;
        auto volume_capacity_reduced_by(
            const units::volume &mod,
            const excluded_stacks &without = {} ) const -> units::volume;

        auto can_pick_volume( const item &it ) const -> bool;
        auto can_pick_volume( units::volume volume ) const -> bool;
        auto can_pick_weight( const item &it, bool safe = true ) const -> bool;
        auto can_pick_weight( units::mass weight, bool safe = true ) const -> bool;
        /**
         * Checks if character stats and skills meet minimum requirements for the item.
         * Prints an appropriate message if requirements not met.
         * @param it Item we are checking
         * @param context optionally override effective item when checking contextual skills
         */
        auto can_use( const item &it, const item &context = item() ) const -> bool;
        /**
         * Check character capable of wearing an item.
         * @param it Thing to be worn
         * @param with_equip_change If true returns if it could be worn if things were taken off
         */
        auto can_wear( const item &it, bool with_equip_change = false ) const -> ret_val<bool>;
        /**
         * Returns true if the character is wielding something.
         * Note: this item may not actually be used to attack.
         */
        auto is_armed() const -> bool;

        /**
         * Removes currently wielded item (if any) and replaces it with the target item.
         * @param target replacement item to wield or null item to remove existing weapon without replacing it
         * @return whether both removal and replacement were successful (they are performed atomically)
         */
        virtual auto wield( item &target ) -> bool = 0;
        /**
         * Check player capable of unwielding an item.
         * @param it Thing to be unwielded
         */
        auto can_unwield( const item &it ) const -> ret_val<bool>;

        /**
         * Check player capable of swapping the side of a worn item.
         * @param it Thing to be swapped
         */
        auto can_swap( const item &it ) const -> ret_val<bool>;

        void drop_invalid_inventory();
        /** Returns all items that must be taken off before taking off this item */
        auto get_dependent_worn_items( const item &it ) const -> std::list<item *>;
        /** Drops an item to the specified location */
        void drop( item_location loc, const tripoint &where );
        virtual void drop( const drop_locations &what, const tripoint &target, bool stash = false );

        virtual auto has_artifact_with( art_effect_passive effect ) const -> bool;

        auto is_wielding( const item &target ) const -> bool;

        auto covered_with_flag( const std::string &flag, const body_part_set &parts ) const -> bool;
        auto is_waterproof( const body_part_set &parts ) const -> bool;
        // Carried items may leak radiation or chemicals
        auto leak_level( const std::string &flag ) const -> int;

        // --------------- Clothing Stuff ---------------
        /** Returns true if the player is wearing the item. */
        auto is_wearing( const item &itm ) const -> bool;
        /** Returns true if the player is wearing an item of this type. */
        auto is_wearing( const itype_id &it ) const -> bool;
        /** Returns true if the player is wearing the item on the given body part. */
        auto is_wearing_on_bp( const itype_id &it, const bodypart_id &bp ) const -> bool;
        /** Returns true if the player is wearing an item with the given flag. */
        auto worn_with_flag( const std::string &flag,
                             const bodypart_id &bp = bodypart_str_id::NULL_ID() ) const -> bool;
        /** Returns the first worn item with a given flag. */
        auto item_worn_with_flag( const std::string &flag,
                                         const bodypart_id &bp = bodypart_str_id::NULL_ID() ) const -> const item *;

        // drawing related stuff
        /**
         * Returns a list of the IDs of overlays on this character,
         * sorted from "lowest" to "highest".
         *
         * Only required for rendering.
         */
        auto get_overlay_ids() const -> std::vector<std::string>;

        // --------------- Skill Stuff ---------------
        auto get_skill_level( const skill_id &ident ) const -> int;
        auto get_skill_level( const skill_id &ident, const item &context ) const -> int;

        auto get_all_skills() const -> const SkillLevelMap &;
        auto get_skill_level_object( const skill_id &ident ) -> SkillLevel &;
        auto get_skill_level_object( const skill_id &ident ) const -> const SkillLevel &;

        void set_skill_level( const skill_id &ident, int level );
        void mod_skill_level( const skill_id &ident, int delta );
        /** Checks whether the character's skills meet the required */
        auto meets_skill_requirements( const std::map<skill_id, int> &req,
                                       const item &context = item() ) const -> bool;
        /** Checks whether the character's skills meet the required */
        auto meets_skill_requirements( const construction &con ) const -> bool;
        /** Checks whether the character's stats meets the stats required by the item */
        auto meets_stat_requirements( const item &it ) const -> bool;
        /** Checks whether the character meets overall requirements to be able to use the item */
        auto meets_requirements( const item &it, const item &context = item() ) const -> bool;
        /** Returns a string of missed requirements (both stats and skills) */
        auto enumerate_unmet_requirements( const item &it, const item &context = item() ) const -> std::string;

        /** Returns the player's skill rust rate */
        auto rust_rate() const -> int;

        // Mental skills and stats
        /** Returns the player's reading speed */
        auto read_speed( bool return_stat_effect = true ) const -> int;

        // --------------- Other Stuff ---------------

        /** return the calendar::turn the character expired */
        auto get_time_died() const -> time_point {
            return time_died;
        }
        /** set the turn the turn the character died if not already done */
        void set_time_died( const time_point &time ) {
            if( time_died != calendar::before_time_starts ) {
                time_died = time;
            }
        }
        // magic mod
        pimpl<known_magic> magic;

        /** Calls Creature::normalize()
         *  nulls out the player's weapon
         *  Should only be called through player::normalize(), not on it's own!
         */
        void normalize() override;
        void die( Creature *nkiller ) override;

        auto get_name() const -> std::string override;

        auto get_grammatical_genders() const -> std::vector<std::string> override;

        /**
         * It is supposed to hide the query_yn to simplify player vs. npc code.
         */
        template<typename ...Args>
        auto query_yn( const char *const msg, Args &&... args ) const -> bool {
            return query_yn( string_format( msg, std::forward<Args>( args ) ... ) );
        }
        virtual auto query_yn( const std::string &msg ) const -> bool = 0;

        auto is_immune_field( const field_type_id &fid ) const -> bool override;
        /** Returns true is the player is protected from electric shocks */
        auto is_elec_immune() const -> bool override;
        /** Returns true if the player is immune to this kind of effect */
        auto is_immune_effect( const efftype_id & ) const -> bool override;
        /** Returns true if the player is immune to this kind of damage */
        auto is_immune_damage( damage_type ) const -> bool override;
        /** Returns true if the player is protected from radiation */
        auto is_rad_immune() const -> bool;
        /** Returns true if the player is immune to throws */
        auto is_throw_immune() const -> bool;

        /** Returns true if the player has some form of night vision */
        auto has_nv() -> bool;

        /**
         * Returns >0 if character is sitting/lying and relatively inactive.
         * 1 represents sleep on comfortable bed, so anything above that should be rare.
         */
        auto rest_quality() const -> float;
        /**
         * Average hit points healed per turn.
         */
        auto healing_rate( float at_rest_quality ) const -> float;
        /**
         * Average hit points healed per turn from healing effects.
         */
        auto healing_rate_medicine( float at_rest_quality, const bodypart_id &bp ) const -> float;

        /**
         * Goes over all mutations, gets min and max of a value with given name
         * @return min( 0, lowest ) + max( 0, highest )
         */
        auto mutation_value( const std::string &val ) const -> float;

        /**
         * Goes over all mutations, returning the sum of the social modifiers
         */
        auto get_mutation_social_mods() const -> social_modifiers;

        /** Color's character's tile's background */
        auto symbol_color() const -> nc_color override;

        auto extended_description() const -> std::string override;

        /** Returns a random name from NAMES_* */
        void pick_name( bool bUseDefault = false );
        /** Get the idents of all base traits. */
        auto get_base_traits() const -> std::vector<trait_id>;
        /** Get the idents of all traits/mutations. */
        auto get_mutations( bool include_hidden = true ) const -> std::vector<trait_id>;
        auto get_vision_modes() const -> const std::bitset<NUM_VISION_MODES> & {
            return vision_mode_cache;
        }
        /** Clear the skills map, setting all levels to 0 */
        void clear_skills();
        /** Empties the trait and mutations lists */
        void clear_mutations();
        /** Returns true if the player has crossed a mutation threshold
         *  Player can only cross one mutation threshold.
         */
        auto crossed_threshold() const -> bool;

        // --------------- Values ---------------
        std::string name;
        bool male = true;

        std::list<item> worn;
        std::array<int, num_hp_parts> damage_bandaged, damage_disinfected;
        bool nv_cached = false;
        // Means player sit inside vehicle on the tile he is now
        bool in_vehicle = false;
        bool hauling = false;

        player_activity stashed_outbounds_activity;
        player_activity stashed_outbounds_backlog;
        player_activity activity;
        std::list<player_activity> backlog;
        cata::optional<tripoint> destination_point;
        inventory inv;
        itype_id last_item;
        item weapon;

        int scent = 0;
        pimpl<bionic_collection> my_bionics;
        pimpl<character_martial_arts> martial_arts_data;

        stomach_contents stomach;
        pimpl<consumption_history_t> consumption_history;

        int oxygen = 0;
        int tank_plut = 0;
        int reactor_plut = 0;
        int slow_rad = 0;

        int focus_pool = 0;
        int cash = 0;
        std::set<character_id> follower_ids;
        weak_ptr_fast<Creature> last_target;
        cata::optional<tripoint> last_target_pos;
        // Save favorite ammo location
        item_location ammo_location;
        std::set<tripoint_abs_omt> camps;
        /* crafting inventory cached time */
        time_point cached_time;

        std::vector <addiction> addictions;
        /** Adds an addiction to the player */
        void add_addiction( add_type type, int strength );
        /** Removes an addition from the player */
        void rem_addiction( add_type type );
        /** Returns true if the player has an addiction of the specified type */
        auto has_addiction( add_type type ) const -> bool;
        /** Returns the intensity of the specified addiction */
        auto  addiction_level( add_type type ) const -> int;

        shared_ptr_fast<monster> mounted_creature;
        // for loading NPC mounts
        int mounted_creature_id = 0;
        // for vehicle work
        int activity_vehicle_part_index = -1;

        // Hauling items on the ground
        void start_hauling();
        void stop_hauling();
        auto is_hauling() const -> bool;

        // Has a weapon, inventory item or worn item with flag
        auto has_item_with_flag( const std::string &flag, bool need_charges = false ) const -> bool;
        /**
         * All items that have the given flag (@ref item::has_flag).
         */
        auto all_items_with_flag( const std::string &flag ) const -> std::vector<const item *>;

        auto has_charges( const itype_id &it, int quantity,
                          const std::function<bool( const item & )> &filter = return_true<item> ) const -> bool;

        // has_amount works ONLY for quantity.
        // has_charges works ONLY for charges.
        auto use_amount( itype_id it, int quantity,
                                    const std::function<bool( const item & )> &filter = return_true<item> ) -> std::list<item>;
        // Uses up charges
        auto use_charges_if_avail( const itype_id &it, int quantity ) -> bool;

        // Uses up charges
        auto use_charges( const itype_id &what, int qty,
                                     const std::function<bool( const item & )> &filter = return_true<item> ) -> std::list<item>;

        auto has_fire( int quantity ) const -> bool;
        void use_fire( int quantity );
        void assign_stashed_activity();
        auto check_outbounds_activity( const player_activity &act, bool check_only = false ) -> bool;
        /** Legacy activity assignment, does not work for any activites using
         * the new activity_actor class and may cause issues with resuming.
         * TODO: delete this once migration of activites to the activity_actor system is complete
         */
        void assign_activity( const activity_id &type, int moves = calendar::INDEFINITELY_LONG,
                              int index = -1, int pos = INT_MIN,
                              const std::string &name = "" );
        /** Assigns activity to player, possibly resuming old activity if it's similar enough. */
        void assign_activity( const player_activity &act, bool allow_resume = true );
        /** Check if player currently has a given activity */
        auto has_activity( const activity_id &type ) const -> bool;
        /** Check if player currently has any of the given activities */
        auto has_activity( const std::vector<activity_id> &types ) const -> bool;
        void resume_backlog_activity();
        void cancel_activity();
        void cancel_stashed_activity();
        auto get_stashed_activity() const -> player_activity;
        void set_stashed_activity( const player_activity &act,
                                   const player_activity &act_back = player_activity() );
        auto has_stashed_activity() const -> bool;
        void initialize_stomach_contents();

        /** Stable base metabolic rate due to traits */
        auto metabolic_rate_base() const -> float;
        /** Current metabolic rate due to traits, hunger, speed, etc. */
        auto metabolic_rate() const -> float;
        // Your mass + "kg/lbs"
        auto get_weight_string() const -> std::string;
        // gets the max value healthy you can be
        auto get_max_healthy() const -> int;
        // calculates the BMI
        auto bmi() const -> float;
        // returns amount of calories burned in a day given various metabolic factors
        auto bmr() const -> int;
        // Reset age and height to defaults for consistent test results
        void reset_chargen_attributes();
        // age in years, determined at character creation
        auto base_age() const -> int;
        void set_base_age( int age );
        void mod_base_age( int mod );
        // age in years
        auto age() const -> int;
        auto age_string() const -> std::string;
        // returns the height in cm
        auto base_height() const -> int;
        void set_base_height( int height );
        void mod_base_height( int mod );
        auto height_string() const -> std::string;
        // returns the height of the player character in cm
        auto height() const -> int;
        // returns bodyweight of the Character
        auto bodyweight() const -> units::mass;
        // returns total weight of installed bionics
        auto bionics_weight() const -> units::mass;

        /** Returns overall bashing resistance for the body_part */
        auto get_armor_bash( bodypart_id bp ) const -> int override;
        /** Returns overall cutting resistance for the body_part */
        auto get_armor_cut( bodypart_id bp ) const -> int override;
        /** Returns overall bullet resistance for the body_part */
        auto get_armor_bullet( bodypart_id bp ) const -> int override;
        /** Returns bashing resistance from the creature and armor only */
        auto get_armor_bash_base( bodypart_id bp ) const -> int override;
        /** Returns cutting resistance from the creature and armor only */
        auto get_armor_cut_base( bodypart_id bp ) const -> int override;
        /** Returns cutting resistance from the creature and armor only */
        auto get_armor_bullet_base( bodypart_id bp ) const -> int override;
        /** Returns overall env_resist on a body_part */
        auto get_env_resist( bodypart_id bp ) const -> int override;
        /** Returns overall acid resistance for the body part */
        auto get_armor_acid( bodypart_id bp ) const -> int;
        /** Returns overall resistance to given type on the bod part */
        auto get_armor_type( damage_type dt, bodypart_id bp ) const -> int override;
        auto get_all_armor_type( damage_type dt,
                const std::map<bodypart_id, std::vector<const item *>> &clothing_map ) const -> std::map<bodypart_id, int>;

        auto get_stim() const -> int;
        void set_stim( int new_stim );
        void mod_stim( int mod );

        auto get_rad() const -> int;
        void set_rad( int new_rad );
        void mod_rad( int mod );

        auto get_stamina() const -> int;
        auto get_stamina_max() const -> int;
        void set_stamina( int new_stamina );
        void mod_stamina( int mod );
        void burn_move_stamina( int moves );
        auto stamina_move_cost_modifier() const -> float;
        /** Regenerates stamina */
        void update_stamina( int turns );

    protected:
        void on_damage_of_type( int adjusted_damage, damage_type type, const bodypart_id &bp ) override;
    public:
        /** Called when an item is worn */
        void on_item_wear( const item &it );
        /** Called when an item is taken off */
        void on_item_takeoff( const item &it );
        /** Called when an item is washed */
        void on_worn_item_washed( const item &it );
        /** Called when effect intensity has been changed */
        void on_effect_int_change( const efftype_id &effect_type, int intensity,
                                   const bodypart_str_id &bp ) override;
        /** Called when a mutation is gained */
        void on_mutation_gain( const trait_id &mid );
        /** Called when a mutation is lost */
        void on_mutation_loss( const trait_id &mid );
        /** Called when a stat is changed */
        void on_stat_change( const std::string &stat, int value ) override;
        /** Returns an unoccupied, safe adjacent point. If none exists, returns player position. */
        auto adjacent_tile() const -> tripoint;
        /** Removes "sleep" and "lying_down" */
        void wake_up();
        // how loud a character can shout. based on mutations and clothing
        auto get_shout_volume() const -> int;
        // shouts a message
        void shout( std::string msg = "", bool order = false );
        /** Handles Character vomiting effects */
        void vomit();
        // adds total healing to the bodypart. this is only a counter.
        void healed_bp( int bp, int amount );

        // the amount healed per bodypart per day
        std::array<int, num_hp_parts> healed_total;

        std::map<std::string, int> mutation_category_level;

        auto adjust_for_focus( int amount ) const -> int;
        void update_type_of_scent( bool init = false );
        void update_type_of_scent( const trait_id &mut, bool gain = true );
        void set_type_of_scent( const scenttype_id &id );
        auto get_type_of_scent() const -> scenttype_id;
        /**restore scent after masked_scent effect run out or is removed by water*/
        void restore_scent();
        /** Modifies intensity of painkillers  */
        void mod_painkiller( int npkill );
        /** Sets intensity of painkillers  */
        void set_painkiller( int npkill );
        /** Returns intensity of painkillers  */
        auto get_painkiller() const -> int;
        void react_to_felt_pain( int intensity );

        void spores();
        void blossoms();

        /** Handles rooting effects */
        void rooted_message() const;
        void rooted();

        /** Adds "sleep" to the player */
        void fall_asleep();
        void fall_asleep( const time_duration &duration );
        /** Checks to see if the player is using floor items to keep warm, and return the name of one such item if so */
        auto is_snuggling() const -> std::string;

        auto power_rating() const -> float override;
        auto speed_rating() const -> float override;

        /** Returns the item in the player's inventory with the highest of the specified quality*/
        auto item_with_best_of_quality( const quality_id &qid ) -> item &;
        /**
         * Check whether the this player can see the other creature with infrared. This implies
         * this player can see infrared and the target is visible with infrared (is warm).
         * And of course a line of sight exists.
        */
        auto sees_with_infrared( const Creature &critter ) const -> bool;
        // Put corpse+inventory on map at the place where this is.
        void place_corpse();
        // Put corpse+inventory on defined om tile
        void place_corpse( const tripoint_abs_omt &om_target );

        /** Returns the player's modified base movement cost */
        auto  run_cost( int base_cost, bool diag = false ) const -> int;
        auto get_pathfinding_settings() const -> const pathfinding_settings & override;
        auto get_path_avoid() const -> std::set<tripoint> override;
        /**
         * Get all hostile creatures currently visible to this player.
         */
        auto get_hostile_creatures( int range ) const -> std::vector<Creature *>;

        /**
         * Returns all creatures that this player can see and that are in the given
         * range. This player object itself is never included.
         * The player character (g->u) is checked and might be included (if applicable).
         * @param range The maximal distance (@ref rl_dist), creatures at this distance or less
         * are included.
         */
        auto get_visible_creatures( int range ) const -> std::vector<Creature *>;
        /** Returns an enumeration of visible mutations with colors */
        auto visible_mutations( int visibility_cap ) const -> std::string;
        auto get_destination_activity() const -> player_activity;
        void set_destination_activity( const player_activity &new_destination_activity );
        void clear_destination_activity();
        /** Returns warmth provided by armor, etc. */
        auto warmth( const std::map<bodypart_id, std::vector<const item *>>
                                           &clothing_map ) const -> std::map<bodypart_id, int>;
        /** Can the player lie down and cover self with blankets etc. **/
        auto can_use_floor_warmth() const -> bool;
        /**
         * Warmth from terrain, furniture, vehicle furniture and traps.
         * Can be negative.
         **/
        static auto floor_bedding_warmth( const tripoint &pos ) -> int;
        /** Warmth from clothing on the floor **/
        static auto floor_item_warmth( const tripoint &pos ) -> int;
        /** Final warmth from the floor **/
        auto floor_warmth( const tripoint &pos ) const -> int;

        /** Correction factor of the body temperature due to traits and mutations **/
        auto bodytemp_modifier_traits( bool overheated ) const -> int;
        /** Correction factor of the body temperature due to traits and mutations for player lying on the floor **/
        auto bodytemp_modifier_traits_floor() const -> int;
        /** Value of the body temperature corrected by climate control **/
        auto temp_corrected_by_climate_control( int temperature ) const -> int;

        auto in_sleep_state() const -> bool override;

        /** Set vitamin deficiency/excess disease states dependent upon current vitamin levels */
        void update_vitamins( const vitamin_id &vit );
        /**
         * Check current level of a vitamin
         *
         * Accesses level of a given vitamin.  If the vitamin_id specified does not
         * exist then this function simply returns 0.
         *
         * @param vit ID of vitamin to check level for.
         * @returns current level for specified vitamin
         */
        auto vitamin_get( const vitamin_id &vit ) const -> int;
        /**
         * Sets level of a vitamin or returns false if id given in vit does not exist
         *
         * @note status effects are still set for deficiency/excess
         *
         * @param[in] vit ID of vitamin to adjust quantity for
         * @param[in] qty Quantity to set level to
         * @returns false if given vitamin_id does not exist, otherwise true
         */
        auto vitamin_set( const vitamin_id &vit, int qty ) -> bool;
        /**
          * Add or subtract vitamins from character storage pools
         * @param vit ID of vitamin to modify
         * @param qty amount by which to adjust vitamin (negative values are permitted)
         * @param capped if true prevent vitamins which can accumulate in excess from doing so
         * @return adjusted level for the vitamin or zero if vitamin does not exist
         */
        auto vitamin_mod( const vitamin_id &vit, int qty, bool capped = true ) -> int;
        void vitamins_mod( const std::map<vitamin_id, int> &, bool capped = true );
        /** Get vitamin usage rate (minutes per unit) accounting for bionics, mutations and effects */
        auto vitamin_rate( const vitamin_id &vit ) const -> time_duration;

        /** Handles the nutrition value for a comestible **/
        auto nutrition_for( const item &comest ) const -> int;
        /** Can the food be [theoretically] eaten no matter the consequen
        ces? */
        auto can_eat( const item &food ) const -> ret_val<edible_rating>;
        /**
         * Same as @ref can_eat, but takes consequences into account.
         * Asks about them if @param interactive is true, refuses otherwise.
         */
        auto will_eat( const item &food, bool interactive = false ) const -> ret_val<edible_rating>;
        /** Determine character's capability of recharging their CBMs. */
        auto can_feed_reactor_with( const item &it ) const -> bool;
        auto can_feed_furnace_with( const item &it ) const -> bool;
        auto get_cbm_rechargeable_with( const item &it ) const -> rechargeable_cbm;
        auto get_acquirable_energy( const item &it, rechargeable_cbm cbm ) const -> int;
        auto get_acquirable_energy( const item &it ) const -> int;

        /**
        * Recharge CBMs whenever possible.
        * @return true when recharging was successful.
        */
        auto feed_reactor_with( item &it ) -> bool;
        auto feed_furnace_with( item &it ) -> bool;
        auto fuel_bionic_with( item &it ) -> bool;
        /** Used to apply stimulation modifications from food and medication **/
        void modify_stimulation( const islot_comestible &comest );
        /** Used to apply fatigue modifications from food and medication **/
        void modify_fatigue( const islot_comestible &comest );
        /** Used to apply radiation from food and medication **/
        void modify_radiation( const islot_comestible &comest );
        /** Used to apply addiction modifications from food and medication **/
        void modify_addiction( const islot_comestible &comest );
        /** Used to apply health modifications from food and medication **/
        void modify_health( const islot_comestible &comest );
        /** Handles the effects of consuming an item */
        auto consume_effects( item &food ) -> bool;
        /** Check character's capability of consumption overall */
        auto can_consume( const item &it ) const -> bool;
        /** True if the character has enough skill (in cooking or survival) to estimate time to rot */
        auto can_estimate_rot() const -> bool;
        /** Check whether character can consume this very item */
        auto can_consume_as_is( const item &it ) const -> bool;
        auto can_consume_for_bionic( const item &it ) const -> bool;
        /**
         * Returns a reference to the item itself (if it's consumable),
         * the first of its contents (if it's consumable) or null item otherwise.
         * WARNING: consumable does not necessarily guarantee the comestible type.
         */
        auto get_consumable_from( item &it ) const -> item &;

        /** Get calorie & vitamin contents for a comestible, taking into
         * account character traits */
        /** Get range of possible nutrient content, for a particular recipe,
         * depending on choice of ingredients */
        auto compute_nutrient_range(
            const item &, const recipe_id &,
            const cata::flat_set<std::string> &extra_flags = {} ) const -> std::pair<nutrients, nutrients>;
        /** Same, but across arbitrary recipes */
        auto compute_nutrient_range(
            const itype_id &, const cata::flat_set<std::string> &extra_flags = {} ) const -> std::pair<nutrients, nutrients>;
        /** Returns allergy type or MORALE_NULL if not allergic for this character */
        auto allergy_type( const item &food ) const -> morale_type;
        auto compute_effective_nutrients( const item & ) const -> nutrients;
        /** Returns true if the character is wearing something on the entered body part */
        auto wearing_something_on( const bodypart_id &bp ) const -> bool;
        /** Returns true if the character is wearing something occupying the helmet slot */
        auto is_wearing_helmet() const -> bool;
        /** Returns the total encumbrance of all SKINTIGHT and HELMET_COMPAT items coveringi
         *  the head */
        auto head_cloth_encumbrance() const -> int;
        /** Same as footwear factor, but for arms */
        auto armwear_factor() const -> double;
        /** Returns 1 if the player is wearing an item of that count on one foot, 2 if on both,
         *  and zero if on neither */
        auto shoe_type_count( const itype_id &it ) const -> int;
        /** Returns 1 if the player is wearing something on both feet, .5 if on one,
         *  and 0 if on neither */
        auto footwear_factor() const -> double;
        /** Returns true if the player is wearing something on their feet that is not SKINTIGHT */
        auto is_wearing_shoes( const side &which_side = side::BOTH ) const -> bool;

        /** Swap side on which item is worn; returns false on fail. If interactive is false, don't alert player or drain moves */
        auto change_side( item &it, bool interactive = true ) -> bool;
        auto change_side( item_location &loc, bool interactive = true ) -> bool;

        auto get_check_encumbrance() -> bool {
            return check_encumbrance;
        }
        void set_check_encumbrance( bool new_check ) {
            check_encumbrance = new_check;
        }
        /** Ticks down morale counters and removes them */
        void update_morale();
        /** Ensures persistent morale effects are up-to-date */
        void apply_persistent_morale();
        /** Used to apply morale modifications from food and medication **/
        void modify_morale( item &food, int nutr = 0 );
        // Modified by traits, &c
        auto get_morale_level() const -> int;
        void add_morale( const morale_type &type, int bonus, int max_bonus = 0,
                         const time_duration &duration = 1_hours,
                         const time_duration &decay_start = 30_minutes, bool capped = false,
                         const itype *item_type = nullptr );
        auto has_morale( const morale_type &type ) const -> bool;
        auto get_morale( const morale_type &type ) const -> int;
        void rem_morale( const morale_type &type );
        void clear_morale();
        auto has_morale_to_read() const -> bool;
        auto has_morale_to_craft() const -> bool;
        auto crafting_inventory( bool clear_path ) -> const inventory &;
        auto crafting_inventory( const tripoint &src_pos = tripoint_zero,
                                             int radius = PICKUP_RANGE, bool clear_path = true ) -> const inventory &;
        void invalidate_crafting_inventory();

        /** Returns all known recipes. */
        auto get_learned_recipes() const -> const recipe_subset &;

        auto knows_recipe( const recipe *rec ) const -> bool;
        void learn_recipe( const recipe *rec );
        auto can_learn_by_disassembly( const recipe &rec ) const -> bool;

        /** Checks permanent morale for consistency and recovers it when an inconsistency is found. */
        auto check_and_recover_morale() -> bool;

        /** Handles the enjoyability value for a comestible. First value is enjoyability, second is cap. **/
        auto fun_for( const item &comest ) const -> std::pair<int, int>;

        /** Handles a large number of timers decrementing and other randomized effects */
        void suffer();
        /** Handles mitigation and application of radiation */
        auto irradiate( float rads, bool bypass = false ) -> bool;
        /** Handles the chance for broken limbs to spontaneously heal to 1 HP */
        void mend( int rate_multiplier );

        /** Creates an auditory hallucination */
        void sound_hallu();

        /** Drenches the player with water, saturation is the percent gotten wet */
        void drench( int saturation, const body_part_set &flags, bool ignore_waterproof );
        /** Recalculates morale penalty/bonus from wetness based on mutations, equipment and temperature */
        void apply_wetness_morale( int temperature );
        auto short_description_parts() const -> std::vector<std::string>;
        auto short_description() const -> std::string;
        auto print_info( const catacurses::window &w, int vStart, int vLines, int column ) const -> int override;
        // Checks whether a player can hear a sound at a given volume and location.
        auto can_hear( const tripoint &source, int volume ) const -> bool;
        // Returns a multiplier indicating the keenness of a player's hearing.
        auto hearing_ability() const -> float;

        using trap_map = std::map<tripoint, std::string>;
        auto knows_trap( const tripoint &pos ) const -> bool;
        void add_known_trap( const tripoint &pos, const trap &t );
        /** Define color for displaying the body temperature */
        auto bodytemp_color( int bp ) const -> nc_color;

        // see Creature::sees
        auto sees( const tripoint &t, bool is_player = false, int range_mod = 0 ) const -> bool override;
        // see Creature::sees
        auto sees( const Creature &critter ) const -> bool override;
        auto attitude_to( const Creature &other ) const -> Attitude override;

        // used in debugging all health
        auto get_lowest_hp() const -> int;
        auto has_weapon() const -> bool override;
        void shift_destination( const point &shift );
        // Auto move methods
        void set_destination( const std::vector<tripoint> &route,
                              const player_activity &new_destination_activity = player_activity() );
        void clear_destination();
        auto has_distant_destination() const -> bool;

        // true if the player is auto moving, or if the player is going to finish
        // auto moving but the destination is not yet reset, such as in avatar_action::move
        auto is_auto_moving() const -> bool;
        // true if there are further moves in the auto move route
        auto has_destination() const -> bool;
        // true if player has destination activity AND is standing on destination tile
        auto has_destination_activity() const -> bool;
        // starts destination activity and cleans up to ensure it is called only once
        void start_destination_activity();
        auto get_auto_move_route() -> std::vector<tripoint> &;
        auto get_next_auto_move_direction() -> action_id;
        auto defer_move( const tripoint &next ) -> bool;

    protected:
        Character();
        Character( Character && );
        auto operator=( Character && ) -> Character &;
        struct trait_data {
            /** Whether the mutation is activated. */
            bool powered = false;
            /** Key to select the mutation in the UI. */
            char key = ' ';
            /**
             * Time (in turns) until the mutation increase hunger/thirst/fatigue according
             * to its cost (@ref mutation_branch::cost). When those costs have been paid, this
             * is reset to @ref mutation_branch::cooldown.
             */
            int charge = 0;
            void serialize( JsonOut &json ) const;
            void deserialize( JsonIn &jsin );
        };

        // The player's position on the local map.
        tripoint position;

        /** Bonuses to stats, calculated each turn */
        int str_bonus = 0;
        int dex_bonus = 0;
        int per_bonus = 0;
        int int_bonus = 0;

        /** How healthy the character is. */
        int healthy = 0;
        int healthy_mod = 0;

        /** age in years at character creation */
        int init_age = 25;
        /**height at character creation*/
        int init_height = 175;
        /** Size class of character. */
        m_size size_class = MS_MEDIUM;

        trap_map known_traps;
        pimpl<char_encumbrance_data> encumbrance_cache;
        mutable std::map<std::string, double> cached_info;
        bool bio_soporific_powered_at_last_sleep_check = false;
        /** last time we checked for sleep */
        time_point last_sleep_check = calendar::turn_zero;
        /** warnings from a faction about bad behavior */
        std::map<faction_id, std::pair<int, time_point>> warning_record;
        /**
         * Traits / mutations of the character. Key is the mutation id (it's also a valid
         * key into @ref mutation_data), the value describes the status of the mutation.
         * If there is not entry for a mutation, the character does not have it. If the map
         * contains the entry, the character has the mutation.
         */
        std::unordered_map<trait_id, trait_data> my_mutations;
        /**
         * Contains mutation ids of the base traits.
         */
        std::unordered_set<trait_id> my_traits;
        /**
         * Pointers to mutation branches in @ref my_mutations.
         */
        std::vector<const mutation_branch *> cached_mutations;

        void store( JsonOut &json ) const;
        void load( const JsonObject &data );

        // --------------- Values ---------------
        /** Character skills. */
        pimpl<SkillLevelMap> _skills;
        /** Stamp of character skills. @ref learned_recipes are valid only with this set of skills. */
        mutable pimpl<SkillLevelMap> autolearn_skills_stamp;
        /** Subset of learned recipes. Needs to be mutable for lazy initialization. */
        mutable pimpl<recipe_subset> learned_recipes;

        // Cached vision values.
        std::bitset<NUM_VISION_MODES> vision_mode_cache;
        // "Raw" night vision range - just stats+mutations+items
        float nv_range = 0;
        int sight_max = 0;

        // turn the character expired, if calendar::before_time_starts it has not been set yet.
        // TODO: change into an optional<time_point>
        time_point time_died = calendar::before_time_starts;

        /**
         * Cache for pathfinding settings.
         * Most of it isn't changed too often, hence mutable.
         */
        mutable pimpl<pathfinding_settings> path_settings;

        // faction API versions
        // 2 - allies are in your_followers faction; NPCATT_FOLLOW is follower but not an ally
        // 0 - allies may be in your_followers faction; NPCATT_FOLLOW is an ally (legacy)
        // faction API versioning
        int faction_api_version = 2;
        // A temp variable used to inform the game which faction to link
        faction_id fac_id;
        faction *my_fac = nullptr;

        character_movemode move_mode = CMM_WALK;
        /** Current deficiency/excess quantity for each vitamin */
        std::map<vitamin_id, int> vitamin_levels;

        pimpl<player_morale> morale;

    public:
        /**
         * Map body parts to their total exposure, from 0.0 (fully covered) to 1.0 (buck naked).
         * Clothing layers are multiplied, ex. two layers of 50% coverage will leave only 25% exposed.
         * Used to determine suffering effects of albinism and solar sensitivity.
         */
        auto bodypart_exposure() -> std::map<bodypart_id, float>;
    private:
        /** suffer() subcalls */
        void suffer_water_damage( const mutation_branch &mdata );
        void suffer_mutation_power( const mutation_branch &mdata, Character::trait_data &tdata );
        void suffer_while_underwater();
        void suffer_from_addictions();
        void suffer_while_awake( int current_stim );
        void suffer_from_chemimbalance();
        void suffer_from_schizophrenia();
        void suffer_from_asthma( int current_stim );
        void suffer_in_sunlight();
        void suffer_from_sunburn();
        void suffer_from_other_mutations();
        void suffer_from_radiation();
        void suffer_from_bad_bionics();
        void suffer_from_artifacts();
        void suffer_from_stimulants( int current_stim );
        void suffer_without_sleep( int sleep_deprivation );
        /**
         * Check whether the other creature is in range and can be seen by this creature.
         * @param critter Creature to check for visibility
         * @param range The maximal distance (@ref rl_dist), creatures at this distance or less
         * are included.
         */
        auto is_visible_in_range( const Creature &critter, int range ) const -> bool;

        player_activity destination_activity;
        // A unique ID number, assigned by the game class. Values should never be reused.
        character_id id;

        units::energy power_level;
        units::energy max_power_level;

        /** Needs (hunger, starvation, thirst, fatigue, etc.) */
        int stored_calories = 0;

        int thirst = 0;
        int stamina = 0;

        int fatigue = 0;
        int sleep_deprivation = 0;
        bool check_encumbrance = true;

        int stim = 0;
        int pkill = 0;

        int radiation = 0;

        std::vector<tripoint> auto_move_route;
        // Used to make sure auto move is canceled if we stumble off course
        cata::optional<tripoint> next_expected_position;
        scenttype_id type_of_scent;

        struct weighted_int_list<std::string> melee_miss_reasons;

        int cached_moves = 0;
        tripoint cached_position;
        inventory cached_crafting_inventory;

    protected:
        // a cache of all active enchantment values.
        // is recalculated every turn in Character::recalculate_enchantment_cache
        pimpl<enchantment> enchantment_cache;

        /** Amount of time the player has spent in each overmap tile. */
        std::unordered_map<point_abs_omt, time_duration> overmap_time;

    public:
        // TODO: make these private
        std::array<int, num_bp> temp_cur, frostbite_timer, temp_conv;
        std::array<int, num_bp> body_wetness;
        std::array<int, num_bp> drench_capacity;

        time_point next_climate_control_check;
        bool last_climate_control_ret = false;
};

auto get_player_character() -> Character &;

// TODO: Move to its own file (it's not Character-specific)
namespace vision
{
/**
 * Returns the light level that darkness will have at this range from player.
 * Assumes pure air transparency.
 */
auto threshold_for_nv_range( float range ) -> float;

auto nv_range_from_per( int per ) -> float;
auto nv_range_from_eye_encumbrance( int enc ) -> float;
} // namespace vision

namespace warmth
{

auto from_clothing( const
        std::map<bodypart_id, std::vector<const item *>> &clothing_map ) -> std::map<bodypart_id, int>;
auto bonus_from_clothing( const
        std::map<bodypart_id, std::vector<const item *>> &clothing_map ) -> std::map<bodypart_id, int>;
auto from_effects( const Character &c ) -> std::map<bodypart_id, int>;

/** Returns wind resistance provided by armor, etc **/
auto wind_resistance_from_clothing(
    const std::map<bodypart_id, std::vector<const item *>> &clothing_map ) -> std::map<bodypart_id, int>;
} // namespace warmth

#endif // CATA_SRC_CHARACTER_H
