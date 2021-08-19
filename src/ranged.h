#ifndef CATA_SRC_RANGED_H
#define CATA_SRC_RANGED_H

#include <vector>

#include "memory_fast.h"
#include "type_id.h"
#include "units.h"

class aim_activity_actor;
class avatar;
class map;
class JsonIn;
class JsonOut;
class Creature;
class Character;
class item;
class player;
class spell;
class turret_data;
class vehicle;
struct itype;
struct tripoint;
struct vehicle_part;
template<typename T> struct enum_traits;

enum target_mode : int {
    TARGET_MODE_FIRE,
    TARGET_MODE_THROW,
    TARGET_MODE_TURRET,
    TARGET_MODE_TURRET_MANUAL,
    TARGET_MODE_REACH,
    TARGET_MODE_THROW_BLIND,
    TARGET_MODE_SPELL
};

/**
 * Specifies weapon source for aiming across turns and
 * (de-)serialization of targeting_data
 */
enum weapon_source_enum {
    /** Invalid weapon source */
    WEAPON_SOURCE_INVALID,
    /** Firing wielded weapon */
    WEAPON_SOURCE_WIELDED,
    /** Firing fake gun provided by a bionic */
    WEAPON_SOURCE_BIONIC,
    /** Firing fake gun provided by a mutation */
    WEAPON_SOURCE_MUTATION,
    NUM_WEAPON_SOURCES
};

template <>
struct enum_traits<weapon_source_enum> {
    static constexpr weapon_source_enum last = NUM_WEAPON_SOURCES;
};

/** Stores data for aiming the player's weapon across turns */
struct targeting_data {
    weapon_source_enum weapon_source;

    /** Cached fake weapon provided by bionic/mutation */
    shared_ptr_fast<item> cached_fake_weapon;

    /** Bionic power cost per shot */
    units::energy bp_cost_per_shot;

    bool is_valid() const;

    /** Use wielded gun */
    static targeting_data use_wielded();

    /** Use fake gun provided by a bionic */
    static targeting_data use_bionic( const item &fake_gun, const units::energy &cost_per_shot );

    /** Use fake gun provided by a mutation */
    static targeting_data use_mutation( const item &fake_gun );

    // Since only avatar uses targeting_data,
    // (de-)serialization is implemented in savegame_json.cpp
    // near avatar (de-)serialization
    void serialize( JsonOut &json ) const;
    void deserialize( JsonIn &jsin );
};

class target_handler_old
{
        // TODO: alias return type of target_ui
    public:
        /**
         *  Prompts for target and returns trajectory to it.
         *  TODO: pass arguments via constructor(s) and add methods for getting names and button labels,
         *        switching ammo & firing modes, drawing - stuff like that
         *  @param pc The player doing the targeting
         *  @param mode targeting mode, which affects UI display among other things.
         *  @param relevant active item, if any (for instance, a weapon to be aimed).
         *  @param range the maximum distance to which we're allowed to draw a target.
         *  @param ammo effective ammo data (derived from @param relevant if unspecified).
         *  @param turret turret being fired (relevant for TARGET_MODE_TURRET_MANUAL)
         *  @param veh vehicle that turrets belong to (relevant for TARGET_MODE_TURRET)
         *  @param vturrets vehicle turrets being aimed (relevant for TARGET_MODE_TURRET)
         */
        std::vector<tripoint> target_ui( player &pc, target_mode mode,
                                         item *relevant, int range,
                                         const itype *ammo = nullptr,
                                         turret_data *turret = nullptr,
                                         vehicle *veh = nullptr,
                                         const std::vector<vehicle_part *> &vturrets = std::vector<vehicle_part *>()
                                       );
        // magic version of target_ui
        std::vector<tripoint> target_ui( spell_id sp, bool no_fail = false,
                                         bool no_mana = false );
        std::vector<tripoint> target_ui( spell &casting, bool no_fail = false,
                                         bool no_mana = false );
};

namespace target_handler
{
// Trajectory to target. Empty if selection was aborted or player ran out of moves
using trajectory = std::vector<tripoint>;

/**
 * Firing ranged weapon. This mode allows spending moves on aiming.
 */
trajectory mode_fire( avatar &you, aim_activity_actor &activity );

/** Throwing item */
trajectory mode_throw( avatar &you, item &relevant, bool blind_throwing );

/** Reach attacking */
trajectory mode_reach( avatar &you, item &weapon );

/** Manually firing vehicle turret */
trajectory mode_turret_manual( avatar &you, turret_data &turret );

/** Selecting target for turrets (when using vehicle controls) */
trajectory mode_turrets( avatar &you, vehicle &veh, const std::vector<vehicle_part *> &turrets );

/** Casting a spell */
trajectory mode_spell( avatar &you, spell &casting, bool no_fail, bool no_mana );
} // namespace target_handler

int range_with_even_chance_of_good_hit( int dispersion );

namespace ranged
{

/**
 * Common checks for gunmode (when firing a weapon / manually firing turret)
 * @param messages Used to store messages describing failed checks
 * @return True if all conditions are true
 */
bool gunmode_checks_common( avatar &you, const map &m, std::vector<std::string> &messages,
                            const gun_mode &gmode );

/**
 * Various checks for gunmode when firing a weapon
 * @param messages Used to store messages describing failed checks
 * @return True if all conditions are true
 */
bool gunmode_checks_weapon( avatar &you, const map &m, std::vector<std::string> &messages,
                            const gun_mode &gmode );

std::vector<Creature *> targetable_creatures( const Character &c, int range );
std::vector<Creature *> targetable_creatures( const Character &c, int range,
        const turret_data &turret );

int burst_penalty( const Character &p, const item &gun, int gun_recoil );

} // namespace ranged

#endif // CATA_SRC_RANGED_H
