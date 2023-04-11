#include "item_functions.h"

#include "item.h"
#include "units.h"

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

// HACK: power needs to be provided due to hazy implementation of bionic guns.
int shots_remaining( item it, units::energy power )
{
    int shots = 0;
    units::energy energy_drain = units::from_kilojoule( it.get_gun_ups_drain() );
    if( it.ammo_required() > 0 && it.get_gun_ups_drain() > 0 ) {
        shots = std::min( it.ammo_remaining(), power / energy_drain );
    } else if( it.get_gun_ups_drain() > 0 ) {
        shots = power / energy_drain;
    } else if( it.ammo_required() > 0 ) {
        shots = it.ammo_remaining();
    } else {
        shots = 10;
    }
    return shots;
}

} // namespace item_funcs
