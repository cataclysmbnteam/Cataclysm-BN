#pragma once
#ifndef CATA_SRC_MONSTER_H
#define CATA_SRC_MONSTER_H

#include <bitset>
#include <climits>
#include <cstddef>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "bodypart.h"
#include "calendar.h"
#include "character_id.h"
#include "color.h"
#include "creature.h"
#include "cursesdef.h"
#include "damage.h"
#include "effect.h"
#include "enums.h"
#include "optional.h"
#include "pldata.h"
#include "point.h"
#include "type_id.h"
#include "units.h"
#include "value_ptr.h"
#include "visitable.h"

class Character;
class JsonIn;
class JsonObject;
class JsonOut;
class effect;
class item;
class player;
struct dealt_projectile_attack;
struct pathfinding_settings;
struct trap;

enum class mon_trigger;

class mon_special_attack
{
    public:
        int cooldown = 0;
        bool enabled = true;

        void serialize( JsonOut &json ) const;
        // deserialize inline in monster::load due to backwards/forwards compatibility concerns
};

enum monster_attitude {
    MATT_NULL = 0,
    MATT_FRIEND,
    MATT_FPASSIVE,
    MATT_FLEE,
    MATT_IGNORE,
    MATT_FOLLOW,
    MATT_ATTACK,
    MATT_ZLAVE,
    NUM_MONSTER_ATTITUDES
};

enum monster_effect_cache_fields {
    MOVEMENT_IMPAIRED = 0,
    FLEEING,
    VISION_IMPAIRED,
    NUM_MEFF
};

enum monster_horde_attraction {
    MHA_NULL = 0,
    MHA_ALWAYS,
    MHA_LARGE,
    MHA_OUTDOORS,
    MHA_OUTDOORS_AND_LARGE,
    MHA_NEVER,
    NUM_MONSTER_HORDE_ATTRACTION
};

class monster : public Creature, public visitable<monster>
{
        friend class editmap;
    public:
        monster();
        monster( const mtype_id &id );
        monster( const mtype_id &id, const tripoint &pos );
        monster( const monster & );
        monster( monster && );
        ~monster() override;
        auto operator=( const monster & ) -> monster &;
        auto operator=( monster && ) -> monster &;

        auto is_monster() const -> bool override {
            return true;
        }
        auto as_monster() -> monster * override {
            return this;
        }
        auto as_monster() const -> const monster * override {
            return this;
        }

        void poly( const mtype_id &id );
        auto can_upgrade() const -> bool;
        void hasten_upgrade();
        auto get_upgrade_time() const -> int;
        void allow_upgrade();
        void try_upgrade( bool pin_time );
        void try_reproduce();
        void refill_udders();
        void spawn( const tripoint &p );
        auto get_size() const -> m_size override;
        auto get_weight() const -> units::mass override;
        auto weight_capacity() const -> units::mass override;
        auto get_volume() const -> units::volume;
        auto get_hp( const bodypart_id & ) const -> int override;
        auto get_hp() const -> int override;
        auto get_hp_max( const bodypart_id & ) const -> int override;
        auto get_hp_max() const -> int override;
        auto hp_percentage() const -> int override;

        auto get_mountable_weight_ratio() const -> float;

        // Access
        auto get_name() const -> std::string override;
        auto name( unsigned int quantity = 1 ) const -> std::string; // Returns the monster's formal name
        auto name_with_armor() const -> std::string; // Name, with whatever our armor is called
        // the creature-class versions of the above
        auto disp_name( bool possessive = false, bool capitalize_first = false ) const -> std::string override;
        auto skin_name() const -> std::string override;
        void get_HP_Bar( nc_color &color, std::string &text ) const;
        auto get_attitude() const -> std::pair<std::string, nc_color>;
        auto print_info( const catacurses::window &w, int vStart, int vLines, int column ) const -> int override;

        // Information on how our symbol should appear
        auto basic_symbol_color() const -> nc_color override;
        auto symbol_color() const -> nc_color override;
        auto symbol() const -> const std::string & override;
        auto is_symbol_highlighted() const -> bool override;

        auto color_with_effects() const -> nc_color; // Color with fire, beartrapped, etc.

        auto extended_description() const -> std::string override;
        // Inverts color if inv==true
        auto has_flag( m_flag f ) const -> bool override; // Returns true if f is set (see mtype.h)
        auto can_see() const -> bool;      // MF_SEES and no MF_BLIND
        auto can_hear() const -> bool;     // MF_HEARS and no MF_DEAF
        auto can_submerge() const -> bool; // MF_AQUATIC or swims() or MF_NO_BREATH, and not MF_ELECTRONIC
        auto can_drown() const -> bool;    // MF_AQUATIC or swims() or MF_NO_BREATHE or flies()
        auto can_climb() const -> bool;         // climbs() or flies()
        auto digging() const -> bool override;  // digs() or can_dig() and diggable terrain
        auto can_dig() const -> bool;
        auto digs() const -> bool;
        auto flies() const -> bool;
        auto climbs() const -> bool;
        auto swims() const -> bool;
        // Returns false if the monster is stunned, has 0 moves or otherwise wouldn't act this turn
        auto can_act() const -> bool;
        auto sight_range( int light_level ) const -> int override;
        auto made_of( const material_id &m ) const -> bool override; // Returns true if it's made of m
        auto made_of_any( const std::set<material_id> &ms ) const -> bool override;
        auto made_of( phase_id p ) const -> bool; // Returns true if its phase is p

        auto avoid_trap( const tripoint &pos, const trap &tr ) const -> bool override;

        void serialize( JsonOut &json ) const;
        void deserialize( JsonIn &jsin );

        auto move_target() -> tripoint; // Returns point at the end of the monster's current plans
        auto attack_target() -> Creature *; // Returns the creature at the end of plans (if hostile)

        // Movement
        void shift( const point &sm_shift ); // Shifts the monster to the appropriate submap
        void set_goal( const tripoint &p );
        // Updates current pos AND our plans
        auto wander() -> bool; // Returns true if we have no plans

        /**
         * Checks whether we can move to/through p. This does not account for bashing.
         *
         * This is used in pathfinding and ONLY checks the terrain. It ignores players
         * and monsters, which might only block this tile temporarily.
         * will_move_to() checks for impassable terrain etc
         * can_reach_to() checks for z-level difference.
         * can_move_to() is a wrapper for both of them.
         * can_squeeze_to() checks for vehicle holes.
         */
        auto can_move_to( const tripoint &p ) const -> bool;
        auto can_reach_to( const tripoint &p ) const -> bool;
        auto will_move_to( const tripoint &p ) const -> bool;
        auto can_squeeze_to( const tripoint &p ) const -> bool;

        auto will_reach( const point &p ) -> bool; // Do we have plans to get to (x, y)?
        auto  turns_to_reach( const point &p ) -> int; // How long will it take?

        // Go in a straight line to p
        void set_dest( const tripoint &p );
        // Reset our plans, we've become aimless.
        void unset_dest();

        /**
         * Set p as wander destination.
         *
         * This will cause the monster to slowly move towards the destination,
         * unless there is an overriding smell or plan.
         *
         * @param p Destination of monster's wanderings
         * @param f The priority of the destination, as well as how long we should
         *          wander towards there.
         */
        void wander_to( const tripoint &p, int f ); // Try to get to (x, y), we don't know
        // the route.  Give up after f steps.

        // How good of a target is given creature (checks for visibility)
        auto rate_target( Creature &c, float best, bool smart = false ) const -> float;
        void plan();
        void move(); // Actual movement
        void footsteps( const tripoint &p ); // noise made by movement
        void shove_vehicle( const tripoint &remote_destination,
                            const tripoint &nearby_destination ); // shove vehicles out of the way

        // check if the given square could drown a drownable monster
        auto is_aquatic_danger( const tripoint &at_pos ) -> bool;

        // check if a monster at a position will drown and kill it if necessary
        // returns true if the monster dies
        // chance is the one_in( chance ) that the monster will drown
        auto die_if_drowning( const tripoint &at_pos, int chance = 1 ) -> bool;

        auto scent_move() -> tripoint;
        auto calc_movecost( const tripoint &f, const tripoint &t ) const -> int;
        auto calc_climb_cost( const tripoint &f, const tripoint &t ) const -> int;

        auto is_immune_field( const field_type_id &fid ) const -> bool override;

        /**
         * Attempt to move to p.
         *
         * If there's something blocking the movement, such as infinite move
         * costs at the target, an existing NPC or monster, this function simply
         * aborts and does nothing.
         *
         * @param p Destination of movement
         * @param force If this is set to true, the movement will happen even if
         *              there's currently something, else than a creature, blocking the destination.
         * @param step_on_critter If this is set to true, the movement will happen even if
         *              there's currently a creature blocking the destination.
         *
         * @param stagger_adjustment is a multiplier for move cost to compensate for staggering.
         *
         * @return true if movement successful, false otherwise
         */
        auto move_to( const tripoint &p, bool force = false, bool step_on_critter = false,
                      float stagger_adjustment = 1.0 ) -> bool;

        /**
         * Attack any enemies at the given location.
         *
         * Attacks only if there is a creature at the given location towards
         * we are hostile.
         *
         * @return true if something was attacked, false otherwise
         */
        auto attack_at( const tripoint &p ) -> bool;

        /**
         * Try to smash/bash/destroy your way through the terrain at p.
         *
         * @return true if we destroyed something, false otherwise.
         */
        auto bash_at( const tripoint &p ) -> bool;

        /**
         * Try to push away whatever occupies p, then step in.
         * May recurse and try to make the creature at p push further.
         *
         * @param p Location of pushed object
         * @param boost A bonus on the roll to represent a horde pushing from behind
         * @param depth Number of recursions so far
         *
         * @return True if we managed to push something and took its place, false otherwise.
         */
        auto push_to( const tripoint &p, int boost, size_t depth ) -> bool;

        /** Returns innate monster bash skill, without calculating additional from helpers */
        auto bash_skill() -> int;
        auto bash_estimate() -> int;
        /** Returns ability of monster and any cooperative helpers to
         * bash the designated target.  **/
        auto group_bash_skill( const tripoint &target ) -> int;

        void stumble();
        void knock_back_to( const tripoint &to ) override;

        // Combat
        auto is_fleeing( player &u ) const -> bool; // True if we're fleeing
        auto attitude( const Character *u = nullptr ) const -> monster_attitude; // See the enum above
        auto attitude_to( const Creature &other ) const -> Attitude override;
        void process_triggers(); // Process things that anger/scare us

        auto is_underwater() const -> bool override;
        auto is_on_ground() const -> bool override;
        auto is_warm() const -> bool override;
        auto in_species( const species_id &spec ) const -> bool override;

        auto has_weapon() const -> bool override;
        auto is_dead_state() const -> bool override; // check if we should be dead or not
        auto is_elec_immune() const -> bool override;
        auto is_immune_effect( const efftype_id & ) const -> bool override;
        auto is_immune_damage( damage_type ) const -> bool override;

        void absorb_hit( const bodypart_id &bp, damage_instance &dam ) override;
        auto block_hit( Creature *source, bodypart_id &bp_hit, damage_instance &d ) -> bool override;
        void melee_attack( Creature &target );
        void melee_attack( Creature &target, float accuracy );
        void melee_attack( Creature &p, bool ) = delete;
        void deal_projectile_attack( Creature *source, dealt_projectile_attack &attack ) override;
        void deal_damage_handle_type( const damage_unit &du, bodypart_id bp, int &damage,
                                      int &pain ) override;
        void apply_damage( Creature *source, bodypart_id bp, int dam,
                           bool bypass_med = false ) override;
        // create gibs/meat chunks/blood etc all over the place, does not kill, can be called on a dead monster.
        void explode();
        // Let the monster die and let its body explode into gibs
        void die_in_explosion( Creature *source );
        /**
         * Flat addition to the monsters @ref hp. If `overheal` is true, this is not capped by max hp.
         * Returns actually healed hp.
         */
        auto heal( int delta_hp, bool overheal = false ) -> int;
        /**
         * Directly set the current @ref hp of the monster (not capped at the maximal hp).
         * You might want to use @ref heal / @ref apply_damage or @ref deal_damage instead.
         */
        void set_hp( int hp );

        /** Processes monster-specific effects before calling Creature::process_effects(). */
        void process_effects_internal() override;

        /** Returns true if the monster has its movement impaired */
        auto movement_impaired() -> bool;
        /** Processes effects which may prevent the monster from moving (bear traps, crushed, etc.).
         *  Returns false if movement is stopped. */
        auto move_effects( bool attacking ) -> bool override;
        /** Performs any monster-specific modifications to the arguments before passing to Creature::add_effect(). */
        void add_effect( const efftype_id &eff_id, const time_duration &dur, const bodypart_str_id &bp,
                         int intensity = 0, bool force = false, bool deferred = false ) override;
        void add_effect( const efftype_id &eff_id, const time_duration &dur, body_part bp = num_bp,
                         int intensity = 0, bool force = false, bool deferred = false );
        /** Returns a std::string containing effects for descriptions */
        auto get_effect_status() const -> std::string;

        auto power_rating() const -> float override;
        auto speed_rating() const -> float override;

        auto get_worn_armor_val( damage_type dt ) const -> int;
        auto  get_armor_cut( bodypart_id bp ) const -> int override; // Natural armor, plus any worn armor
        auto  get_armor_bash( bodypart_id bp ) const -> int override; // Natural armor, plus any worn armor
        auto  get_armor_bullet( bodypart_id bp ) const -> int override; // Natural armor, plus any worn armor
        auto  get_armor_type( damage_type dt, bodypart_id bp ) const -> int override;

        auto get_hit_base() const -> float override;
        auto get_dodge_base() const -> float override;

        auto  get_dodge() const -> float override;       // Natural dodge, or 0 if we're occupied
        auto  get_melee() const -> float override; // For determining attack skill when awarding dodge practice.
        auto  hit_roll() const -> float override;  // For the purposes of comparing to player::dodge_roll()
        auto  dodge_roll() -> float override;  // For the purposes of comparing to player::hit_roll()

        auto get_grab_strength() const -> int; // intensity of grabbed effect

        auto get_horde_attraction() -> monster_horde_attraction;
        void set_horde_attraction( monster_horde_attraction mha );
        auto will_join_horde( int size ) -> bool;

        /** Returns multiplier on fall damage at low velocity (knockback/pit/1 z-level, not 5 z-levels) */
        auto fall_damage_mod() const -> float override;
        /** Deals falling/collision damage with terrain/creature at pos */
        auto impact( int force, const tripoint &pos ) -> int override;

        auto has_grab_break_tec() const -> bool override;

        auto stability_roll() const -> float override;
        // We just dodged an attack from something
        void on_dodge( Creature *source, float difficulty ) override;
        // Something hit us (possibly null source)
        void on_hit( Creature *source, bodypart_id bp_hit,
                     float difficulty = INT_MIN, dealt_projectile_attack const *proj = nullptr ) override;
        void on_damage_of_type( int amt, damage_type dt, const bodypart_id &bp ) override;

        /** Resets a given special to its monster type cooldown value */
        void reset_special( const std::string &special_name );
        /** Resets a given special to a value between 0 and its monster type cooldown value. */
        void reset_special_rng( const std::string &special_name );
        /** Sets a given special to the given value */
        void set_special( const std::string &special_name, int time );
        /** Sets the enabled flag for the given special to false */
        void disable_special( const std::string &special_name );
        /** Return the lowest cooldown for an enabled special */
        auto shortest_special_cooldown() const -> int;

        void process_turn() override;
        /** Resets the value of all bonus fields to 0, clears special effect flags. */
        void reset_bonuses() override;
        /** Resets stats, and applies effects in an idempotent manner */
        void reset_stats() override;

        void die( Creature *killer ) override; //this is the die from Creature, it calls kill_mo
        void drop_items_on_death();

        // Other
        /**
         * Makes this monster into a fungus version
         * Returns false if no such monster exists
         * Returns true if monster is immune to spores, or if it has been fungalized
         */
        auto make_fungus() -> bool;
        void make_friendly();
        /** Makes this monster an ally of the given monster. */
        void make_ally( const monster &z );
        // Add an item to inventory
        void add_item( const item &it );
        // check mech power levels and modify it.
        auto use_mech_power( int amt ) -> bool;
        auto check_mech_powered() const -> bool;
        auto mech_str_addition() const -> int;

        void process_items();

        /**
         * Makes monster react to heard sound
         *
         * @param source Location of the sound source
         * @param vol Volume at the center of the sound source
         * @param distance Distance to sound source (currently just rl_dist)
         */
        void hear_sound( const tripoint &source, int vol, int distance );

        auto is_hallucination() const -> bool override;    // true if the monster isn't actually real

        auto bloodType() const -> field_type_id override;
        auto gibType() const -> field_type_id override;

        using Creature::add_msg_if_npc;
        void add_msg_if_npc( const std::string &msg ) const override;
        void add_msg_if_npc( const game_message_params &params, const std::string &msg ) const override;
        using Creature::add_msg_player_or_npc;
        void add_msg_player_or_npc( const std::string &player_msg,
                                    const std::string &npc_msg ) const override;
        void add_msg_player_or_npc( const game_message_params &params, const std::string &player_msg,
                                    const std::string &npc_msg ) const override;
        // TEMP VALUES
        tripoint wander_pos; // Wander destination - Just try to move in that direction
        int wandf;           // Urge to wander - Increased by sound, decrements each move
        std::vector<item> inv; // Inventory
        std::vector<item> corpse_components; // Hack to make bionic corpses generate CBMs on death
        Character *mounted_player = nullptr; // player that is mounting this creature
        character_id mounted_player_id; // id of player that is mounting this creature ( for save/load )
        character_id dragged_foe_id; // id of character being dragged by the monster
        cata::value_ptr<item> tied_item; // item used to tie the monster
        cata::value_ptr<item> tack_item; // item representing saddle and reins and such
        cata::value_ptr<item> armor_item; // item of armor the monster may be wearing
        cata::value_ptr<item> storage_item; // storage item for monster carrying items
        cata::value_ptr<item> battery_item; // item to power mechs
        auto get_carried_weight() -> units::mass;
        auto get_carried_volume() -> units::volume;
        void move_special_item_to_inv( cata::value_ptr<item> &it );

        // DEFINING VALUES
        int friendly;
        int anger = 0;
        int morale = 0;
        // Our faction (species, for most monsters)
        mfaction_id faction;
        // If we're related to a mission
        int mission_id;
        const mtype *type;
        // If true, don't spawn loot items as part of death.
        bool no_extra_death_drops;
        // If true, monster dies quietly and leaves no corpse.
        bool no_corpse_quiet = false;
        // Turned to false for simulating monsters during distant missions so they don't drop in sight.
        bool death_drops = true;
        auto is_dead() const -> bool;
        bool made_footstep;
        // If we're unique
        std::string unique_name;
        bool hallucination;
        // abstract for a fish monster representing a hidden stock of population in that area.
        int fish_population = 1;

        void setpos( const tripoint &p ) override;
        auto pos() const -> const tripoint & override;
        inline auto posx() const -> int override {
            return position.x;
        }
        inline auto posy() const -> int override {
            return position.y;
        }
        inline auto posz() const -> int override {
            return position.z;
        }

        short ignoring;
        cata::optional<time_point> lastseen_turn;

        // Stair data.
        int staircount;

        // Ammunition if we use a gun.
        std::map<itype_id, int> ammo;

        /**
         * Convert this monster into an item (see @ref mtype::revert_to_itype).
         * Only useful for robots and the like, the monster must have at least
         * a non-empty item id as revert_to_itype.
         */
        auto to_item() const -> item;
        /**
         * Initialize values like speed / hp from data of an item.
         * This applies to robotic monsters that are spawned by invoking an item (e.g. turret),
         * and to reviving monsters that spawn from a corpse.
         */
        void init_from_item( const item &itm );

        time_point last_updated = calendar::turn_zero;
        /**
         * Do some cleanup and caching as monster is being unloaded from map.
         */
        void on_unload();
        /**
         * Retroactively update monster.
         * Call this after a preexisting monster has been placed on map.
         * Don't call for monsters that have been freshly created, it may cause
         * the monster to upgrade itself into another monster type.
         */
        void on_load();

        auto get_pathfinding_settings() const -> const pathfinding_settings & override;
        auto get_path_avoid() const -> std::set<tripoint> override;
        // summoned monsters via spells
        void set_summon_time( const time_duration &length );
        // handles removing the monster if the timer runs out
        void decrement_summon_timer();
    private:
        void process_trigger( mon_trigger trig, int amount );
        void process_trigger( mon_trigger trig, const std::function<int()> &amount_func );

    private:
        int hp;
        std::map<std::string, mon_special_attack> special_attacks;
        tripoint goal;
        tripoint position;
        bool dead;
        /** Legacy loading logic for monsters that are packing ammo. **/
        void normalize_ammo( int old_ammo );
        /** Normal upgrades **/
        auto next_upgrade_time() -> int;
        bool upgrades;
        int upgrade_time;
        bool reproduces;
        cata::optional<time_point> baby_timer;
        time_point udder_timer;
        monster_horde_attraction horde_attraction;
        /** Found path. Note: Not used by monsters that don't pathfind! **/
        std::vector<tripoint> path;
        std::bitset<NUM_MEFF> effect_cache;
        cata::optional<time_duration> summon_time_limit = cata::nullopt;

        auto find_dragged_foe() -> player *;
        void nursebot_operate( player *dragged_foe );

    protected:
        void store( JsonOut &json ) const;
        void load( const JsonObject &data );

        /** Processes monster-specific effects of an effect. */
        void process_one_effect( effect &it, bool is_new ) override;
};

#endif // CATA_SRC_MONSTER_H
