#include "calendar.h"
#include "drop_token.h"
#include "json.h"

void item_drop_token::serialize( JsonOut &jsout ) const
{
    jsout.start_object();

    jsout.member( "turn", turn );
    jsout.member( "drop_number", drop_number );
    jsout.member( "parent_number", parent_number );

    jsout.end_object();
}

void item_drop_token::deserialize( JsonIn &jsin )
{
    JsonObject jo = jsin.get_object();
    jo.read( "turn", turn );
    jo.read( "drop_number", drop_number );
    jo.read( "parent_number", parent_number );
}

namespace drop_token
{

item_drop_token make_next()
{
    // TODO: Implement properly
    static time_point last_turn = calendar::before_time_starts;
    static int last_drop = 0;
    if( calendar::turn != last_turn ) {
        last_turn = calendar::turn;
        last_drop = 0;
    }

    last_drop++;

    return item_drop_token{ calendar::turn, last_drop, last_drop };
}

} // namespace drop_token

std::ostream &operator<<( std::ostream &os, const item_drop_token &dt )
{
    os << "{turn:" << to_turn<int>( dt.turn )
       << ", drop:" << dt.drop_number
       << ", parent:" << dt.parent_number
       << '}';
    return os;
}
