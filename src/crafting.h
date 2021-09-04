#pragma once
#ifndef CATA_SRC_CRAFTING_H
#define CATA_SRC_CRAFTING_H

#include <list>
#include <set>
#include <vector>
#include "point.h"

class Character;
class inventory;
class item;
class player;
class recipe;
struct tool_comp;

using itype_id = std::string;

enum class cost_adjustment : int;

enum class bench_type : int {
    ground = 0,
    hands,
    furniture,
    vehicle
};

struct bench_location {
    explicit bench_location( bench_type type, tripoint position )
        : type( type ), position( position )
    {}
    bench_type type;
    tripoint position;
};

template<typename Type>
struct comp_selection;

// removes any (removable) ammo from the item and stores it in the
// players inventory.
void remove_ammo( item &dis_item, player &p );
// same as above but for each item in the list
void remove_ammo( std::list<item> &dis_items, player &p );

bench_location find_best_bench( const player &p, const item &craft );

float workbench_crafting_speed_multiplier( const item &craft, const bench_location &bench );
float crafting_speed_multiplier( const player &p, const recipe &rec, bool in_progress );
float crafting_speed_multiplier( const player &p, const item &craft, const bench_location &bench );
void complete_craft( player &p, item &craft, const bench_location &bench );

namespace crafting
{

/**
 * Returns the set of book types in crafting_inv that provide the
 * given recipe.
 * @param c Character whose skills are used to limit the available recipes
 * @param crafting_inv Current available items that may contain readable books
 * @param r Recipe to search for in the available books
 */
std::set<itype_id> get_books_for_recipe( const Character &c, const inventory &crafting_inv,
        const recipe *r );

/**
 * Returns the set of book types that provide the given recipe.
 */
std::set<itype_id> get_books_for_recipe( const recipe *r );

int charges_for_complete( int full_charges );
int charges_for_starting( int full_charges );
int charges_for_continuing( int full_charges );

/**
 * Returns selected tool component that matches one of the expected ones.
 * @param tools tools to match
 * @param batch size of batch to craft, multiplier on expected charges
 * @param map_inv map inventory to select from
 * @param player_with_inv if not null, character who provides additional inventory
 * @param hotkeys hotkeys available to the menu
 * @param can_cancel can the selection be aborted with no result
 * @param pick_first select like an npc - pick first result matched, preferring ones on player
 * @param adjustment affects required charges, see @ref cost_adjustment
 */
comp_selection<tool_comp>
select_tool_component( const std::vector<tool_comp> &tools, int batch, const inventory &map_inv,
                       const Character *player_with_inv,
                       bool can_cancel,
                       const std::string &hotkeys,
                       cost_adjustment adjustment );

comp_selection<tool_comp>
select_tool_component( const std::vector<tool_comp> &tools, int batch, const inventory &map_inv,
                       const Character *player_with_inv,
                       bool can_cancel = false );

} // namespace crafting

#endif // CATA_SRC_CRAFTING_H
