#pragma once
#ifndef CATA_SRC_CRAFTING_H
#define CATA_SRC_CRAFTING_H

#include <list>
#include <set>
#include "point.h"

class Character;
class inventory;
class item;
class player;
class recipe;

using itype_id = std::string;

enum class craft_flags : int {
    none = 0,
    start_only = 1, // Only require 5% (plus remainder) of tool charges
};

enum class bench_type : int {
    ground = 0,
    hands,
    furniture,
    vehicle
};

inline constexpr craft_flags operator&( craft_flags l, craft_flags r )
{
    return static_cast<craft_flags>( static_cast<unsigned>( l ) & static_cast<unsigned>( r ) );
}

struct bench_location {
    explicit bench_location( bench_type type, tripoint position )
        : type( type ), position( position )
    {}
    bench_type type;
    tripoint position;
};

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

} // namespace crafting

#endif // CATA_SRC_CRAFTING_H
