#include "overmap_location.h"

#include <algorithm>
#include <map>
#include <set>
#include <utility>

#include "debug.h"
#include "generic_factory.h"
#include "json.h"
#include "omdata.h"
#include "overmap.h"
#include "rng.h"

namespace
{

generic_factory<overmap_location> locations( "overmap location" );

} // namespace

template<>
auto string_id<overmap_location>::is_valid() const -> bool
{
    return locations.is_valid( *this );
}

template<>
auto string_id<overmap_location>::obj() const -> const overmap_location &
{
    return locations.obj( *this );
}

auto overmap_location::test( const oter_id &oter ) const -> bool
{
    return std::any_of( terrains.cbegin(), terrains.cend(),
    [ &oter ]( const oter_type_str_id & type ) {
        return oter->type_is( type );
    } );
}

auto overmap_location::get_random_terrain() const -> oter_type_id
{
    return random_entry( terrains );
}

void overmap_location::load( const JsonObject &jo, const std::string & )
{
    optional( jo, was_loaded, "flags", flags );
    optional( jo, was_loaded, "terrains", terrains );
    if( flags.empty() && terrains.empty() ) {
        jo.throw_error( "At least one flag or terrain must be specified." );
    }
}

auto overmap_location::get_all_terrains() const -> std::vector<oter_type_id>
{
    std::vector<oter_type_id> ret;
    for( oter_type_str_id elem : terrains ) {
        ret.push_back( elem );
    }
    return ret;
}

void overmap_location::check() const
{
    for( const auto &element : terrains ) {
        if( !element.is_valid() ) {
            debugmsg( "In overmap location \"%s\", terrain \"%s\" is invalid.", id.c_str(), element.c_str() );
        }
    }
}

void overmap_location::finalize()
{
    for( const std::string &elem : flags ) {
        auto it = oter_flags_map.find( elem );
        if( it == oter_flags_map.end() ) {
            continue;
        }
        oter_flags check_flag = it->second;
        for( const oter_t &ter_elem : overmap_terrains::get_all() ) {
            if( ter_elem.has_flag( check_flag ) ) {
                terrains.push_back( ter_elem.get_type_id() );
            }
        }
    }
}

void overmap_locations::load( const JsonObject &jo, const std::string &src )
{
    locations.load( jo, src );
}

void overmap_locations::check_consistency()
{
    locations.check();
}

void overmap_locations::reset()
{
    locations.reset();
}

void overmap_locations::finalize()
{
    locations.finalize();
    for( const overmap_location &elem : locations.get_all() ) {
        const_cast<overmap_location &>( elem ).finalize(); // This cast is ugly, but safe.
    }
}
