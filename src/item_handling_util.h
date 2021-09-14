#pragma once
#ifndef CATA_SRC_ITEM_HANDLING_UTIL_H
#define CATA_SRC_ITEM_HANDLING_UTIL_H

#include <list>

#include "item_location.h"

class JsonIn;
class JsonOut;

struct iuse_location {
    iuse_location() = default;
    ~iuse_location() = default;
    iuse_location( const item_location &loc, int count ) : loc( loc ), count( count ) { }

    item_location loc;
    int count = 0;

    void serialize( JsonOut &jsout ) const;
    void deserialize( JsonIn &jsin );
};
using iuse_locations = std::list<iuse_location>;

using drop_location = iuse_location;
using drop_locations = std::list<drop_location>;

#endif // CATA_SRC_ITEM_HANDLING_UTIL_H
