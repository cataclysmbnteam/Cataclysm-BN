#pragma once

#include <set>
#include <vector>

#include "type_id.h"

class Character;
class inventory;
class item;
class material_type;
class JsonIn;

struct tripoint;

void populate_salvage_materials( quality &q );

namespace salvage
{

using quality_cache = std::map<quality_id, std::map<int, int>>;

enum class q_result {
    yes,
    ignore,
    abort,
    skip,
    fail
};

ret_val<bool> try_salvage( const item &, quality_cache & );

q_result promt_warnings( const Character &who, const item &it, quality_cache & );

std::set<material_id> &get_all_salvagable_materials();

std::unordered_map<material_id, std::set<quality_id>> &get_salvage_material_quality_dictionary();

units::mass minimal_weight_to_cut( const item &it );

std::vector<std::pair< itype_id, float>> salvage_results( const item &target );

void complete_salvage( Character &who, item &cut, tripoint_abs_ms pos );

int moves_to_salvage( const item &target );

bool has_salvage_tools( quality_cache &, const material_id &material );
bool has_salvage_tools( quality_cache &, const item &item, bool strict = false );
bool menu_salvage_single( player &you );
bool prompt_salvage_single( Character &who, item &target );
bool salvage_single( Character &who, item &target );
bool salvage_all( Character &who );

};


