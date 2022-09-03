#include "item_functions.h"

#include "item.h"

static flag_str_id flag_NO_UNLOAD( "NO_UNLOAD" );

namespace item_funcs
{

bool can_be_unloaded( const item &itm )
{
    if( ( itm.is_container() || itm.is_bandolier() ) && !itm.contents.empty() &&
        itm.can_unload_liquid() ) {
        return true;
    }

    if( itm.has_flag( flag_NO_UNLOAD ) ) {
        return false;
    }

    if( itm.magazine_current() ) {
        return true;
    }

    for( const item *mod : itm.gunmods() ) {
        if( can_be_unloaded( *mod ) ) {
            return true;
        }
    }

    if( itm.ammo_types().empty() ) {
        return false;
    }

    return itm.ammo_remaining() > 0 || itm.casings_count() > 0;
}

} // namespace item_funcs
