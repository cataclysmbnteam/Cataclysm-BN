#include "drop_token.h"

namespace drop_token
{

item_drop_token make_next()
{
    // TODO: Implement properly
    static time_point last_turn;
    static int last_drop = 0;
    if( calendar::turn != last_turn ) {
        last_turn = calendar::turn;
        last_drop = 0;
    }

    return item_drop_token{ calendar::turn, last_drop, 0 };
}

};
