#pragma once

#include <set>
#include <vector>

#include "type_id.h"

class Character;
class inventory;
class item;
class material_type;

static std::set<material_id> can_salvage_materials( const item &it );

bool try_salvage( Character &who, item &it, bool mute = true );

static std::vector<std::pair< material_type *, float>> get_salvagable_materials(
            const item &target );

void salvage( Character &who, item &cut, tripoint pos );

static int moves_to_salvage( const item &target );

bool has_salvage_tools( const inventory &inv, item &item, bool check_charges = false );

struct salvage_quality {
    public:
        quality_id quality;

        /** Moves used per unit of volume of cut item */
        int moves_per_part;

        /** Materials it can cut */
        std::set<material_id> salvagable_materials;
};


