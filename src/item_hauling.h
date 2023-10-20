#pragma once

#include "point.h"
#include "item.h"
#include "map.h"

// Item hauler helper class
class item_hauling
{
    public:
        // Checks if items at position are haulable
        static bool has_haulable_items( const tripoint &pos );

        // Checks and returns if an item is haulable
        // (e.g returns false if checked item has tag Liquid)
        static bool is_haulable( const item &item );
};

