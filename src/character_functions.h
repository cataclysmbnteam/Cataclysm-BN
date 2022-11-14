#pragma once
#ifndef CATA_SRC_CHARACTER_FUNCTIONS_H
#define CATA_SRC_CHARACTER_FUNCTIONS_H

#include "point.h"
#include "type_id.h"

class Character;
class Creature;
class item;
class time_duration;
class vehicle;

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

} // namespace character_funcs

#endif // CATA_SRC_CHARACTER_FUNCTIONS_H
