#pragma once
#ifndef CATA_SRC_GAME_INVENTORY_H
#define CATA_SRC_GAME_INVENTORY_H

#include <functional>
#include <list>
#include <string>
#include <utility>

#include "item_handling_util.h"
#include "item_location.h"

struct tripoint;

namespace cata
{
template<typename T>
class optional;
} // namespace cata
class avatar;
class item;
class player;
class repair_item_actor;
class salvage_actor;

using item_filter = std::function<bool( const item & )>;

namespace game_menus
{

namespace inv
{
// item selector for all items in @you's inventory.
auto titled_menu( avatar &you, const std::string &title,
                           const std::string &none_message = "" ) -> item_location;
// item selector for items in @you's inventory with a filter
auto titled_filter_menu( item_filter filter, avatar &you,
                                  const std::string &title, const std::string &none_message = "" ) -> item_location;

/**
* @name Customized inventory menus
*
* The functions here execute customized inventory menus for specific game situations.
* Each menu displays only related inventory (or nearby) items along with context dependent information.
* More functions will follow. TODO: update all 'inv_for...()' functions to return @ref item_location instead of
* plain int and move them here.
* @return Either location of the selected item or null location if none was selected.
*/
/*@{*/

void common( avatar &you );
void compare( player &p, const cata::optional<tripoint> &offset );
void compare( const item &left, const item &right );
void reassign_letter( player &p, item &it );
void swap_letters( player &p );

/**
 * Select items to drop.
 * @return A list of pairs of item_location, quantity.
 */
auto multidrop( player &p ) -> drop_locations;

/**
 * Select items to wash.
 * @param water Available water
 * @param cleanser Available cleanser
 * @param do_soft Whether to allow soft items
 * @param do_hard Whether to allow hard items
 * @return A list of selected item_locations with quantities.
 */
auto multiwash( Character &ch, int water, int cleanser, bool do_soft, bool do_hard ) -> iuse_locations;

/** Consuming an item. */
auto consume( player &p ) -> item_location;
/** Consuming a food item via a custom menu. */
auto consume_food( player &p ) -> item_location;
/** Consuming a drink item via a custom menu. */
auto consume_drink( player &p ) -> item_location;
/** Consuming a medication item via a custom menu. */
auto consume_meds( player &p ) -> item_location;
/** Choosing a container for liquid. */
auto container_for( avatar &you, const item &liquid, int radius = 0 ) -> item_location;
/** Item disassembling menu. */
auto disassemble( player &p ) -> item_location;
/** Gunmod installation menu. */
auto gun_to_modify( player &p, const item &gunmod ) -> item_location;
/** Book reading menu. */
auto read( player &pl ) -> item_location;
/** Menu for stealing stuff. */
auto steal( avatar &you, player &victim ) -> item_location;
/** Item activation menu. */
auto use( avatar &you ) -> item_location;
/** Item wielding/unwielding menu. */
auto wield( avatar &you ) -> item_location;
/** Item wielding/unwielding menu. */
auto holster( player &p, item &holster ) -> item_location;
/** Choosing a gun to saw down it's barrel. */
auto saw_barrel( player &p, item &tool ) -> item_location;
/** Choose item to wear. */
auto wear( player &p ) -> item_location;
/** Choose item to take off. */
auto take_off( avatar &you ) -> item_location;
/** Item cut up menu. */
auto salvage( player &p, const salvage_actor *actor ) -> item_location;
/** Repair menu. */
auto repair( player &p, const repair_item_actor *actor, const item *main_tool ) -> item_location;
/** Bionic install menu. */
auto install_bionic( player &p, player &patient, bool surgeon = false ) -> item_location;
/** Bionic uninstall menu. */
auto uninstall_bionic( player &p, player &patient ) -> item_location;
/**Autoclave sterilize menu*/
auto sterilize_cbm( player &p ) -> item_location;
/*@}*/

} // namespace inv

} // namespace game_menus

#endif // CATA_SRC_GAME_INVENTORY_H
