#pragma once
#ifndef CATA_SRC_ITEM_FUNCTIONS_H
#define CATA_SRC_ITEM_FUNCTIONS_H

#include "units.h"

class item;

namespace item_funcs
{

/** Returns whether given item (or one of its gunmods) can be unloaded. */
bool can_be_unloaded( const item &itm );

/** Number of shots left, considers if a gun uses ups, ammo, or both */
int shots_remaining( item it, units::energy power );

} // namespace item_funcs

#endif // CATA_SRC_ITEM_FUNCTIONS_H
