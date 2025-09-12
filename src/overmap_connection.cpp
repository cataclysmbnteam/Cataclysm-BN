#include "overmap_connection.h"

#include <algorithm>
#include <cstddef>
#include <algorithm>
#include <cassert>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <vector>

#include "generic_factory.h"
#include "json.h"
#include "overmap_location.h"
#include "debug.h"
#include "type_id.h"
#include "weighted_list.h"
namespace
{

generic_factory<overmap_connection> connections( "overmap connection" );

} // namespace

namespace io
{

template<>
std::string enum_to_string<overmap_connection_layout>( overmap_connection_layout data )
{
    switch( data ) {
        // *INDENT-OFF*
        case overmap_connection_layout::city: return "city";
        case overmap_connection_layout::p2p: return "p2p";
        // *INDENT-ON*
        case overmap_connection_layout::last:
            break;
    }
    debugmsg( "Invalid overmap_connection_layout" );
    abort();
}

} // namespace io

static const std::map<std::string, overmap_connection::subtype::flag> connection_subtype_flag_map
= {
    { "ORTHOGONAL", overmap_connection::subtype::flag::orthogonal },
};

template<>
bool string_id<overmap_connection>::is_valid() const
{
    return connections.is_valid( *this );
}

template<>
const overmap_connection &string_id<overmap_connection>::obj() const
{
    return connections.obj( *this );
}

bool overmap_connection::subtype::allows_terrain( const oter_id &oter ) const
{
    if( oter->type_is( terrain ) ) {
        return true;    // Can be built on similar terrains.
    } else {
        if( oter->get_mapgen_id().starts_with( terrain.str() ) ) {
            return true;    // For bridges, starts the same, is the same
        }
    }

    return std::ranges::any_of( locations,
    [&oter]( const overmap_location_id & elem ) {
        return elem->test( oter );
    } );
}

void overmap_connection::subtype::load( const JsonObject &jo )
{
    const auto flag_reader = make_flag_reader( connection_subtype_flag_map, "connection subtype flag" );

    mandatory( jo, false, "terrain", terrain );
    mandatory( jo, false, "locations", locations );

    optional( jo, false, "basic_cost", basic_cost, 0 );
    optional( jo, false, "weight", weight, 1 );
    optional( jo, false, "flags", flags, flag_reader );
}

void overmap_connection::subtype::deserialize( JsonIn &jsin )
{
    JsonObject jo = jsin.get_object();
    load( jo );
}

void overmap_connection::clear_subtype_cache() const
{
    auto newcache = std::vector<cache>( overmap_terrains::get_all().size() );
    cached_subtypes.swap( newcache );
}

const overmap_connection::subtype *overmap_connection::pick_subtype_for(
    const oter_id &ground ) const
{
    if( !ground ) {
        return nullptr;
    }

    const size_t cache_index = ground.to_i();
    assert( cache_index < cached_subtypes.size() );

    if( cached_subtypes[cache_index] ) {
        return cached_subtypes[cache_index].value;
    }
    // Used below in find_if iteration
    auto passes = [&ground]( const subtype & elem ) {
        return elem.allows_terrain( ground );
    };
    const overmap_connection::subtype *result = nullptr;
    // Weighted list for indexes in subtypes
    // Can't just use the subtype, because then we return a local var pointer
    // No segfaults if we return the pointer to subtypes
    weighted_int_list<size_t> weighted_terrain;
    // Everything is lower then max
    int min_cost = INT_MAX;
    // For loop iterating through all instances of subtypes that can be placed here.
    for( auto iter = std::find_if( subtypes.begin(), subtypes.end(), passes );
         iter != subtypes.end();
         iter = std::find_if( ++iter, subtypes.end(), passes ) ) {
        auto subtype = &*iter;
        // If it's hardcoded to be set here
        // We better set it here
        if( subtype->locations.empty() ) {
            weighted_terrain.clear();
            weighted_terrain.add( iter - subtypes.begin(), subtype->weight );
            break;
        }
        // Revert if cost is lower, keep if same, ignore if higher
        if( subtype->basic_cost < min_cost ) {
            weighted_terrain.clear();
            weighted_terrain.add( iter - subtypes.begin(), subtype->weight );
            min_cost = subtype->basic_cost;
        } else if( subtype->basic_cost == min_cost ) {
            weighted_terrain.add( iter - subtypes.begin(), subtype->weight );
        }
    }
    // Essentially get a random one out of the weights
    // Only issue is we need to make sure it's not null
    // Null happens when trying to place a road over a cabin for instance
    size_t *idx = weighted_terrain.pick();
    if( idx != nullptr ) {
        result = &subtypes[*idx];
    }
    // Null subtypes are fine, and expected from this function
    // This is the cache so long roads are easy
    cached_subtypes[cache_index].value = result;
    cached_subtypes[cache_index].assigned = true;

    return result;
}

bool overmap_connection::can_start_at( const oter_id &ground ) const
{
    const overmap_connection::subtype *subtype = overmap_connection::pick_subtype_for( ground );
    return subtype != nullptr && subtype->allows_turns();
}

bool overmap_connection::has( const oter_id &oter ) const
{
    return std::ranges::find_if( subtypes, [&oter]( const subtype & elem ) {
        return oter->type_is( elem.terrain );
    } ) != subtypes.cend();
}

void overmap_connection::load( const JsonObject &jo, const std::string & )
{
    mandatory( jo, was_loaded, "default_terrain", default_terrain );
    mandatory( jo, was_loaded, "subtypes", subtypes );
    optional( jo, was_loaded, "layout", layout, overmap_connection_layout::city );
}

void overmap_connection::check() const
{
    if( subtypes.empty() ) {
        debugmsg( "Overmap connection \"%s\" doesn't have subtypes.", id.c_str() );
    }
    for( const auto &subtype : subtypes ) {
        if( !subtype.terrain.is_valid() ) {
            debugmsg( "In overmap connection \"%s\", terrain \"%s\" is invalid.", id.c_str(),
                      subtype.terrain.c_str() );
        }
        for( const auto &location : subtype.locations ) {
            if( !location.is_valid() ) {
                debugmsg( "In overmap connection \"%s\", location \"%s\" is invalid.", id.c_str(),
                          location.c_str() );
            }
        }
    }
}

void overmap_connection::finalize()
{
    cached_subtypes.resize( overmap_terrains::get_all().size() );
}

namespace overmap_connections
{

void load( const JsonObject &jo, const std::string &src )
{
    connections.load( jo, src );
}

void finalize()
{
    connections.finalize();
    for( const auto &elem : connections.get_all() ) {
        const_cast<overmap_connection &>( elem ).finalize(); // This cast is ugly, but safe.
    }
}

void check_consistency()
{
    connections.check();
}

void reset()
{
    connections.reset();
}

overmap_connection_id guess_for( const oter_id &oter )
{
    const auto &all = connections.get_all();
    const auto iter = std::ranges::find_if( all,
    [&oter]( const overmap_connection & elem ) {
        return elem.pick_subtype_for( oter ) != nullptr;
    } );

    return iter != all.cend() ? iter->id : overmap_connection_id::NULL_ID();
}

overmap_connection_id guess_for( const oter_type_id &oter )
{
    return guess_for( oter->get_first() );
}

} // namespace overmap_connections
