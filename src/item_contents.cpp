#include "item_contents.h"

#include <limits>
#include <algorithm>
#include <memory>

#include "character.h"
#include "enums.h"
#include "handle_liquid.h"
#include "item.h"
#include "itype.h"
#include "map.h"

struct tripoint;

auto item_contents::empty() const -> bool
{
    return items.empty();
}

auto item_contents::insert_item( const item &it ) -> ret_val<bool>
{
    items.push_back( it );
    return ret_val<bool>::make_success();
}

auto item_contents::num_item_stacks() const -> size_t
{
    return items.size();
}

auto item_contents::spill_contents( const tripoint &pos ) -> bool
{
    for( item &it : items ) {
        get_map().add_item_or_charges( pos, it );
    }

    items.clear();
    return true;
}

void item_contents::handle_liquid_or_spill( Character &guy )
{
    for( auto iter = items.begin(); iter != items.end(); ) {
        if( iter->made_of( LIQUID ) ) {
            item liquid( *iter );
            iter = items.erase( iter );
            liquid_handler::handle_all_liquid( liquid, 1 );
        } else {
            item i_copy( *iter );
            iter = items.erase( iter );
            guy.i_add_or_drop( i_copy );
        }
    }
}

void item_contents::casings_handle( const std::function<bool( item & )> &func )
{

    for( auto it = items.begin(); it != items.end(); ) {
        if( it->has_flag( "CASING" ) ) {
            it->unset_flag( "CASING" );
            if( func( *it ) ) {
                it = items.erase( it );
                continue;
            }
            // didn't handle the casing so reset the flag ready for next call
            it->set_flag( "CASING" );
        }
        ++it;
    }
}

void item_contents::clear_items()
{
    items.clear();
}

void item_contents::set_item_defaults()
{
    /* For Items with a magazine or battery in its contents */
    for( item &contained_item : items ) {
        /* for guns and other items defined to have a magazine but don't use "ammo" */
        if( contained_item.is_magazine() ) {
            contained_item.ammo_set(
                contained_item.ammo_default(), contained_item.ammo_capacity() / 2
            );
        } else { //Contents are batteries or food
            contained_item.charges = contained_item.typeId()->charges_default();
        }
    }
}

void item_contents::migrate_item( item &obj, const std::set<itype_id> &migrations )
{
    for( const itype_id &c : migrations ) {
        if( std::none_of( items.begin(), items.end(), [&]( const item & e ) {
        return e.typeId() == c;
        } ) ) {
            obj.put_in( item( c, obj.birthday() ) );
        }
    }
}

auto item_contents::has_any_with( const std::function<bool( const item &it )> &filter ) const -> bool
{
    return std::any_of( items.begin(), items.end(), filter );
}

auto item_contents::stacks_with( const item_contents &rhs ) const -> bool
{
    return std::equal( items.begin(), items.end(), rhs.items.begin(), []( const item & a,
    const item & b ) {
        return a.charges == b.charges && a.stacks_with( b );
    } );
}

auto item_contents::get_item_with( const std::function<bool( const item &it )> &filter ) -> item *
{
    auto bomb_it = std::find_if( items.begin(), items.end(), filter );
    if( bomb_it == items.end() ) {
        return nullptr;
    } else {
        return &*bomb_it;
    }
}

auto item_contents::all_items_top() -> std::list<item *>
{
    std::list<item *> ret;
    for( item &it : items ) {
        ret.push_back( &it );
    }
    return ret;
}

auto item_contents::all_items_top() const -> std::list<const item *>
{
    std::list<const item *> ret;
    for( const item &it : items ) {
        ret.push_back( &it );
    }
    return ret;
}

auto item_contents::all_items_ptr() -> std::list<item *>
{
    std::list<item *> ret;
    for( item &it : items ) {
        ret.push_back( &it );
        std::list<item *> inside = it.contents.all_items_ptr();
        ret.insert( ret.end(), inside.begin(), inside.end() );
    }
    return ret;
}

auto item_contents::all_items_ptr() const -> std::list<const item *>
{
    std::list<const item *> ret;
    for( const item &it : items ) {
        ret.push_back( &it );
        std::list<const item *> inside = it.contents.all_items_ptr();
        ret.insert( ret.end(), inside.begin(), inside.end() );
    }
    return ret;
}

auto item_contents::gunmods() -> std::vector<item *>
{
    std::vector<item *> res;
    for( item &e : items ) {
        if( e.is_gunmod() ) {
            res.push_back( &e );
        }
    }
    return res;
}

auto item_contents::gunmods() const -> std::vector<const item *>
{
    std::vector<const item *> res;
    for( const item &e : items ) {
        if( e.is_gunmod() ) {
            res.push_back( &e );
        }
    }
    return res;
}

auto item_contents::front() -> item &
{
    return items.front();
}

auto item_contents::front() const -> const item &
{
    return items.front();
}

auto item_contents::back() -> item &
{
    return items.back();
}

auto item_contents::back() const -> const item &
{
    return items.back();
}

auto item_contents::item_size_modifier() const -> units::volume
{
    units::volume ret = 0_ml;
    for( const item &it : items ) {
        ret += it.volume();
    }
    return ret;
}

auto item_contents::item_weight_modifier() const -> units::mass
{
    units::mass ret = 0_gram;
    for( const item &it : items ) {
        ret += it.weight();
    }
    return ret;
}

auto item_contents::best_quality( const quality_id &id ) const -> int
{
    int ret = INT_MIN;
    for( const item &it : items ) {
        ret = std::max( ret, it.get_quality( id ) );
    }
    return ret;
}
