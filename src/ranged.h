#ifndef CATA_SRC_RANGED_H
#define CATA_SRC_RANGED_H

#include <map>
#include <vector>

#include "type_id.h"

class aim_activity_actor;
class avatar;
class Character;
class Creature;
class gun_mode;
class item;
class map;
class player;
class spell;
class turret_data;
class vehicle;
class shape;
class shape_factory;
struct itype;
struct tripoint;
struct projectile;
struct vehicle_part;
struct dealt_damage_instance;
struct damage_instance;

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

/** Executing an AoE attack given by shape. */
trajectory mode_shaped( avatar &you, shape_factory &shape_fac, aim_activity_actor &activity );

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

/** How much dispersion does one point of target's dodge add when throwing at said target? */
int throw_dispersion_per_dodge( const Character &c, bool add_encumbrance );
/** Dispersion of a thrown item, against a given target, taking into account whether or not the throw was blind. */
int throwing_dispersion( const Character &c, const item &to_throw, Creature *critter,
                         bool is_blind_throw );
int throw_cost( const player &c, const item &to_throw );

/** Penalties potentially incurred by STR_DRAW weapons */
float get_str_draw_penalty( const item &it, const Character &p );
float str_draw_damage_modifier( const item &it, const Character &p );
float str_draw_dispersion_modifier( const item &it, const Character &p );
float str_draw_range_modifier( const item &it, const Character &p );

/** AoE attack, with area given by shape */
void execute_shaped_attack( const shape &sh, const projectile &proj, Creature &attacker );

std::map<tripoint, double> expected_coverage( const shape &sh, const map &here, int bash_power );

dealt_damage_instance hit_with_aoe( Creature &target, Creature *source, const damage_instance &di );

void draw_cone_aoe( const tripoint &origin, const std::map<tripoint, double> &aoe );

enum class hit_tier : int {
    grazing = 0,
    normal,
    critical
};

void print_dmg_msg( Creature &target, Creature *source, const dealt_damage_instance &dealt_dam,
                    hit_tier ht = hit_tier::normal );

/**
 * Prompts to select default ammo compatible with provided gun.
 */
void prompt_select_default_ammo_for( avatar &u, const item &w );

} // namespace ranged

#endif // CATA_SRC_RANGED_H
