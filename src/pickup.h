#pragma once
#ifndef CATA_SRC_PICKUP_H
#define CATA_SRC_PICKUP_H

#include <vector>

class item;
class item_location;
class Character;
class JsonIn;
class JsonOut;
class map;
struct tripoint;

namespace pickup
{

struct pick_drop_selection;

/**
 * Returns `false` if the player was presented a prompt and decided to cancel the pickup.
 * `true` in other cases.
 */
auto do_pickup( std::vector<pick_drop_selection> &targets, bool autopickup ) -> bool;
auto query_thief() -> bool;

enum from_where : int {
    from_cargo = 0,
    from_ground,
    prompt
};

/** Pick up items; 'g' or ',' or via examine() */
void pick_up( const tripoint &p, int min, from_where get_items_from = prompt );
/** Determines the cost of moving an item by a character. */
auto cost_to_move_item( const Character &who, const item &it ) -> int;

/**
 * If character is handling a potentially spillable bucket, gracefully handle what
 * to do with the contents.
 *
 * Returns true if we handled the container, false if we chose to spill the
 * contents and the container still needs to be put somewhere.
 * @param c Character handling the spillable item
 * @param it item to handle
 * @param m map they are on
 */
auto handle_spillable_contents( Character &c, item &it, map &m ) -> bool;
} // namespace pickup

#endif // CATA_SRC_PICKUP_H
