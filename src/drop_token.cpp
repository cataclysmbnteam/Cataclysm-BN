#include "calendar.h"
#include "drop_token.h"
#include "game.h"
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

item_drop_token drop_token_provider::make_next( time_point tp )
{
    if( tp != last_turn ) {
        last_turn = tp;
        last_drop = 0;
    }

    last_drop++;

    return item_drop_token{ tp, last_drop, last_drop };
}

void drop_token_provider::clear()
{
    last_turn = calendar::before_time_starts;
    last_drop = 0;
}

namespace drop_token
{

drop_token_provider &get_provider()
{
    return *g->token_provider;
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
