#pragma once
#ifndef CATA_SRC_CONSTRUCTION_PARTIAL_H
#define CATA_SRC_CONSTRUCTION_PARTIAL_H

#include <list>

#include "item.h"
#include "type_id.h"

struct partial_con {
    int counter = 0;
    std::list<item> components = {};
    construction_id id = construction_id( -1 );
};

#endif // CATA_SRC_CONSTRUCTION_PARTIAL_H
