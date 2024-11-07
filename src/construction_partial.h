#pragma once
#ifndef CATA_SRC_CONSTRUCTION_PARTIAL_H
#define CATA_SRC_CONSTRUCTION_PARTIAL_H

#include <list>

#include "item.h"
#include "type_id.h"
#include "locations.h"

struct partial_con {
    partial_con( tripoint loc ) : components( new partial_con_item_location( loc ) ) {};
    int counter = 0;
    location_vector<item> components;
    construction_id id = construction_id( -1 );
};

#endif // CATA_SRC_CONSTRUCTION_PARTIAL_H
