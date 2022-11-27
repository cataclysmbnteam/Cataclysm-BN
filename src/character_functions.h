#pragma once
#ifndef CATA_SRC_CHARACTER_FUNCTIONS_H
#define CATA_SRC_CHARACTER_FUNCTIONS_H

#include "type_id.h"

#include <string>

enum body_part : int;
class Character;
class Creature;
class item;
class time_duration;
class vehicle;
struct tripoint;

namespace character_funcs
{

/** Estimate effect duration based on relevant skill */
time_duration estimate_effect_dur( int skill_lvl, const efftype_id &effect,
                                   const time_duration &error_magnitude, int threshold, const Creature &target );

/**
 * Siphons fuel (if available) from the specified vehicle into container or
 * similar via @ref game::handle_liquid. May start a character activity.
 */
void siphon( Character &ch, vehicle &veh, const itype_id &desired_liquid );

/** Returns enjoyability value of a book for given character. */
int get_book_fun_for( const Character &ch, const item &book );

/** Returns whether character considers it fun to read the book. */
bool is_fun_to_read( const Character &ch, const item &book );

/** Threshold for fine detail vision mod. */
constexpr float FINE_VISION_THRESHOLD = 4.0f;
/** Value for perfect fine detail vision. */
constexpr float FINE_VISION_PERFECT = 1.0f;

/**
 * @brief Calculates multiplier for the time taken to perform tasks that
 * require detail vision (reading, crafting, etc.).
 *
 * @param who Character to check for
 * @param pos Check remote spot as if the character were standing there
 *
 * @returns A value from 1.0 (unimpeded vision) to 11.0 (totally blind).
 *
 * Examples of produced values:
 *   1.0 is LIGHT_AMBIENT_LIT or brighter
 *   4.0 is a dark clear night, barely bright enough for reading and crafting
 *   6.0 is LIGHT_AMBIENT_DIM
 *   7.3 is LIGHT_AMBIENT_MINIMAL, a dark cloudy night, unlit indoors
 *   11.0 is zero light or blindness
 *
 * @{
 */
float fine_detail_vision_mod( const Character &who );
float fine_detail_vision_mod( const Character &who, const tripoint &p );
/** @} */

/**
 * @brief Checks whether character vision at given pos is good enough to see fine details.
 *
 * Fine vision is required for some interactions (reading, crafting, etc.)
 *
 * @param who Character to check for
 * @param pos Check remote spot as if the character were standing there
 *
 * @{
 */
bool can_see_fine_details( const Character &who );
bool can_see_fine_details( const Character &who, const tripoint &p );
/** @} */

enum class comfort_level {
    impossible = -999,
    uncomfortable = -7,
    neutral = 0,
    slightly_comfortable = 3,
    comfortable = 5,
    very_comfortable = 10
};

struct comfort_response_t {
    comfort_level level = comfort_level::neutral;
    const item *aid = nullptr;
};

/** Rate point's ability to serve as a bed. Only takes certain mutations into account, and not fatigue nor stimulants. */
comfort_response_t base_comfort_value( const Character &who, const tripoint &p );

/** Rate point's ability to serve as a bed. Takes all mutations, fatigue and stimulants into account. */
int rate_sleep_spot( const Character &who, const tripoint &p );

/** Checked each turn during "lying_down", returns true if the avatar falls asleep */
bool roll_can_sleep( Character &who );

/** Check whether character has an active bionic capable of interfacing with power armor. */
bool can_interface_armor( const Character &who );

/** Get the formatted name of the currently wielded item (if any) with current gun mode (if gun) */
std::string fmt_wielded_weapon( const Character &who );

/**
 * Add message describing how character feels pain.
 * @param who Character that feels the pain
 * @param val Amount of pain
 * @param bp Target body part, use num_bp if no specific body part.
 */
void add_pain_msg( const Character &who, int val, body_part bp );

} // namespace character_funcs

#endif // CATA_SRC_CHARACTER_FUNCTIONS_H
