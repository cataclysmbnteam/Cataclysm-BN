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

bool try_salvage_silent( Character &who, const item &it );
q_result try_salvage( Character &who, const item &it, bool mute, bool mute_promts );

units::mass minimal_weight_to_cut( const item &it );

std::vector<std::pair< material_id, float>> salvage_result_proportions( const item &target );

void complete_salvage( Character &who, item &cut, tripoint_abs_ms pos );

int moves_to_salvage( const item &target );

bool has_salvage_tools( const inventory &inv, const material_id &material );
bool has_salvage_tools( inventory &inv, const item &item, bool strict = false );
bool salvage_single( Character &, item & );
bool salvage_all( Character & );

};


