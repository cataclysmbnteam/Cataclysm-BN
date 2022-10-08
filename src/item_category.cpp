#include "item_category.h"

#include <set>

#include "generic_factory.h"
#include "item.h"
#include "json.h"
#include "string_id.h"

namespace
{
generic_factory<item_category> item_category_factory( "item_category" );
} // namespace

template<>
auto string_id<item_category>::obj() const -> const item_category &
{
    return item_category_factory.obj( *this );
}

template<>
auto string_id<item_category>::is_valid() const -> bool
{
    return item_category_factory.is_valid( *this );
}

void zone_priority_data::deserialize( JsonIn &jsin )
{
    JsonObject data = jsin.get_object();
    load( data );
}

void zone_priority_data::load( JsonObject &jo )
{
    mandatory( jo, was_loaded, "id", id );
    optional( jo, was_loaded, "flags", flags );
    optional( jo, was_loaded, "filthy", filthy, false );
}

void item_category::load_item_cat( const JsonObject &jo, const std::string &src )
{
    item_category_factory.load( jo, src );
}

void item_category::load( const JsonObject &jo, const std::string & )
{
    mandatory( jo, was_loaded, "id", id );
    mandatory( jo, was_loaded, "name", name_ );
    mandatory( jo, was_loaded, "sort_rank", sort_rank_ );
    optional( jo, was_loaded, "priority_zones", zone_priority_ );
    optional( jo, was_loaded, "zone", zone_, cata::nullopt );
}

auto item_category::operator<( const item_category &rhs ) const -> bool
{
    if( sort_rank_ != rhs.sort_rank_ ) {
        return sort_rank_ < rhs.sort_rank_;
    }
    if( name_.translated_ne( rhs.name_ ) ) {
        return name_.translated_lt( rhs.name_ );
    }
    return id < rhs.id;
}

auto item_category::operator==( const item_category &rhs ) const -> bool
{
    return sort_rank_ == rhs.sort_rank_ && name_.translated_eq( rhs.name_ ) && id == rhs.id;
}

auto item_category::operator!=( const item_category &rhs ) const -> bool
{
    return !operator==( rhs );
}

auto item_category::name() const -> std::string
{
    return name_.translated();
}

auto item_category::get_id() const -> item_category_id
{
    return id;
}

auto item_category::zone() const -> cata::optional<zone_type_id>
{
    return zone_;
}

auto item_category::priority_zone( const item &it ) const -> cata::optional<zone_type_id>
{
    for( const zone_priority_data &zone_dat : zone_priority_ ) {
        if( zone_dat.filthy ) {
            if( it.is_filthy() ) {
                return zone_dat.id;

            } else {
                continue;
            }
        }
        for( const std::string &flag : zone_dat.flags ) {
            if( it.has_flag( flag ) ) {
                return zone_dat.id;
            }
        }
    }
    return cata::nullopt;
}

auto item_category::sort_rank() const -> int
{
    return sort_rank_;
}
