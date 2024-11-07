#include "item_functions.h"

#include "character.h"
#include "item.h"
#include "itype.h"
#include "units.h"

static flag_id flag_NO_UNLOAD( "NO_UNLOAD" );

static const itype_id itype_UPS( "UPS" );

namespace item_funcs
{

bool can_be_unloaded( const item &itm )
{
    if( ( itm.is_container() || itm.is_bandolier() || itm.type->can_use( "holster" ) ) &&
        !itm.contents.empty() &&
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

// TODO: Add consideration for BIONIC_GUNS when their fake_items get USES_BIONIC_POWER
int shots_remaining( const Character &who, const item &it )
{
    if( !it.is_gun() ) {
        return 0;
    }

    int ammo_drain = it.ammo_required();
    int energy_drain = it.get_gun_ups_drain();
    int power = who.charges_of( itype_UPS );

    if( ammo_drain > 0 && energy_drain > 0 ) {
        // Both UPS and ammo, lower is limiting.
        return std::min( it.ammo_remaining() / ammo_drain, power / energy_drain );
    } else if( energy_drain > 0 ) {
        //Only one of the two, it is limiting.
        return power / energy_drain;
    } else if( ammo_drain > 0 ) {
        return it.ammo_remaining() / ammo_drain;
    } else {
        // Effectively infinite ammo.
        return item::INFINITE_CHARGES;
    }
}

} // namespace item_funcs
