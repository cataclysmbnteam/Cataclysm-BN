#pragma once
#ifndef CATA_SRC_AVATAR_FUNCTIONS_H
#define CATA_SRC_AVATAR_FUNCTIONS_H

#include "calendar.h"

class avatar;

namespace avatar_funcs
{

/** Handles sleep attempts by the player, starts ACT_TRY_SLEEP activity */
void try_to_sleep( avatar &you, const time_duration &dur = 30_minutes );

} // namespace avatar_funcs

#endif // CATA_SRC_AVATAR_FUNCTIONS_H
