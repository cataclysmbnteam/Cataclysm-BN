#pragma once
#ifndef CATA_SRC_ITEM_FUNCTIONS_H
#define CATA_SRC_ITEM_FUNCTIONS_H

class item;

namespace item_funcs
{

/** Returns whether given item (or one of its gunmods) can be unloaded. */
auto can_be_unloaded( const item &itm ) -> bool;

} // namespace item_funcs

#endif // CATA_SRC_ITEM_FUNCTIONS_H
