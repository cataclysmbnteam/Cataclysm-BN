#include "itype.h"

#include <cstdlib>

#include "debug.h"
#include "item.h"
#include "player.h"
#include "ret_val.h"
#include "translations.h"
#include "relic.h"
#include "skill.h"

struct tripoint;

auto gunmod_location::name() const -> std::string
{
    // Yes, currently the name is just the translated id.
    return _( _id );
}

namespace io
{
template<>
auto enum_to_string<condition_type>( condition_type data ) -> std::string
{
    switch( data ) {
        case condition_type::FLAG:
            return "FLAG";
        case condition_type::COMPONENT_ID:
            return "COMPONENT_ID";
        case condition_type::num_condition_types:
            break;
    }
    debugmsg( "Invalid condition_type" );
    abort();
}
} // namespace io

itype::itype()
{
    melee.fill( 0 );
}

itype::~itype() = default;

auto itype::nname( unsigned int quantity ) const -> std::string
{
    // Always use singular form for liquids.
    // (Maybe gases too?  There are no gases at the moment)
    if( phase == LIQUID ) {
        quantity = 1;
    }
    return name.translated( quantity );
}

auto itype::charges_per_volume( const units::volume &vol ) const -> int
{
    if( volume == 0_ml ) {
        // TODO: items should not have 0 volume at all!
        return item::INFINITE_CHARGES;
    }
    return ( count_by_charges() ? stack_size : 1 ) * vol / volume;
}

// Members of iuse struct, which is slowly morphing into a class.
auto itype::has_use() const -> bool
{
    return !use_methods.empty();
}

auto itype::has_flag( const std::string &flag ) const -> bool
{
    return item_tags.count( flag );
}

auto itype::has_flag( const flag_str_id &flag ) const -> bool
{
    return item_tags.count( flag.str() );
}

auto itype::get_flags() const -> const itype::FlagsSetType &
{
    return item_tags;
}

auto itype::can_use( const std::string &iuse_name ) const -> bool
{
    return get_use( iuse_name ) != nullptr;
}

auto itype::get_use( const std::string &iuse_name ) const -> const use_function *
{
    const auto iter = use_methods.find( iuse_name );
    return iter != use_methods.end() ? &iter->second : nullptr;
}

void itype::tick( player &p, item &it, const tripoint &pos ) const
{
    // Maybe should move charge decrementing here?
    for( auto &method : use_methods ) {
        method.second.call( p, it, true, pos );
    }
}

auto itype::invoke( player &p, item &it, const tripoint &pos ) const -> int
{
    if( !has_use() ) {
        return 0;
    }
    return invoke( p, it, pos, use_methods.begin()->first );
}

auto itype::invoke( player &p, item &it, const tripoint &pos, const std::string &iuse_name ) const -> int
{
    const use_function *use = get_use( iuse_name );
    if( use == nullptr ) {
        debugmsg( "Tried to invoke %s on a %s, which doesn't have this use_function",
                  iuse_name, nname( 1 ) );
        return 0;
    }

    const auto ret = use->can_call( p, it, false, pos );

    if( !ret.success() ) {
        p.add_msg_if_player( m_info, ret.str() );
        return 0;
    }

    return use->call( p, it, false, pos );
}

auto gun_type_type::name() const -> std::string
{
    return pgettext( "gun_type_type", name_.c_str() );
}

auto itype::can_have_charges() const -> bool
{
    if( count_by_charges() ) {
        return true;
    }
    if( tool && tool->max_charges > 0 ) {
        return true;
    }
    if( gun && gun->clip > 0 ) {
        return true;
    }
    if( has_flag( "CAN_HAVE_CHARGES" ) ) {
        return true;
    }
    return false;
}
