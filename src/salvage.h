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

bool try_salvage_silent( Character &who, const item &it );
bool try_salvage( Character &who, const item &it, bool mute, bool mute_promts );

std::set<material_id> can_salvage_materials( const item &it );

units::mass minimal_weight_to_cut( const item &it );

std::vector<std::pair< material_id, float>> salvage_result_proportions( const item &target );

void complete_salvage( Character &who, item &cut, tripoint pos );

int moves_to_salvage( const item &target );

bool has_salvage_tools( const inventory &inv, const material_id &material );
bool has_salvage_tools( const inventory &inv, const item &item, bool strict = false );
bool salvage_single( Character &, item & );
bool salvage_all( Character & );

};


