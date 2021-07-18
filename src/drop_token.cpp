#include "calendar.h"
#include "drop_token.h"

namespace drop_token
{

item_drop_token make_next()
{
    // TODO: Implement properly
    static time_point last_turn = time_point::from_turn( -1 );
    static int last_drop = 0;
    if( calendar::turn != last_turn ) {
        last_turn = calendar::turn;
        last_drop = 0;
    }

    last_drop++;

    return item_drop_token{ calendar::turn, last_drop, last_drop };
}

}

std::ostream &operator<<( std::ostream &os, const item_drop_token &dt )
{
    os << "{turn:" << to_turn<int>( dt.turn )
       << ", drop:" << dt.drop_number
       << ", parent:" << dt.parent_number
       << '}';
    return os;
}
