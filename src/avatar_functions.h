#pragma once
#ifndef CATA_SRC_AVATAR_FUNCTIONS_H
#define CATA_SRC_AVATAR_FUNCTIONS_H

#include "calendar.h"

class avatar;
class npc;
class item_location;

namespace avatar_funcs
{

/** Handles sleep attempts by the player, starts ACT_TRY_SLEEP activity */
void try_to_sleep( avatar &you, const time_duration &dur = 30_minutes );

/**
 * Try to disarm the NPC. May result in fail attempt, you receiving the wepon and instantly wielding it,
 * or the weapon falling down on the floor nearby. NPC is always getting angry with you.
 * @param target Target NPC to disarm
 */
void try_disarm_npc( avatar &you, npc &target );

/**
 * Try to steal an item from the NPC's inventory. May result in fail attempt, when NPC not notices you,
 * notices your steal attempt and getting angry with you, and you successfully stealing the item.
 * @param target Target NPC to steal from
 */
void try_steal_from_npc( avatar &you, npc &target );

/**
 * Attempt to start an activity to mend an item (fix any current faults)
 * @param obj Object to mend
 * @param interactive If true prompts player when there are multiple faults, otherwise mends the first
 */
void mend_item( avatar &you, item_location &&obj, bool interactive = true );

} // namespace avatar_funcs

#endif // CATA_SRC_AVATAR_FUNCTIONS_H
