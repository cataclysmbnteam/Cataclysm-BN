#pragma once
#ifndef CATA_SRC_ITEM_FUNCTIONS_H
#define CATA_SRC_ITEM_FUNCTIONS_H

class item;

namespace item_funcs
{

/** Returns whether given item (or one of its gunmods) can be unloaded. */
bool can_be_unloaded( const item &itm );

} // namespace item_funcs

#endif // CATA_SRC_ITEM_FUNCTIONS_H
