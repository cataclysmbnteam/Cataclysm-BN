#include "character_functions.h"

#include "calendar.h"
#include "character.h"
#include "creature.h"
#include "handle_liquid.h"
#include "itype.h"
#include "rng.h"
#include "vehicle.h"

static const trait_id trait_CANNIBAL( "CANNIBAL" );
static const trait_id trait_LOVES_BOOKS( "LOVES_BOOKS" );
static const trait_id trait_PSYCHOPATH( "PSYCHOPATH" );
static const trait_id trait_SAPIOVORE( "SAPIOVORE" );
static const trait_id trait_SPIRITUAL( "SPIRITUAL" );

static const itype_id itype_cookbook_human( "cookbook_human" );

namespace character_funcs
{

time_duration estimate_effect_dur( int skill_lvl, const efftype_id &target_effect,
                                   const time_duration &error_magnitude, int threshold, const Creature &target )
{
    const time_duration zero_duration = 0_turns;

    time_duration estimate = std::max( zero_duration, target.get_effect_dur( target_effect ) +
                                       rng( -1, 1 ) * error_magnitude *
                                       rng( 0, std::max( 0, threshold - skill_lvl ) ) );
    return estimate;
}

void siphon( Character &ch, vehicle &veh, const itype_id &desired_liquid )
{
    if( !ch.is_avatar() ) {
        // TODO: implement for NPCs
        debugmsg( "Siphoning not implemented for NPCs." );
        return;
    }
    auto qty = veh.fuel_left( desired_liquid );
    if( qty <= 0 ) {
        ch.add_msg_if_player( m_bad, _( "There is not enough %s left to siphon it." ),
                              item::nname( desired_liquid ) );
        return;
    }

    item liquid( desired_liquid, calendar::turn, qty );
    if( liquid_handler::handle_liquid( liquid, nullptr, 1, nullptr, &veh ) ) {
        veh.drain( desired_liquid, qty - liquid.charges );
    }
}

bool is_fun_to_read( const Character &ch, const item &book )
{
    // If you don't have a problem with eating humans, To Serve Man becomes rewarding
    if( ( ch.has_trait( trait_CANNIBAL ) || ch.has_trait( trait_PSYCHOPATH ) ||
          ch.has_trait( trait_SAPIOVORE ) ) &&
        book.typeId() == itype_cookbook_human ) {
        return true;
    } else if( ch.has_trait( trait_SPIRITUAL ) && book.has_flag( "INSPIRATIONAL" ) ) {
        return true;
    } else {
        return get_book_fun_for( ch, book ) > 0;
    }
}

int get_book_fun_for( const Character &ch, const item &book )
{
    int fun_bonus = book.type->book->fun;
    if( !book.is_book() ) {
        debugmsg( "called avatar::book_fun_for with non-book" );
        return 0;
    }

    // If you don't have a problem with eating humans, To Serve Man becomes rewarding
    if( ( ch.has_trait( trait_CANNIBAL ) ||
          ch.has_trait( trait_PSYCHOPATH ) ||
          ch.has_trait( trait_SAPIOVORE ) ) &&
        book.typeId() == itype_cookbook_human ) {
        fun_bonus = std::abs( fun_bonus );
    } else if( ch.has_trait( trait_SPIRITUAL ) && book.has_flag( "INSPIRATIONAL" ) ) {
        fun_bonus = std::abs( fun_bonus * 3 );
    }

    if( ch.has_trait( trait_LOVES_BOOKS ) ) {
        fun_bonus++;
    }

    if( fun_bonus > 1 && book.get_chapters() > 0 && book.get_remaining_chapters( ch ) == 0 ) {
        fun_bonus /= 2;
    }

    return fun_bonus;
}

} // namespace character_funcs
