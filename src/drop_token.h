#pragma once
#ifndef CATA_SRC_ITEM_DROP_TOKEN_H
#define CATA_SRC_ITEM_DROP_TOKEN_H

#include "calendar.h"

class item_drop_token
{
    public:
        time_point turn;
        int drop_number;
        int parent_number;

        item_drop_token() : item_drop_token( time_point(), 0, 0 )
        {}
        item_drop_token( time_point turn, int drop_number, int parent_number )
            : turn( turn )
            , drop_number( drop_number )
            , parent_number( parent_number )
        {}
};

namespace drop_token
{

item_drop_token make_next();

};

#endif // CATA_SRC_ITEM_DROP_TOKEN_H
