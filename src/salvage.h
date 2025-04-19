#pragma once

#include <set>
#include <vector>

#include "type_id.h"
#include "units.h"

class Character;
class inventory;
class item;
class material_type;
class tripoint;
class JsonIn;

void populate_salvage_materials( quality &q );

namespace salvage
{

bool valid_to_salvage( const item &it );

bool try_salvage( Character &who, item &it, bool mute = true );

std::set<material_id> can_salvage_materials( const item &it );

units::mass minimal_weight_to_cut( const item &it );

std::vector<std::pair< material_id, float>> get_salvagable_materials(
            const item &target );

void complete_salvage( Character &who, item &cut, tripoint pos );

int moves_to_salvage( const item &target );

bool has_salvage_tools( const inventory &inv, item &item, bool check_charges = false );
bool salvage_single( Character &, item & );
bool salvage_all( Character & );

};


