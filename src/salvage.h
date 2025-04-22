#pragma once

#include <set>
#include <vector>

#include "type_id.h"

class Character;
class inventory;
class item;
class material_type;
class tripoint;
class JsonIn;

void populate_salvage_materials( quality &q );

namespace salvage
{
enum class q_result {
    yes,
    ignore,
    abort,
    skip,
    fail
};

bool try_salvage_silent( const Character &who, const item &it, inventory inv );
q_result try_salvage( const Character &who, const item &it, inventory inv, bool mute,
                      bool mute_promts );

units::mass minimal_weight_to_cut( const item &it );

std::vector<std::pair< itype_id, float>> salvage_results( const item &target );

void complete_salvage( Character &who, item &cut, tripoint_abs_ms pos );

int moves_to_salvage( const item &target );

bool has_salvage_tools( const inventory &inv, const material_id &material );
bool has_salvage_tools( inventory &inv, const item &item, bool strict = false );
bool menu_salvage_single( player &you );
bool prompt_salvage_single( Character &who, item &target );
bool salvage_single( Character &who, item &target );
bool salvage_all( Character &who );

};


