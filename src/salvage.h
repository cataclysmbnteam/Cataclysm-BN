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

struct salvage_quality {
    public:
        bool was_loaded = false;
        quality_id id;

        /** Moves used per unit of volume of cut item */
        int moves_per_part;

        /** Materials it can cut */
        std::set<material_id> salvagable_materials;


        void load( const JsonObject &jo, const std::string &src );

        void finalize() const;

        static void reset();
        static void load_static( const JsonObject &jo, const std::string &src );
};

namespace salvage
{
static std::unordered_map<quality_id, salvage_quality> salvage_quality_dictionary;
static std::unordered_map<material_id, std::set<quality_id>> salvage_material_dictionary;
static std::set<material_id> all_salvagable_materials;

bool valid_to_salvage( const item &it );

bool try_salvage( Character &who, item &it, bool mute = true );

std::set<material_id> can_salvage_materials( const item &it );

units::mass minimal_weight_to_cut( const item &it );

std::vector<std::pair< const material_type *, float>> get_salvagable_materials(
            const item &target );

void complete_salvage( Character &who, item &cut, tripoint pos );

int moves_to_salvage( const item &target );

bool has_salvage_tools( const inventory &inv, item &item, bool check_charges = false );
bool salvage_single( Character &, item & );
bool salvage_all( Character & );

};


