#pragma once
#ifndef CATA_SRC_CHARACTER_FUNCTIONS_H
#define CATA_SRC_CHARACTER_FUNCTIONS_H

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

} // namespace character_funcs

#endif // CATA_SRC_CHARACTER_FUNCTIONS_H
