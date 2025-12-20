#include "catalua_bindings.h"
#include "catalua_bindings_utils.h"
#include "catalua_luna.h"
#include "catalua_luna_doc.h"

#include "distribution_grid.h"
#include "field.h"
#include "map.h"
#include "map_iterator.h"
#include "trap.h"
#include "detached_ptr.h"

namespace sol
{
template <>
struct is_container<item_stack> : std::false_type {};
template <>
struct is_container<map_stack> : std::false_type {};
} // namespace sol

namespace
{

struct item_stack_lua_it_state {
    item_stack *stack;
    size_t index;

    item_stack_lua_it_state( item_stack &stk )
        : stack( &stk ), index( 0 ) {
    }
};

static std::tuple<sol::object, sol::object>
item_stack_lua_next(
    sol::user<item_stack_lua_it_state &> user_it_state,
    sol::this_state l )
{
    // this gets called
    // to start the first iteration, and every
    // iteration there after

    // the state you passed in item_stack_lua_pairs is argument 1
    // the key value is argument 2, but we do not
    // care about the key value here
    item_stack_lua_it_state &it_state = user_it_state;
    if( it_state.index >= it_state.stack->size() ) {
        // return nil to signify that
        // there's nothing more to work with.
        return std::make_tuple( sol::object( sol::lua_nil ),
                                sol::object( sol::lua_nil ) );
    }
    auto it = it_state.stack->begin();
    std::advance( it, it_state.index );
    item *elem = *it;
    // 2 values are returned (pushed onto the stack):
    // the key and the value
    // the state is left alone
    auto r = std::make_tuple(
                 sol::object( l,  sol::in_place, it_state.index ),
                 sol::object( l, sol::in_place, elem ) );
    // the iterator must be moved forward one before we return
    it_state.index++;
    return r;
}

auto item_stack_lua_pairs( item_stack &stk )
{
    // pairs expects 3 returns:
    // the "next" function on how to advance,
    // the "table" itself or some state,
    // and an initial key value (can be nil)

    // prepare our state
    item_stack_lua_it_state it_state( stk );
    // sol::user is a space/time optimization over regular
    // usertypes, it's incompatible with regular usertypes and
    // stores the type T directly in lua without any pretty
    // setup saves space allocation and a single dereference
    return std::make_tuple( &item_stack_lua_next,
                            sol::user<item_stack_lua_it_state>( std::move( it_state ) ),
                            sol::lua_nil );
}

auto item_stack_lua_length( const item_stack &stk )
{
    return stk.size();
}


item *item_stack_lua_index( item_stack &stk, int i )
{
    --i;
    if( i < 0 || i >= static_cast<int>( stk.size() ) ) {
        return nullptr;
    }
    auto it = stk.begin();
    std::advance( it, i );
    return *it;
}

} // namespace

void cata::detail::reg_map( sol::state &lua )
{
    // Register 'map' class to be used in Lua
    {
        sol::usertype<map> ut = luna::new_usertype<map>( lua, luna::no_bases, luna::no_constructor );

        DOC( "Convert local ms -> absolute ms" );
        luna::set_fx( ut, "get_abs_ms", sol::resolve<tripoint( const tripoint & ) const>( &map::getabs ) );
        DOC( "Convert absolute ms -> local ms" );
        luna::set_fx( ut, "get_local_ms",
                      sol::resolve<tripoint( const tripoint & ) const>( &map::getlocal ) );

        luna::set_fx( ut, "get_map_size_in_submaps", &map::getmapsize );
        DOC( "In map squares" );
        luna::set_fx( ut, "get_map_size", []( const map & m ) -> int { return m.getmapsize() * SEEX; } );

        DOC( "Creates a new item(s) at a position on the map." );
        luna::set_fx( ut, "create_item_at", []( map & m, const tripoint & p, const itype_id & itype,
        int count ) -> item* {
            detached_ptr<item> new_item = item::spawn( itype, calendar::turn, count );
            return m.add_item_or_charges( p, std::move( new_item ) ).get();
        } );

        DOC( "Creates a new corpse at a position on the map. You can skip `Opt` ones by omitting them or passing `nil`. `MtypeId` specifies which monster's body it is, `TimePoint` indicates when it died, `string` gives it a custom name, and `int` determines the revival time if the monster has the `REVIVES` flag." );
        luna::set_fx( ut, "create_corpse_at", []( map & m, const tripoint & p,
                      sol::optional<mtype_id> mtype,
                      sol::optional<time_point> turn, sol::optional<std::string> name,
        sol::optional<int> upgrade_time ) -> void {
            mtype_id the_id = mtype.value_or( mtype_id::NULL_ID() );
            time_point the_tp = turn.value_or( calendar::turn );
            std::string the_name = name.value_or( "" );
            int the_upgrade = upgrade_time.value_or( -1 );

            detached_ptr<item> new_corpse = item::make_corpse( the_id, the_tp, the_name, the_upgrade );
            m.add_item_or_charges( p, std::move( new_corpse ) );
        } );

        luna::set_fx( ut, "has_items_at", &map::has_items );
        luna::set_fx( ut, "remove_item_at", []( map & m, const tripoint & p, item * it ) -> void { m.i_rem( p, it ); } );
        luna::set_fx( ut, "clear_items_at", []( map & m, const tripoint & p ) -> void { m.i_clear( p ); } );

        luna::set_fx( ut, "get_items_at", []( map & m, const tripoint & p ) {
            return m.i_at( p );
        } );
        luna::set_fx( ut, "get_items_in_radius", []( map & m, const tripoint & p,
        int radius ) -> std::vector<map_stack> {
            std::vector<map_stack> items;
            for( const auto pt : m.points_in_radius( p, radius ) )
            {
                items.push_back( m.i_at( pt ) );
            }
            return items;
        } );

        DOC( "Moves an item from one position to another, preserving all item state including contents." );
        luna::set_fx( ut, "move_item_to", []( map & m, const tripoint & from, item * it,
        const tripoint & to ) -> void {
            detached_ptr<item> detached = m.i_rem( from, it );
            if( detached )
            {
                m.add_item_or_charges( to, std::move( detached ) );
            }
        } );

        // Static storage for items that need to survive map changes (e.g., teleportation)
        // Uses namespaces to allow different systems to have separate storage
        // Inner map uses monotonic keys so retrieval truly removes entries
        static std::map<std::string, std::map<int, detached_ptr<item>>> lua_item_storage;
        static int lua_item_storage_next_key = 0;

        DOC( "Removes an item from the map and stores it in a namespace. Returns a storage_key (like a coat check ticket), or -1 on failure. Optional namespace parameter (defaults to 'default'). Use retrieve_stored_item with the key to get it back. IMPORTANT: To avoid conflicts with other mods, always prefix your namespace with your mod name, e.g. 'mymod.bank' or 'mymod.bag_of_holding'." );
        luna::set_fx( ut, "store_item", []( map & m, const tripoint & from, item * it,
        sol::optional<std::string> ns ) -> int {
            std::string storage_ns = ns.value_or( "default" );
            detached_ptr<item> detached = m.i_rem( from, it );
            if( detached )
            {
                int key = lua_item_storage_next_key++;
                lua_item_storage[storage_ns][key] = std::move( detached );
                return key;
            }
            return -1;
        } );

        DOC( "Retrieves a stored item by its storage_key and places it at a position. The item is removed from storage. Optional namespace parameter (defaults to 'default')." );
        luna::set_fx( ut, "retrieve_stored_item", []( map & m, int storage_key, const tripoint & to,
        sol::optional<std::string> ns ) -> bool {
            std::string storage_ns = ns.value_or( "default" );
            auto ns_it = lua_item_storage.find( storage_ns );
            if( ns_it == lua_item_storage.end() )
            {
                return false;
            }
            std::map<int, detached_ptr<item>> &storage = ns_it->second;
            auto item_it = storage.find( storage_key );
            if( item_it == storage.end() || !item_it->second )
            {
                return false;
            }
            m.add_item_or_charges( to, std::move( item_it->second ) );
            storage.erase( item_it );
            return true;
        } );

        DOC( "Returns a table of item snapshots for all items in a namespace. Each entry has: storage_key (for retrieval), type_id (itype_id string), name (display name), count (charges). Optional namespace parameter (defaults to 'default')." );
        luna::set_fx( ut, "list_stored_items", []( map &, sol::this_state L,
        sol::optional<std::string> ns ) -> sol::table {
            sol::state_view lua( L );
            sol::table result = lua.create_table();
            std::string storage_ns = ns.value_or( "default" );

            auto ns_it = lua_item_storage.find( storage_ns );
            if( ns_it == lua_item_storage.end() )
            {
                return result;
            }

            const std::map<int, detached_ptr<item>> &storage = ns_it->second;
            int table_idx = 1;
            for( const auto &pair : storage )
            {
                if( pair.second )
                {
                    const item &it = *pair.second;
                    sol::table entry = lua.create_table();
                    entry["storage_key"] = pair.first;
                    entry["type_id"] = it.typeId().str();
                    entry["name"] = it.tname( 1, true );
                    entry["count"] = it.charges;
                    result[table_idx++] = entry;
                }
            }
            return result;
        } );

        DOC( "Clears all items from a specific namespace. Optional namespace parameter (defaults to 'default')." );
        luna::set_fx( ut, "clear_stored_items", []( map &,
        sol::optional<std::string> ns ) -> void {
            std::string storage_ns = ns.value_or( "default" );
            lua_item_storage[storage_ns].clear();
        } );

        DOC( "Clears all items from ALL namespaces." );
        luna::set_fx( ut, "clear_all_stored_items", []( map & ) -> void {
            lua_item_storage.clear();
        } );

        DOC( "Returns the number of items in a namespace. Optional namespace parameter (defaults to 'default')." );
        luna::set_fx( ut, "get_stored_item_count", []( map &,
        sol::optional<std::string> ns ) -> int {
            std::string storage_ns = ns.value_or( "default" );
            auto ns_it = lua_item_storage.find( storage_ns );
            if( ns_it == lua_item_storage.end() )
            {
                return 0;
            }
            return static_cast<int>( ns_it->second.size() );
        } );

        DOC( "Returns a list of all namespace names that have stored items." );
        luna::set_fx( ut, "get_storage_namespaces", []( map &, sol::this_state L ) -> sol::table {
            sol::state_view lua( L );
            sol::table result = lua.create_table();
            int idx = 1;
            for( const auto &pair : lua_item_storage )
            {
                if( !pair.second.empty() )
                {
                    result[idx++] = pair.first;
                }
            }
            return result;
        } );

        luna::set_fx( ut, "get_ter_at", sol::resolve<ter_id( const tripoint & )const>( &map::ter ) );
        luna::set_fx( ut, "set_ter_at",
                      sol::resolve<bool( const tripoint &, const ter_id & )>( &map::ter_set ) );

        luna::set_fx( ut, "get_furn_at", sol::resolve<furn_id( const tripoint & )const>( &map::furn ) );
        luna::set_fx( ut, "set_furn_at", []( map & m, const tripoint & p, const furn_id & id ) { m.furn_set( p, id ); } );

        luna::set_fx( ut, "has_field_at",
                      []( const map & m, const tripoint & p, const field_type_id & fid ) -> bool { return !!m.field_at( p ).find_field( fid ); } );
        luna::set_fx( ut, "get_field_int_at", &map::get_field_intensity );
        luna::set_fx( ut, "get_field_age_at", &map::get_field_age );
        luna::set_fx( ut, "mod_field_int_at", &map::mod_field_intensity );
        luna::set_fx( ut, "mod_field_age_at", &map::mod_field_age );
        luna::set_fx( ut, "set_field_int_at", &map::set_field_intensity );
        luna::set_fx( ut, "set_field_age_at", &map::set_field_age );
        luna::set_fx( ut, "add_field_at", []( map & m, const tripoint & p, const field_type_id & fid,
        int intensity, const time_duration & age ) -> bool {
            return m.add_field( p, fid, intensity, age );
        } );
        luna::set_fx( ut, "remove_field_at", &map::remove_field );
        luna::set_fx( ut, "get_trap_at", []( map & m, const tripoint & p ) -> trap_id { return m.tr_at( p ).loadid; } );
        DOC( "Set a trap at a position on the map. It can also replace existing trap, even with `trap_null`." );
        luna::set_fx( ut, "set_trap_at", &map::trap_set );
        DOC( "Disarms a trap using your skills and stats, with consequences depending on success or failure." );
        luna::set_fx( ut, "disarm_trap_at", &map::disarm_trap );
        DOC( "Simpler version of `set_trap_at` with `trap_null`." );
        luna::set_fx( ut, "remove_trap_at", &map::remove_trap );
    }

    // Register 'tinymap' class to be used in Lua
    {
        luna::new_usertype<tinymap>( lua, luna::bases<map>(), luna::no_constructor );
    }

    // Register 'item_stack' class to be used in Lua
#define UT_CLASS item_stack
    {
        DOC( "Iterate over this using pairs() for reading. Can also be indexed." );
        sol::usertype<item_stack> ut = luna::new_usertype<item_stack>( lua, luna::no_bases,
                                       luna::no_constructor );

        luna::set_fx( ut, sol::meta_function::pairs, item_stack_lua_pairs );
        luna::set_fx( ut, sol::meta_function::length, item_stack_lua_length );
        luna::set_fx( ut, sol::meta_function::index, item_stack_lua_index );

        DOC( "Modifying the stack while iterating may cause problems. This returns a frozen copy of the items in the stack for safe modification of the stack (eg. removing items while iterating)." );
        luna::set_fx( ut, "items", []( UT_CLASS & c ) {
            std::vector<item *> ret{};
            std::ranges::copy( c, std::back_inserter( ret ) );
            return ret;
        } );
        SET_FX( remove );
        luna::set_fx( ut, "insert", []( UT_CLASS & c, detached_ptr<item> &i ) {
            c.insert( std::move( i ) );
        } );
        SET_FX( clear );
        SET_FX_N( size, "count" );
        SET_FX( amount_can_fit );
        SET_FX( count_limit );
        SET_FX( free_volume );
        SET_FX( stored_volume );
        SET_FX( max_volume );
        SET_FX( move_all_to );
        SET_FX( only_item );
        SET_FX_T( stacks_with, item * ( const item & ) );
    }
#undef UT_CLASS

    // Register 'map_stack' class to be used in Lua
    {
        sol::usertype<map_stack> ut = luna::new_usertype<map_stack>( lua, luna::bases<item_stack>(),
                                      luna::no_constructor );

        luna::set_fx( ut, "as_item_stack", []( map_stack & ref ) -> item_stack& { return ref; } );

        luna::set_fx( ut, sol::meta_function::pairs, item_stack_lua_pairs );
        luna::set_fx( ut, sol::meta_function::length, item_stack_lua_length );
        luna::set_fx( ut, sol::meta_function::index, item_stack_lua_index );
    }
}

void cata::detail::reg_distribution_grid( sol::state &lua )
{
    {
        sol::usertype<distribution_grid> ut =
            luna::new_usertype<distribution_grid>(
                lua,
                luna::no_bases,
                luna::no_constructor
            );

        DOC( "Boolean argument controls recursive behavior" );
        luna::set_fx( ut, "get_resource", &distribution_grid::get_resource );
        DOC( "Boolean argument controls recursive behavior" );
        luna::set_fx( ut, "mod_resource", &distribution_grid::mod_resource );
    }

    {
        sol::usertype<distribution_grid_tracker> ut =
            luna::new_usertype<distribution_grid_tracker>(
                lua,
                luna::no_bases,
                luna::no_constructor
            );

        luna::set_fx( ut, "get_grid_at_abs_ms",
                      []( distribution_grid_tracker & tr, const tripoint & p ) -> distribution_grid& { return tr.grid_at( tripoint_abs_ms( p ) ); } );
    }

}
