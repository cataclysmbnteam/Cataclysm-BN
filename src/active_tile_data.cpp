#include "active_tile_data.h"
#include "coordinate_conversions.h"
#include "debug.h"
#include "distribution_grid.h"
#include "item.h"
#include "itype.h"
#include "json.h"
#include "map.h"
#include "mapbuffer.h"
#include "rng.h"
#include "vehicle.h"
#include "vpart_range.h"
#include "weather.h"

// TODO: Shouldn't use
#include "submap.h"

static const itype_id itype_battery( "battery" );

static const std::string flag_RECHARGE( "RECHARGE" );
static const std::string flag_USE_UPS( "USE_UPS" );

namespace active_tiles
{

template<typename T>
T *furn_at( const tripoint &p )
{
    tripoint xy_offset = p;
    // TODO: A "ms_to_sm" with explicit offset result
    submap *sm = MAPBUFFER.lookup_submap( ms_to_sm_remain( xy_offset ) );
    if( sm == nullptr ) {
        return nullptr;
    }
    auto iter = sm->active_furniture.find( xy_offset.xy() );
    if( iter == sm->active_furniture.end() ) {
        return nullptr;
    }

    return dynamic_cast<T *>( &*iter->second );
}

template active_tile_data *furn_at<active_tile_data>( const tripoint & );
template vehicle_connector_tile *furn_at<vehicle_connector_tile>( const tripoint & );
template battery_tile *furn_at<battery_tile>( const tripoint & );

} // namespace active_tiles

active_tile_data::~active_tile_data() = default;

void active_tile_data::serialize( JsonOut &jsout ) const
{
    jsout.member( "last_updated", last_updated );
    store( jsout );
}

void active_tile_data::deserialize( JsonIn &jsin )
{
    JsonObject jo( jsin );
    jo.read( "last_updated", last_updated );
    load( jo );
}

class null_tile_data : public active_tile_data
{
        void update_internal( time_point, const tripoint &, distribution_grid & ) override
        {}
        active_tile_data *clone() const override {
            return new null_tile_data( *this );
        }

        const std::string &get_type() const override {
            static const std::string type( "null" );
            return type;
        }
        void store( JsonOut & ) const override
        {}
        void load( JsonObject & ) override
        {}
};

// Copypasted from character.cpp
// TODO: Move somewhere (calendar?)
inline int ticks_between( const time_point &from, const time_point &to,
                          const time_duration &tick_length )
{
    return ( to_turn<int>( to ) / to_turns<int>( tick_length ) ) - ( to_turn<int>
            ( from ) / to_turns<int>( tick_length ) );
}

void solar_tile::update_internal( time_point to, const tripoint &p, distribution_grid &grid )
{
    constexpr time_point zero = time_point::from_turn( 0 );
    constexpr time_duration tick_length = 10_minutes;
    constexpr int tick_turns = to_turns<int>( tick_length );
    time_duration till_then = get_last_updated() - zero;
    time_duration till_now = to - zero;
    // This is just for rounding to nearest tick
    time_duration ticks_then = till_then / tick_turns;
    time_duration ticks_now = till_now / tick_turns;
    // This is to cut down on sum_conditions
    if( ticks_then == ticks_now ) {
        return;
    }
    time_duration rounded_then = ticks_then * tick_turns;
    time_duration rounded_now = ticks_now * tick_turns;

    // TODO: Use something that doesn't calc a ton of worthless crap
    float sunlight = sum_conditions( zero + rounded_then, zero + rounded_now,
                                     p ).sunlight / default_daylight_level();
    // int64 because we can have years in here
    std::int64_t produced = power * static_cast<std::int64_t>( sunlight ) / 1000;
    grid.mod_resource( static_cast<int>( std::min( static_cast<std::int64_t>( INT_MAX ), produced ) ) );
}

active_tile_data *solar_tile::clone() const
{
    return new solar_tile( *this );
}

const std::string &solar_tile::get_type() const
{
    static const std::string type( "solar" );
    return type;
}

void solar_tile::store( JsonOut &jsout ) const
{
    jsout.member( "power", power );
}

void solar_tile::load( JsonObject &jo )
{
    // Can't use generic_factory because we don't have unique ids
    jo.read( "power", power );
    // TODO: Remove all of this, it's a hack around a mistake
    int dummy;
    jo.read( "stored_energy", dummy, false );
    jo.read( "max_energy", dummy, false );

}

void battery_tile::update_internal( time_point, const tripoint &, distribution_grid & )
{
    // TODO: Shouldn't have this function!
}

active_tile_data *battery_tile::clone() const
{
    return new battery_tile( *this );
}

const std::string &battery_tile::get_type() const
{
    static const std::string type( "battery" );
    return type;
}

void battery_tile::store( JsonOut &jsout ) const
{
    jsout.member( "stored", stored );
    jsout.member( "max_stored", max_stored );
}
void battery_tile::load( JsonObject &jo )
{
    jo.read( "stored", stored );
    jo.read( "max_stored", max_stored );
}

int battery_tile::get_resource() const
{
    return stored;
}

int battery_tile::mod_resource( int amt )
{
    // TODO: Avoid int64 math if possible
    std::int64_t sum = static_cast<std::int64_t>( stored ) + amt;
    if( sum >= max_stored ) {
        stored = max_stored;
        return sum - max_stored;
    } else if( sum <= 0 ) {
        stored = 0;
        return sum - stored;
    } else {
        stored = sum;
        return 0;
    }
}

void charger_tile::update_internal( time_point to, const tripoint &p, distribution_grid &grid )
{
    tripoint loc_on_sm = p;
    const tripoint sm_pos = ms_to_sm_remain( loc_on_sm );
    submap *sm = MAPBUFFER.lookup_submap( sm_pos );
    if( sm == nullptr ) {
        return;
    }
    std::int64_t power = this->power * to_seconds<std::int64_t>( to - get_last_updated() );
    // TODO: Make not a copy from map.cpp
    for( item &outer : sm->get_items( loc_on_sm.xy() ) ) {
        outer.visit_items( [&power, &grid]( item * it ) {
            item &n = *it;
            if( !n.has_flag( flag_RECHARGE ) && !n.has_flag( flag_USE_UPS ) ) {
                return VisitResponse::NEXT;
            }
            if( n.ammo_capacity() > n.ammo_remaining() ||
                ( n.type->battery && n.type->battery->max_capacity > n.energy_remaining() ) ) {
                while( power >= 1000 || x_in_y( power, 1000 ) ) {
                    const int missing = grid.mod_resource( -1 );
                    if( missing == 0 ) {
                        if( n.is_battery() ) {
                            n.mod_energy( 1_kJ );
                        } else {
                            n.ammo_set( itype_battery, n.ammo_remaining() + 1 );
                        }
                    }
                    power -= 1000;
                }
                return VisitResponse::ABORT;
            }

            return VisitResponse::SKIP;
        } );
    }
}

active_tile_data *charger_tile::clone() const
{
    return new charger_tile( *this );
}

const std::string &charger_tile::get_type() const
{
    static const std::string type( "charger" );
    return type;
}

void charger_tile::store( JsonOut &jsout ) const
{
    jsout.member( "power", power );
}

void charger_tile::load( JsonObject &jo )
{
    jo.read( "power", power );
}

void vehicle_connector_tile::update_internal( time_point, const tripoint &, distribution_grid & )
{
}

active_tile_data *vehicle_connector_tile::clone() const
{
    return new vehicle_connector_tile( *this );
}

const std::string &vehicle_connector_tile::get_type() const
{
    static const std::string type( "vehicle_connector" );
    return type;
}

void vehicle_connector_tile::store( JsonOut &jsout ) const
{
    jsout.member( "connected_vehicles", connected_vehicles );
}

void vehicle_connector_tile::load( JsonObject &jo )
{
    jo.read( "connected_vehicles", connected_vehicles );
}

static std::map<std::string, std::unique_ptr<active_tile_data>> build_type_map()
{
    std::map<std::string, std::unique_ptr<active_tile_data>> type_map;
    const auto add_type = [&type_map]( active_tile_data * arg ) {
        type_map[arg->get_type()].reset( arg );
    };
    add_type( new solar_tile() );
    add_type( new battery_tile() );
    add_type( new charger_tile() );
    add_type( new vehicle_connector_tile() );
    return type_map;
}

active_tile_data *active_tile_data::create( const std::string &id )
{
    static const auto type_map = build_type_map();
    const auto iter = type_map.find( id );
    if( iter == type_map.end() ) {
        debugmsg( "Invalid active_tile_data id %s", id.c_str() );
        return new null_tile_data();
    }

    active_tile_data *new_tile = iter->second->clone();
    new_tile->last_updated = calendar::start_of_cataclysm;
    return new_tile;
}
