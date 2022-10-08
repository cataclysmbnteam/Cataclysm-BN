#include "vehicle.h" // IWYU pragma: associated

#include <algorithm>
#include <cassert>
#include <cmath>
#include <memory>
#include <set>

#include "avatar.h"
#include "color.h"
#include "debug.h"
#include "enums.h"
#include "flat_set.h"
#include "game.h"
#include "item.h"
#include "item_contents.h"
#include "itype.h"
#include "map.h"
#include "messages.h"
#include "npc.h"
#include "string_formatter.h"
#include "translations.h"
#include "value_ptr.h"
#include "veh_type.h"
#include "vpart_position.h"
#include "weather.h"

static const itype_id fuel_type_battery( "battery" );
static const itype_id fuel_type_none( "null" );

static const itype_id itype_battery( "battery" );
static const itype_id itype_muscle( "muscle" );

/*-----------------------------------------------------------------------------
 *                              VEHICLE_PART
 *-----------------------------------------------------------------------------*/
vehicle_part::vehicle_part()
    : id( vpart_id::NULL_ID() ) {}

vehicle_part::vehicle_part( const vpart_id &vp, const point &dp, item &&obj )
    : mount( dp ), id( vp ), base( std::move( obj ) )
{
    // Mark base item as being installed as a vehicle part
    base.set_flag( "VEHICLE" );

    if( base.typeId() != vp->item ) {
        debugmsg( "incorrect vehicle part item, expected: %s, received: %s",
                  vp->item.c_str(), base.typeId().c_str() );
    }
}

vehicle_part::operator bool() const
{
    return id != vpart_id::NULL_ID();
}

auto vehicle_part::get_base() const -> const item &
{
    return base;
}

void vehicle_part::set_base( const item &new_base )
{
    base = new_base;
}

auto vehicle_part::properties_to_item() const -> item
{
    map &here = get_map();
    item tmp = base;
    tmp.unset_flag( "VEHICLE" );

    // Cables get special handling: their target coordinates need to remain
    // stored, and if a cable actually drops, it should be half-connected.
    // Except grid-connected ones, for now.
    if( tmp.has_flag( "CABLE_SPOOL" ) && !tmp.has_flag( "TOW_CABLE" ) ) {
        if( has_flag( targets_grid ) ) {
            // Ideally, we'd drop the cable on the charger instead
            tmp.erase_var( "source_x" );
            tmp.erase_var( "source_y" );
            tmp.erase_var( "source_z" );
            tmp.erase_var( "state" );
            tmp.active = false;
            tmp.charges = tmp.type->maximum_charges();
        } else {
            const tripoint local_pos = here.getlocal( target.first );
            if( !here.veh_at( local_pos ) ) {
                // That vehicle ain't there no more.
                tmp.set_flag( "NO_DROP" );
            }

            tmp.set_var( "source_x", target.first.x );
            tmp.set_var( "source_y", target.first.y );
            tmp.set_var( "source_z", target.first.z );
            tmp.set_var( "state", "pay_out_cable" );
            tmp.active = true;
        }
    }

    // force rationalization of damage values to the middle value of each damage level so
    // that parts will stack nicely
    tmp.set_damage( tmp.damage_level( 4 ) * itype::damage_scale );
    return tmp;
}

auto vehicle_part::name( bool with_prefix ) const -> std::string
{
    auto res = info().name();

    if( base.engine_displacement() > 0 ) {
        res.insert( 0, string_format( _( "%2.1fL " ), base.engine_displacement() / 100.0 ) );

    } else if( wheel_diameter() > 0 ) {
        res.insert( 0, string_format( _( "%d\" " ), wheel_diameter() ) );
    }

    if( base.is_faulty() ) {
        res += _( " (faulty)" );
    }

    if( base.has_var( "contained_name" ) ) {
        res += string_format( _( " holding %s" ), base.get_var( "contained_name" ) );
    }

    if( is_leaking() ) {
        res += _( " (draining)" );
    }

    if( with_prefix ) {
        res.insert( 0, colorize( base.damage_symbol(), base.damage_color() ) + " " );
    }
    return res;
}

auto vehicle_part::hp() const -> int
{
    const int dur = info().durability;
    if( base.max_damage() > 0 ) {
        return dur - dur * base.damage() / base.max_damage();
    } else {
        return dur;
    }
}

auto vehicle_part::damage() const -> int
{
    return base.damage();
}

auto vehicle_part::max_damage() const -> int
{
    return base.max_damage();
}

auto vehicle_part::damage_level( int max ) const -> int
{
    return base.damage_level( max );
}

auto vehicle_part::health_percent() const -> double
{
    return 1.0 - static_cast<double>( base.damage() ) / base.max_damage();
}

auto vehicle_part::damage_percent() const -> double
{
    return static_cast<double>( base.damage() ) / base.max_damage();
}

/** parts are considered broken at zero health */
auto vehicle_part::is_broken() const -> bool
{
    return base.damage() >= base.max_damage();
}

auto vehicle_part::is_unavailable( const bool carried ) const -> bool
{
    return is_broken() || ( has_flag( carried_flag ) && carried );
}

auto vehicle_part::is_available( const bool carried ) const -> bool
{
    return !is_unavailable( carried );
}

auto vehicle_part::fuel_current() const -> itype_id
{
    if( is_engine() ) {
        if( ammo_pref.is_null() ) {
            return info().fuel_type != itype_muscle ? info().fuel_type : itype_id::NULL_ID();
        } else {
            return ammo_pref;
        }
    }

    return itype_id::NULL_ID();
}

auto vehicle_part::fuel_set( const itype_id &fuel ) -> bool
{
    if( is_engine() ) {
        for( const itype_id &avail : info().engine_fuel_opts() ) {
            if( fuel == avail ) {
                ammo_pref = fuel;
                return true;
            }
        }
    }
    return false;
}

auto vehicle_part::ammo_current() const -> itype_id
{
    if( is_battery() ) {
        return itype_battery;
    }

    if( is_tank() && !base.contents.empty() ) {
        return base.contents.front().typeId();
    }

    if( is_fuel_store( false ) || is_turret() ) {
        return base.ammo_current();
    }

    return itype_id::NULL_ID();
}

auto vehicle_part::ammo_capacity() const -> int
{
    if( is_tank() ) {
        return ammo_current()->charges_per_volume( base.get_container_capacity() );
    }

    if( is_fuel_store( false ) || is_turret() ) {
        return base.ammo_capacity();
    }

    return 0;
}

auto vehicle_part::ammo_remaining() const -> int
{
    if( is_tank() ) {
        return base.contents.empty() ? 0 : base.contents.back().charges;
    }

    if( is_fuel_store( false ) || is_turret() ) {
        return base.ammo_remaining();
    }

    return 0;
}

auto vehicle_part::ammo_set( const itype_id &ammo, int qty ) -> int
{
    const itype *liquid = &*ammo;

    // We often check if ammo is set to see if tank is empty, if qty == 0 don't set ammo
    if( is_tank() && liquid->phase >= LIQUID && qty != 0 ) {
        base.contents.clear_items();
        const auto stack = units::legacy_volume_factor / std::max( liquid->stack_size, 1 );
        const int limit = units::from_milliliter( ammo_capacity() ) / stack;
        base.put_in( item( ammo, calendar::turn, qty > 0 ? std::min( qty, limit ) : limit ) );
        return qty;
    }

    if( is_turret() ) {
        return base.ammo_set( ammo, qty ).ammo_remaining();
    }

    if( is_fuel_store() ) {
        base.ammo_set( ammo, qty >= 0 ? qty : ammo_capacity() );
        return base.ammo_remaining();
    }

    return -1;
}

void vehicle_part::ammo_unset()
{
    if( is_tank() ) {
        base.contents.clear_items();
    } else if( is_fuel_store() ) {
        base.ammo_unset();
    }
}

auto vehicle_part::ammo_consume( int qty, const tripoint &pos ) -> int
{
    if( is_tank() && !base.contents.empty() ) {
        const int res = std::min( ammo_remaining(), qty );
        item &liquid = base.contents.back();
        liquid.charges -= res;
        if( liquid.charges == 0 ) {
            base.contents.clear_items();
        }
        return res;
    }
    return base.ammo_consume( qty, pos );
}

auto vehicle_part::consume_energy( const itype_id &ftype, double energy_j ) -> double
{
    if( base.contents.empty() || !is_fuel_store() ) {
        return 0.0f;
    }

    item &fuel = base.contents.back();
    if( fuel.typeId() == ftype ) {
        assert( fuel.is_fuel() );
        // convert energy density in MJ/L to J/ml
        const double energy_p_mL = fuel.fuel_energy() * 1000;
        const int ml_to_use = static_cast<int>( std::floor( energy_j / energy_p_mL ) );
        int charges_to_use = fuel.charges_per_volume( ml_to_use * 1_ml );

        if( !charges_to_use ) {
            return 0.0;
        }
        if( charges_to_use >= fuel.charges ) {
            charges_to_use = fuel.charges;
            base.contents.clear_items();
        } else {
            fuel.charges -= charges_to_use;
        }
        item fuel_consumed( ftype, calendar::turn, charges_to_use );
        return energy_p_mL * units::to_milliliter<int>( fuel_consumed.volume( true ) );
    }
    return 0.0;
}

auto vehicle_part::can_reload( const item &obj ) const -> bool
{
    // first check part is not destroyed and can contain ammo
    if( !is_fuel_store() ) {
        return false;
    }

    if( !obj.is_null() ) {
        const itype_id obj_type = obj.typeId();
        if( is_reactor() ) {
            return base.is_reloadable_with( obj_type );
        }

        // forbid filling tanks with solids or non-material things
        if( is_tank() && ( obj.made_of( SOLID ) || obj.made_of( PNULL ) ) ) {
            return false;
        }
        // forbid putting liquids, gasses, and plasma in things that aren't tanks
        else if( !obj.made_of( SOLID ) && !is_tank() ) {
            return false;
        }
        // prevent mixing of different ammo
        if( !ammo_current().is_null() && ammo_current() != obj_type ) {
            return false;
        }
        // For storage with set type, prevent filling with different types
        if( info().fuel_type != fuel_type_none && info().fuel_type != obj_type ) {
            return false;
        }
        // don't fill magazines with inappropriate fuel
        if( !is_tank() && !base.is_reloadable_with( obj_type ) ) {
            return false;
        }
    }

    return ammo_remaining() < ammo_capacity();
}

void vehicle_part::process_contents( const tripoint &pos, const bool e_heater )
{
    // for now we only care about processing food containers since things like
    // fuel don't care about temperature yet
    if( base.is_food_container() ) {
        temperature_flag flag = temperature_flag::TEMP_NORMAL;
        if( e_heater ) {
            flag = temperature_flag::TEMP_HEATER;
        }
        base.process( nullptr, pos, false, 1, flag );
    }
}

auto vehicle_part::fill_with( item &liquid, int qty ) -> bool
{
    if( !is_tank() || !can_reload( liquid ) ) {
        return false;
    }

    base.fill_with( liquid, qty );
    return true;
}

auto vehicle_part::faults() const -> const std::set<fault_id> &
{
    return base.faults;
}

auto vehicle_part::faults_potential() const -> std::set<fault_id>
{
    return base.faults_potential();
}

auto vehicle_part::fault_set( const fault_id &f ) -> bool
{
    if( !faults_potential().count( f ) ) {
        return false;
    }
    base.faults.insert( f );
    return true;
}

auto vehicle_part::wheel_area() const -> int
{
    return info().wheel_area();
}

/** Get wheel diameter (inches) or return 0 if part is not wheel */
auto vehicle_part::wheel_diameter() const -> int
{
    return base.is_wheel() ? base.type->wheel->diameter : 0;
}

/** Get wheel width (inches) or return 0 if part is not wheel */
auto vehicle_part::wheel_width() const -> int
{
    return base.is_wheel() ? base.type->wheel->width : 0;
}

auto vehicle_part::crew() const -> npc *
{
    if( is_broken() || !crew_id.is_valid() ) {
        return nullptr;
    }

    npc *const res = g->critter_by_id<npc>( crew_id );
    if( !res ) {
        return nullptr;
    }
    return res->is_player_ally() ? res : nullptr;
}

auto vehicle_part::set_crew( const npc &who ) -> bool
{
    if( who.is_dead_state() || !( who.is_walking_with() || who.is_player_ally() ) ) {
        return false;
    }
    if( is_broken() || ( !is_seat() && !is_turret() ) ) {
        return false;
    }
    crew_id = who.getID();
    return true;
}

void vehicle_part::unset_crew()
{
    crew_id = character_id();
}

void vehicle_part::reset_target( const tripoint &pos )
{
    target.first = pos;
    target.second = pos;
}

auto vehicle_part::is_engine() const -> bool
{
    return info().has_flag( VPFLAG_ENGINE );
}

auto vehicle_part::is_light() const -> bool
{
    const auto &vp = info();
    return vp.has_flag( VPFLAG_CONE_LIGHT ) ||
           vp.has_flag( VPFLAG_WIDE_CONE_LIGHT ) ||
           vp.has_flag( VPFLAG_HALF_CIRCLE_LIGHT ) ||
           vp.has_flag( VPFLAG_CIRCLE_LIGHT ) ||
           vp.has_flag( VPFLAG_AISLE_LIGHT ) ||
           vp.has_flag( VPFLAG_DOME_LIGHT ) ||
           vp.has_flag( VPFLAG_ATOMIC_LIGHT );
}

auto vehicle_part::is_fuel_store( bool skip_broke ) const -> bool
{
    if( skip_broke && is_broken() ) {
        return false;
    }
    return is_tank() || base.is_magazine() || is_reactor();
}

auto vehicle_part::is_tank() const -> bool
{
    return base.is_watertight_container();
}

auto vehicle_part::is_battery() const -> bool
{
    return base.is_magazine() && base.ammo_types().count( ammotype( "battery" ) );
}

auto vehicle_part::is_reactor() const -> bool
{
    return info().has_flag( VPFLAG_REACTOR );
}

auto vehicle_part::is_leaking() const -> bool
{
    return  health_percent() <= 0.5 && ( is_tank() || is_battery() || is_reactor() );
}

auto vehicle_part::is_turret() const -> bool
{
    return base.is_gun();
}

auto vehicle_part::is_seat() const -> bool
{
    return info().has_flag( "SEAT" );
}

auto vehicle_part::info() const -> const vpart_info &
{
    if( !info_cache ) {
        info_cache = &id.obj();
    }
    return *info_cache;
}

void vehicle::set_hp( vehicle_part &pt, int qty )
{
    if( qty == pt.info().durability || pt.info().durability <= 0 ) {
        pt.base.set_damage( 0 );

    } else if( qty == 0 ) {
        pt.base.set_damage( pt.base.max_damage() );

    } else {
        pt.base.set_damage( pt.base.max_damage() - pt.base.max_damage() * qty / pt.info().durability );
    }
}

auto vehicle::mod_hp( vehicle_part &pt, int qty, damage_type dt ) -> bool
{
    if( pt.info().durability > 0 ) {
        return pt.base.mod_damage( -( pt.base.max_damage() * qty / pt.info().durability ), dt );
    } else {
        return false;
    }
}

auto vehicle::can_enable( const vehicle_part &pt, bool alert ) const -> bool
{
    if( std::none_of( parts.begin(), parts.end(), [&pt]( const vehicle_part & e ) {
    return &e == &pt;
} ) || pt.removed ) {
        debugmsg( "Cannot enable removed or non-existent part" );
    }

    if( pt.is_broken() ) {
        return false;
    }

    if( pt.info().has_flag( "PLANTER" ) && !warm_enough_to_plant( g->u.pos() ) ) {
        if( alert ) {
            add_msg( m_bad, _( "It is too cold to plant anything now." ) );
        }
        return false;
    }

    // TODO: check fuel for combustion engines

    if( pt.info().epower < 0 && fuel_left( fuel_type_battery, true ) <= 0 ) {
        if( alert ) {
            add_msg( m_bad, _( "Insufficient power to enable %s" ), pt.name() );
        }
        return false;
    }

    return true;
}

auto vehicle::assign_seat( vehicle_part &pt, const npc &who ) -> bool
{
    if( !pt.is_seat() || !pt.set_crew( who ) ) {
        return false;
    }

    // NPC's can only be assigned to one seat in the vehicle
    for( auto &e : parts ) {
        if( &e == &pt ) {
            // skip this part
            continue;
        }

        if( e.is_seat() ) {
            const npc *n = e.crew();
            if( n && n->getID() == who.getID() ) {
                e.unset_crew();
            }
        }
    }

    return true;
}

auto vehicle_part::carried_name() const -> std::string
{
    if( carry_names.empty() ) {
        return std::string();
    }
    return carry_names.top().substr( name_offset );
}
