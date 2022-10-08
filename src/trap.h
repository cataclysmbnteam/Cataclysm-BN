#pragma once
#ifndef CATA_SRC_TRAP_H
#define CATA_SRC_TRAP_H

#include <cstddef>
#include <functional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "color.h"
#include "magic.h"
#include "translations.h"
#include "type_id.h"
#include "units.h"

class Character;
class Creature;
class JsonObject;
class item;
class map;
struct tripoint;

namespace trapfunc
{
// p is the point where the trap is (not where the creature is)
// creature is the creature that triggered the trap,
// item is the item that triggered the trap,
// creature and item can be nullptr.
auto none( const tripoint &, Creature *, item * ) -> bool;
auto bubble( const tripoint &p, Creature *c, item *i ) -> bool;
auto glass( const tripoint &p, Creature *c, item *i ) -> bool;
auto cot( const tripoint &p, Creature *c, item *i ) -> bool;
auto beartrap( const tripoint &p, Creature *c, item *i ) -> bool;
auto snare_light( const tripoint &p, Creature *c, item *i ) -> bool;
auto snare_heavy( const tripoint &p, Creature *c, item *i ) -> bool;
auto board( const tripoint &p, Creature *c, item *i ) -> bool;
auto caltrops( const tripoint &p, Creature *c, item *i ) -> bool;
auto caltrops_glass( const tripoint &p, Creature *c, item *i ) -> bool;
auto tripwire( const tripoint &p, Creature *c, item *i ) -> bool;
auto crossbow( const tripoint &p, Creature *c, item *i ) -> bool;
auto shotgun( const tripoint &p, Creature *c, item *i ) -> bool;
auto blade( const tripoint &p, Creature *c, item *i ) -> bool;
auto landmine( const tripoint &p, Creature *c, item *i ) -> bool;
auto telepad( const tripoint &p, Creature *c, item *i ) -> bool;
auto goo( const tripoint &p, Creature *c, item *i ) -> bool;
auto dissector( const tripoint &p, Creature *c, item *i ) -> bool;
auto sinkhole( const tripoint &p, Creature *c, item *i ) -> bool;
auto pit( const tripoint &p, Creature *c, item *i ) -> bool;
auto pit_spikes( const tripoint &p, Creature *c, item *i ) -> bool;
auto pit_glass( const tripoint &p, Creature *c, item *i ) -> bool;
auto lava( const tripoint &p, Creature *c, item *i ) -> bool;
auto portal( const tripoint &p, Creature *c, item *i ) -> bool;
auto ledge( const tripoint &p, Creature *c, item *i ) -> bool;
auto boobytrap( const tripoint &p, Creature *c, item *i ) -> bool;
auto temple_flood( const tripoint &p, Creature *c, item *i ) -> bool;
auto temple_toggle( const tripoint &p, Creature *c, item *i ) -> bool;
auto glow( const tripoint &p, Creature *c, item *i ) -> bool;
auto hum( const tripoint &p, Creature *c, item *i ) -> bool;
auto shadow( const tripoint &p, Creature *c, item *i ) -> bool;
auto map_regen( const tripoint &p, Creature *c, item *i ) -> bool;
auto drain( const tripoint &p, Creature *c, item *i ) -> bool;
auto snake( const tripoint &p, Creature *c, item *i ) -> bool;
auto cast_spell( const tripoint &p, Creature *critter, item * ) -> bool;
} // namespace trapfunc

struct vehicle_handle_trap_data {
    bool remove_trap = false;
    bool do_explosion = false;
    bool is_falling = false;
    int chance = 100;
    int damage = 0;
    int shrapnel = 0;
    int sound_volume = 0;
    translation sound;
    std::string sound_type;
    std::string sound_variant;
    // the double represents the count or chance to spawn.
    std::vector<std::pair<itype_id, double>> spawn_items;
    trap_str_id set_trap = trap_str_id::NULL_ID();
};

using trap_function = std::function<bool( const tripoint &, Creature *, item * )>;

struct trap {
        trap_str_id id;
        trap_id loadid;

        bool was_loaded = false;

        int sym = 0;
        nc_color color;
    private:
        // 1 to ??, affects detection
        int visibility = 1;
        // 0 to ??, affects avoidance
        int avoidance = 0;
        // 0 to ??, difficulty of assembly & disassembly
        int difficulty = 0;
        // 0 to ??, trap radius
        int trap_radius = 0;
        bool benign = false;
        bool always_invisible = false;
        // a valid overmap id, for map_regen action traps
        std::string map_regen;
        trap_function act;
        std::string name_;
        /**
         * If an item with this weight or more is thrown onto the trap, it triggers.
         */
        units::mass trigger_weight = units::mass( -1, units::mass::unit_type{} );
        int funnel_radius_mm = 0;
        // For disassembly?
        std::vector<std::tuple<itype_id, int, int>> components;
    public:
        // data required for trapfunc::spell()
        fake_spell spell_data;
        int comfort = 0;
        int floor_bedding_warmth = 0;
    public:
        vehicle_handle_trap_data vehicle_data;
        auto name() const -> std::string;
        /**
         * There are special always invisible traps. See player::search_surroundings
         */
        auto is_always_invisible() const -> bool {
            return always_invisible;
        }
        /**
         * How easy it is to spot the trap. Smaller values means it's easier to spot.
         */
        auto get_visibility() const -> int {
            return visibility;
        }

        auto  map_regen_target() const -> std::string;

        /**
         * Whether triggering the trap can be avoid (if greater than 0) and if so, this is
         * compared to dodge skill (with some adjustments). Smaller values means it's easier
         * to dodge.
         */
        auto get_avoidance() const -> int {
            return avoidance;
        }
        /**
         * This is used when disarming the trap. A value of 0 means disarming will always work
         * (e.g. for funnels), a values of 99 means it can not be disarmed at all. Smaller values
         * makes it easier to disarm the trap.
         */
        auto get_difficulty() const -> int {
            return difficulty;
        }
        /**
         * If true, this is not really a trap and there won't be any safety queries before stepping
         * onto it (e.g. for funnels).
         */
        auto is_benign() const -> bool {
            return benign;
        }
        /** Player has not yet seen the trap and returns the variable chance, at this moment,
         of whether the trap is seen or not. */
        auto detect_trap( const tripoint &pos, const Character &p ) const -> bool;
        /**
         * Can player/npc p see this kind of trap, either by their memory (they known there is
         * the trap) or by the visibility of the trap (the trap is not hidden at all)?
         */
        auto can_see( const tripoint &pos, const Character &p ) const -> bool;
        /**
         * Trigger trap effects.
         * @param creature The creature that triggered the trap, it does not necessarily have to
         * be on the place of the trap (traps can be triggered from adjacent, e.g. when disarming
         * them). This can also be a null pointer if the trap has been triggered by some thrown
         * item (which must have the @ref trigger_weight).
         * @param pos The location of the trap in the main map.
         * @param item The item that triggered the trap
         */
        void trigger( const tripoint &pos, Creature *creature = nullptr, item *item = nullptr ) const;
        /**
         * If the given item is throw onto the trap, does it trigger the trap?
         */
        auto triggered_by_item( const item &itm ) const -> bool;
        /**
         * Called when a trap at the given point in the map has been disarmed.
         * It should spawn trap items (if any) and remove the trap from the map via
         * @ref map::remove_trap.
         */
        void on_disarmed( map &m, const tripoint &p ) const;
        /**
         * This is used when defining area this trap occupies. A value of 0 means trap occupies exactly 1 tile.
         */
        auto get_trap_radius() const -> int {
            return trap_radius;
        }
        /**
         * Whether this is the null-traps, aka no trap at all.
         */
        auto is_null() const -> bool;
        /**
         * Loads this specific trap.
         */
        void load( const JsonObject &jo, const std::string &src );

        /*@{*/
        /**
         * @name Funnels
         *
         * Traps can act as funnels, for this they need a @ref trap::funnel_radius_mm > 0.
         * Funnels are usual not hidden at all (@ref trap::visibility == 0), are @ref trap::benign and can
         * be picked up easily (@ref trap::difficulty == 0).
         * The funnel filling is handled in weather.cpp. is_funnel is used the check whether the
         * funnel specific code should be run for this trap.
         */
        auto is_funnel() const -> bool;
        auto funnel_turns_per_charge( double rain_depth_mm_per_hour ) const -> double;
        /**
         * Returns all trap objects that are actually funnels (is_funnel returns true for all
         * of them).
         */
        static auto get_funnels() -> const std::vector<const trap *> &;
        /*@}*/

        /*@{*/
        /**
         * @name Initialization
         *
         * Those functions are used by the @ref DynamicDataLoader, see there.
         */
        /**
         * Loads the trap and adds it to the trapmap, and the traplist.
         * @throw JsonError if the json is invalid as usual.
         */
        static void load_trap( const JsonObject &jo, const std::string &src );
        /**
         * Releases the loaded trap objects in trapmap and traplist.
         */
        static void reset();
        /**
         * Stores the actual @ref loadid of the loaded traps in the global tr_* variables.
         * It also sets the trap ids of the terrain types that have build-in traps.
         * Must be called after all traps have been loaded.
         */
        static void finalize();
        /**
         * Checks internal consistency (reference to other things like item ids etc.)
         */
        static void check_consistency();
        /*@}*/
        static auto count() -> size_t;
};

auto trap_function_from_string( const std::string &function_name ) -> const trap_function &;

extern trap_id
tr_null,
tr_bubblewrap,
tr_glass,
tr_cot,
tr_funnel,
tr_metal_funnel,
tr_makeshift_funnel,
tr_leather_funnel,
tr_rollmat,
tr_fur_rollmat,
tr_beartrap,
tr_beartrap_buried,
tr_nailboard,
tr_caltrops,
tr_caltrops_glass,
tr_tripwire,
tr_crossbow,
tr_shotgun_2,
tr_shotgun_2_1,
tr_shotgun_1,
tr_engine,
tr_blade,
tr_landmine,
tr_landmine_buried,
tr_telepad,
tr_goo,
tr_dissector,
tr_sinkhole,
tr_pit,
tr_spike_pit,
tr_glass_pit,
tr_lava,
tr_portal,
tr_ledge,
tr_boobytrap,
tr_temple_flood,
tr_temple_toggle,
tr_glow,
tr_hum,
tr_shadow,
tr_drain,
tr_snake;

#endif // CATA_SRC_TRAP_H
