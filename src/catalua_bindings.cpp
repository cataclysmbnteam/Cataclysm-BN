#ifdef LUA
#include "catalua_bindings.h"

#include "avatar.h"
#include "catalua_log.h"
#include "catalua_sol.h"
#include "catalua.h"
#include "character.h"
#include "color.h"
#include "creature.h"
#include "distribution_grid.h"
#include "enum_conversions.h"
#include "game.h"
#include "item.h"
#include "itype.h"
#include "map.h"
#include "messages.h"
#include "monster.h"
#include "npc.h"
#include "player.h"
#include "point.h"
#include "popup.h"
#include "string_id.h"
#include "ui.h"

static int deny_table_readonly( sol::this_state L )
{
    return luaL_error( L.lua_state(), "This table is read-only." );
}

static std::tuple<sol::object, sol::object, sol::nil_t>
reimpl_default_pairs( sol::this_state L, sol::table t )
{
    // TODO: check how broken this HACK is
    sol::state_view lua( L.lua_state() );
    // Forward to the table we're reading from
    return std::make_tuple( lua["next"], t[sol::meta_function::index], sol::nil );
}

sol::table make_readonly_table( sol::state &lua, sol::table read_from )
{
    sol::table ret = lua.create_table();

    read_from[sol::meta_function::index] = read_from;
    read_from[sol::meta_function::new_index] = deny_table_readonly;
    read_from[sol::meta_function::pairs] = reimpl_default_pairs;

    ret[sol::metatable_key] = read_from;
    return ret;
}

sol::table make_readonly_table( sol::state &lua, sol::table read_from,
                                const std::string &error_msg )
{
    sol::table ret = lua.create_table();

    const std::string &copy = error_msg;
    read_from[sol::meta_function::index] = read_from;
    read_from[sol::meta_function::new_index] = [copy]( sol::this_state L ) {
        return luaL_error( L.lua_state(), copy.c_str() );
    };
    read_from[sol::meta_function::pairs] = reimpl_default_pairs;

    ret[sol::metatable_key] = read_from;
    return ret;
}

static std::string fmt_lua_va( sol::variadic_args va )
{
    lua_State *L = va.lua_state();
    sol::state_view lua( L );

    std::string msg;
    for( auto it : va ) {
        msg += lua["tostring"]( it );
    }

    return msg;
}

static void lua_log_info_impl( sol::variadic_args va )
{
    std::string msg = fmt_lua_va( va );

    DebugLog( DL::Info, DC::Lua ) << msg;
    cata::get_lua_log_instance().add( cata::LuaLogLevel::Info, std::move( msg ) );
}

static void lua_log_warn_impl( sol::variadic_args va )
{
    std::string msg = fmt_lua_va( va );

    DebugLog( DL::Warn, DC::Lua ) << msg;
    cata::get_lua_log_instance().add( cata::LuaLogLevel::Warn, std::move( msg ) );
}

static void lua_log_error_impl( sol::variadic_args va )
{
    std::string msg = fmt_lua_va( va );

    DebugLog( DL::Error, DC::Lua ) << msg;
    cata::get_lua_log_instance().add( cata::LuaLogLevel::Error, std::move( msg ) );
}

static void lua_debugmsg_impl( sol::variadic_args va )
{
    std::string msg = fmt_lua_va( va );

    debugmsg( "%s", msg );
    cata::get_lua_log_instance().add( cata::LuaLogLevel::DebugMsg, std::move( msg ) );
}

void reg_debug_logging( sol::state &lua )
{
    // Override global output print function to write into debug.log
    lua.globals()["print"] = lua_log_info_impl;
    // Explicit logging
    lua.globals()["log_info"] = lua_log_info_impl;
    lua.globals()["log_warn"] = lua_log_warn_impl;
    lua.globals()["log_error"] = lua_log_error_impl;
    // Debug message
    lua.globals()["debugmsg"] = lua_debugmsg_impl;
    // Log manipulation
    lua.globals()["clear_lua_log"] = []() {
        cata::get_lua_log_instance().clear();
    };
    lua.globals()["set_log_capacity"] = []( int v ) {
        cata::get_lua_log_instance().set_log_capacity( v );
    };
    lua.globals()["reload_lua_code"] = &cata::reload_lua_code;
    lua.globals()["save_game"] = []() -> bool {
        return g->save();
    };
}


template<typename T>
void reg_string_id( sol::state &lua, const char *name )
{
    // Register string_id class under given name
    sol::usertype<T> ut =
        lua.new_usertype<T>(
            name,
            sol::constructors <
            T(),
            T( const T & ),
            T( const char * ),
            T( std::string )
            > ()
        );

    ut["is_null"] = &T::is_null;
    ut["is_empty"] = &T::is_empty;
    ut["is_valid"] = &T::is_valid;
    ut["str"] = &T::c_str;
    ut["NULL_ID"] = &T::NULL_ID;
    ut[sol::meta_function::to_string] = [name]( const T & id ) -> std::string {
        return string_format( "%s[%s]", name, id.c_str() );
    };
}

template<typename E>
void reg_enum( sol::state &lua, const std::string &name )
{
    // Sol2 has new_enum<E>(...) function, but it needs to know all value-string
    // pairs at compile time, so we can't use it with io::enum_to_string.
    //
    // As such, hack it by creating read-only table.

    sol::table et = lua.create_table();

    using Int = std::underlying_type_t<E>;
    constexpr Int max = static_cast<Int>( enum_traits<E>::last );

    for( Int i = 0; i < max; ++i ) {
        E e = static_cast<E>( i );
        std::string key = io::enum_to_string<E>( e );
        et[key] = e;
    }

    et = make_readonly_table( lua, et, string_format( "Tried to modify enum %s.", name ) );
    lua.globals()[name] = et;
}

static void reg_colors( sol::state &lua, const std::string &name )
{
    // Colors are not enums, we have to do them manually
    sol::table et = lua.create_table();

    using Int = std::underlying_type_t<color_id>;
    constexpr Int max = static_cast<Int>( color_id::num_colors );

    for( Int i = 0; i < max; ++i ) {
        color_id e = static_cast<color_id>( i );
        std::string key = get_all_colors().id_to_name( e );
        et[key] = e;
    }

    et = make_readonly_table( lua, et, "Tried to modify color table." );
    lua.globals()[name] = et;
}

namespace sol
{
template <>
struct is_container<item_stack> : std::false_type {};
template <>
struct is_container<map_stack> : std::false_type {};
} // namespace sol

struct item_stack_lua_it_state {
    using it_t = item_stack::iterator;
    it_t it;
    it_t last;

    item_stack_lua_it_state( item_stack &stk )
        : it( stk.begin() ), last( stk.end() ) {
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
    auto &it = it_state.it;
    if( it == it_state.last ) {
        // return nil to signify that
        // there's nothing more to work with.
        return std::make_tuple( sol::object( sol::lua_nil ),
                                sol::object( sol::lua_nil ) );
    }
    item *elem = &*it;
    // 2 values are returned (pushed onto the stack):
    // the key and the value
    // the state is left alone
    auto r = std::make_tuple(
                 sol::object( l, sol::in_place, it ),
                 sol::object( l, sol::in_place, elem ) );
    // the iterator must be moved forward one before we return
    std::advance( it, 1 );
    return r;
}

static auto item_stack_lua_pairs( item_stack &stk )
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

static void add_msg_lua( game_message_type t, sol::variadic_args va )
{
    if( va.size() == 0 ) {
        // Nothing to print
        return;
    }

    std::string msg = fmt_lua_va( va );
    add_msg( t, msg );
}

void reg_game_bindings( sol::state &lua )
{
    // Register creature class family to be used in Lua.
    {
        // Specifying base classes here allows us to pass derived classes
        // from Lua to C++ functions that expect base class.
        sol::usertype<Creature> ut =
            lua.new_usertype<Creature>(
                "Creature",
                sol::no_constructor
            );

        // TODO: typesafe coords
        ut["get_pos_ms"] = &Creature::pos;
    }

    {
        lua.new_usertype<monster>(
            "Monster",
            sol::no_constructor,
            sol::base_classes, sol::bases<Creature>()
        );
        lua.new_usertype<Character>(
            "Character",
            sol::no_constructor,
            sol::base_classes, sol::bases<Creature>()
        );
        lua.new_usertype<player>(
            "Player",
            sol::no_constructor,
            sol::base_classes, sol::bases<Character, Creature>()
        );
        lua.new_usertype<npc>(
            "Npc",
            sol::no_constructor,
            sol::base_classes, sol::bases<player, Character, Creature>()
        );
        lua.new_usertype<avatar>(
            "Avatar",
            sol::no_constructor,
            sol::base_classes, sol::bases<player, Character, Creature>()
        );
    }

    // Register 'point' class to be used in Lua
    {
        sol::usertype<point> ut =
            lua.new_usertype<point>(
                // Class name in Lua
                "Point",
                // Constructors
                sol::constructors <
                point(),
                point( const point & ),
                point( int, int )
                > ()
            );

        // Members
        ut["x"] = &point::x;
        ut["y"] = &point::y;

        // Methods
        ut["abs"] = &point::abs;
        ut["rotate"] = &point::rotate;

        // To string
        // We're using Lua meta function here to make it work seamlessly with native Lua tostring()
        ut[sol::meta_function::to_string] = &point::to_string;

        // Equality operator
        // It's defined as inline friend function inside point class, we can't access it and so have to improvise
        ut[ sol::meta_function::equal_to ] = []( const point & a, const point & b ) {
            return a == b;
        };

        // Less-then operator
        // Same deal as with equality operator
        ut[sol::meta_function::less_than] = []( const point & a, const point & b ) {
            return a < b;
        };

        // Arithmetic operators
        // point + point
        ut[ sol::meta_function::addition ] = &point::operator+;
        // point - point
        // sol::resolve here makes it possible to specify which overload to use
        ut[ sol::meta_function::subtraction ] = sol::resolve< point( const point & ) const >
                                                ( &point::operator- );
        // point * int
        ut[ sol::meta_function::multiplication ] = &point::operator*;
        // point / float
        ut[ sol::meta_function::division ] = &point::operator/;
        // point / int
        ut[ sol::meta_function::floor_division ] = &point::operator/;
        // -point
        // sol::resolve here makes it possible to specify which overload to use
        ut[ sol::meta_function::unary_minus ] = sol::resolve< point() const >( &point::operator- );
    }

    // Register 'tripoint' class to be used in Lua
    {
        sol::usertype<tripoint> ut =
            lua.new_usertype<tripoint>(
                // Class name in Lua
                "Tripoint",
                // Constructors
                sol::constructors <
                tripoint(),
                tripoint( const point &, int ),
                tripoint( const tripoint & ),
                tripoint( int, int, int )
                > ()
            );

        // Members
        ut["x"] = &tripoint::x;
        ut["y"] = &tripoint::y;
        ut["z"] = &tripoint::z;

        // Methods
        ut["abs"] = &tripoint::abs;
        ut["xy"] = &tripoint::xy;
        ut["rotate_2d"] = &tripoint::rotate_2d;

        // To string
        // We're using Lua meta function here to make it work seamlessly with native Lua tostring()
        ut[sol::meta_function::to_string] = &tripoint::to_string;

        // Equality operator
        // It's defined as inline friend function inside point class, we can't access it and so have to improvise
        ut[ sol::meta_function::equal_to ] = []( const tripoint & a, const tripoint & b ) {
            return a == b;
        };

        // Less-then operator
        // Same deal as with equality operator
        ut[sol::meta_function::less_than] = []( const tripoint & a, const tripoint & b ) {
            return a < b;
        };

        // Arithmetic operators
        // tripoint + tripoint (overload 1)
        // tripoint + point (overload 2)
        ut[ sol::meta_function::addition ] = sol::overload(
                sol::resolve< tripoint( const tripoint & ) const > ( &tripoint::operator+ ),
                sol::resolve< tripoint( const point & ) const > ( &tripoint::operator+ )
                                             );
        // tripoint - tripoint (overload 1)
        // tripoint - point (overload 2)
        ut[ sol::meta_function::subtraction ] = sol::overload(
                sol::resolve< tripoint( const tripoint & ) const > ( &tripoint::operator- ),
                sol::resolve< tripoint( const point & ) const > ( &tripoint::operator- )
                                                );
        // tripoint * int
        ut[ sol::meta_function::multiplication ] = &tripoint::operator*;
        // tripoint / float
        ut[ sol::meta_function::division ] = &tripoint::operator/;
        // tripoint / int
        ut[ sol::meta_function::floor_division ] = &tripoint::operator/;
        // -tripoint
        // sol::resolve here makes it possible to specify which overload to use
        ut[ sol::meta_function::unary_minus ] = sol::resolve< tripoint() const >( &tripoint::operator- );
    }

    // Register 'item' class to be used in Lua
    {
        sol::usertype<item> ut =
            lua.new_usertype<item>(
                // Class name in Lua
                "Item",
                // Constructors
                sol::no_constructor
            );

        ut["get_type"] = &item::typeId;

        ut["has_var"] = &item::has_var;
        ut["erase_var"] = &item::erase_var;
        ut["clear_vars"] = &item::clear_vars;

        ut["get_var_str"] = sol::resolve<std::string( const std::string &, const std::string & ) const>
                            ( &item::get_var );
        ut["get_var_num"] = sol::resolve<double( const std::string &, double ) const>( &item::get_var );
        ut["get_var_tri"] = sol::resolve<tripoint( const std::string &, const tripoint & ) const>
                            ( &item::get_var );

        ut["set_var_str"] = sol::resolve<void( const std::string &, const std::string & )>
                            ( &item::set_var );
        ut["set_var_num"] = sol::resolve<void( const std::string &, double )>( &item::set_var );
        ut["set_var_tri"] = sol::resolve<void( const std::string &, const tripoint & )>( &item::set_var );
    }

    // Register 'map' class to be used in Lua
    {
        sol::usertype<map> ut =
            lua.new_usertype<map>(
                // Class name in Lua
                "Map",
                // Constructors
                sol::no_constructor
            );

        ut["get_abs_ms"] = sol::resolve<tripoint( const tripoint & ) const>( &map::getabs );
        ut["get_local_ms"] = sol::resolve<tripoint( const tripoint & ) const>( &map::getlocal );

        ut["get_map_size_in_submaps"] = &map::getmapsize;
        ut["get_map_size"] = []( const map & m ) -> int {
            return m.getmapsize() * SEEX;
        };

        ut["has_items_at"] = &map::has_items;
        ut["get_items_at"] = []( map & m, const tripoint & p ) -> std::unique_ptr<map_stack> {
            return std::make_unique<map_stack>( m.i_at( p ) );
        };

        // TODO: make it work with int_ids
        ut["get_ter_at"] = []( const map & m, const tripoint & p ) {
            return m.ter( p ).id();
        };
        ut["set_ter_at"] = []( map & m, const tripoint & p, const ter_str_id & id ) {
            m.ter_set( p, id.id() );
        };

        ut["get_furn_at"] = []( const map & m, const tripoint & p ) {
            return m.furn( p ).id();
        };
        ut["set_furn_at"] = []( map & m, const tripoint & p, const furn_str_id & id ) {
            m.furn_set( p, id.id() );
        };
    }

    // Register 'tinymap' class to be used in Lua
    {
        // Specifying base classes here allows us to pass derived classes
        // from Lua to C++ functions that expect base class.
        lua.new_usertype<tinymap>(
            "Tinymap",
            sol::no_constructor,
            sol::base_classes, sol::bases<map>()
        );
    }

    // Register 'item_stack' class to be used in Lua
    {
        sol::usertype<item_stack> ut =
            lua.new_usertype<item_stack>(
                "ItemStack",
                sol::no_constructor
            );

        ut[sol::meta_function::pairs] = item_stack_lua_pairs;
    }

    // Register 'map_stack' class to be used in Lua
    {
        // Specifying base classes here allows us to pass derived classes
        // from Lua to C++ functions that expect base class.
        sol::usertype<map_stack> ut =
            lua.new_usertype<map_stack>(
                "MapStack",
                sol::no_constructor,
                sol::base_classes, sol::bases<item_stack>()
            );

        ut["as_item_stack"] = []( map_stack & ref ) -> item_stack& {
            return ref;
        };
    }

    {
        sol::usertype<distribution_grid> ut =
            lua.new_usertype<distribution_grid>(
                "DistributionGrid",
                sol::no_constructor
            );

        ut["get_resource"] = &distribution_grid::get_resource;
        ut["mod_resource"] = &distribution_grid::mod_resource;
    }

    {
        sol::usertype<distribution_grid_tracker> ut =
            lua.new_usertype<distribution_grid_tracker>(
                "DistributionGridTracker",
                sol::no_constructor
            );

        ut["get_grid_at_abs_ms"] = []( distribution_grid_tracker & tr, const tripoint & p )
        -> distribution_grid& {
            return tr.grid_at( tripoint_abs_ms( p ) );
        };
    }

    // Register some global functions to be used in Lua
    {
        // Global function that returns global avatar instance
        lua["get_avatar"] = &get_avatar;
        // Global function that returns global map instance
        lua["get_map"] = &get_map;
        // Global function that returns global grid tracker
        lua["get_distribution_grid_tracker"] = &get_distribution_grid_tracker;
        // We can use both lambdas and static functions
        lua["get_character_name"] = []( const Character & you ) -> std::string {
            return you.name;
        };
        lua["add_msg"] = sol::overload(
                             add_msg_lua,
        []( sol::variadic_args va ) {
            add_msg_lua( game_message_type::m_neutral, va );
        }
                         );
    }

    // Register coord manipulation funcs
    // TODO: typesafe coords API
    {
        sol::table t = lua.create_table();
        lua.globals()["coords"] = t;

        t["ms_to_sm"] = []( const tripoint & raw ) -> std::tuple<tripoint, point> {
            tripoint_rel_ms fine( raw );
            tripoint_rel_sm rough;
            point_sm_ms remain;
            std::tie( rough, remain ) = coords::project_remain<coords::sm>( fine );
            return std::make_pair( rough.raw(), remain.raw() );
        };
        t["ms_to_omt"] = []( const tripoint & raw ) -> std::tuple<tripoint, point> {
            tripoint_rel_ms fine( raw );
            tripoint_rel_omt rough;
            point_omt_ms remain;
            std::tie( rough, remain ) = coords::project_remain<coords::omt>( fine );
            return std::make_pair( rough.raw(), remain.raw() );
        };
        t["ms_to_om"] = []( const tripoint & raw ) -> std::tuple<point, tripoint> {
            tripoint_rel_ms fine( raw );
            point_rel_om rough;
            coords::coord_point<tripoint, coords::origin::overmap, coords::ms> remain;
            std::tie( rough, remain ) = coords::project_remain<coords::om>( fine );
            return std::make_pair( rough.raw(), remain.raw() );
        };

        t["sm_to_ms"] = []( const tripoint & raw_rough,
        sol::optional<const point &> raw_remain ) -> tripoint {
            tripoint_rel_sm rough( raw_rough );
            point_sm_ms remain( raw_remain ? *raw_remain : point_zero );
            tripoint_rel_ms fine = coords::project_combine( rough, remain );
            return fine.raw();
        };
        t["omt_to_ms"] = []( const tripoint & raw_rough,
        sol::optional<const point &> raw_remain ) -> tripoint {
            tripoint_rel_omt rough( raw_rough );
            point_omt_ms remain( raw_remain ? *raw_remain : point_zero );
            tripoint_rel_ms fine = coords::project_combine( rough, remain );
            return fine.raw();
        };
        t["om_to_ms"] = []( const point & raw_rough,
        sol::optional<const tripoint &> raw_remain ) -> tripoint {
            point_rel_om rough( raw_rough );
            coords::coord_point<tripoint, coords::origin::overmap, coords::ms> remain(
                raw_remain ? *raw_remain : tripoint_zero
            );
            tripoint_rel_ms fine = coords::project_combine( rough, remain );
            return fine.raw();
        };

        t["rl_dist"] = sol::overload(
                           sol::resolve<int( const tripoint &, const tripoint & )>( rl_dist ),
                           sol::resolve<int( const point &, const point & )>( rl_dist )
                       );
        t["trig_dist"] = sol::overload(
                             sol::resolve<float( const tripoint &, const tripoint & )>( trig_dist ),
                             sol::resolve<float( const point &, const point & )>( trig_dist )
                         );
        t["square_dist"] = sol::overload(
                               sol::resolve<int( const tripoint &, const tripoint & )>( square_dist ),
                               sol::resolve<int( const point &, const point & )>( square_dist )
                           );
    }

    reg_string_id<faction_id>( lua, "FactionId" );
    reg_string_id<itype_id>( lua, "ItypeId" );
    reg_string_id<ter_str_id>( lua, "TerId" );
    reg_string_id<furn_str_id>( lua, "FurnId" );

    reg_enum<game_message_type>( lua, "MsgType" );

    reg_colors( lua, "Color" );

    // Register constants
    {
        sol::table t = lua.create_table();
        lua.globals()["const"] = t;

        t["OM_OMT_SIZE"] = OMAPX;
        t["OM_SM_SIZE"] = OMAPX * 2;
        t["OM_MS_SIZE"] = OMAPX * 2 * SEEX;
        t["OMT_SM_SIZE"] = 2;
        t["OMT_MS_SIZE"] = SEEX * 2;
        t["SM_MS_SIZE"] = SEEX;
    }

    // Register uilist
    {
        sol::usertype<uilist> ut =
            lua.new_usertype<uilist>(
                // Class name in Lua
                "UiList",
                // Constructors
                sol::constructors <
                uilist()
                > ()
            );
        ut["title"] = []( uilist & ui, const std::string & text ) {
            ui.title = text;
        };
        ut["add"] = []( uilist & ui, int retval, const std::string & text ) {
            ui.addentry( retval, true, MENU_AUTOASSIGN, text );
        };
        ut["query"] = []( uilist & ui ) {
            ui.query();
            return ui.ret;
        };
    }

    // Register popup
    {
        sol::usertype<query_popup> ut =
            lua.new_usertype<query_popup>(
                // Class name in Lua
                "QueryPopup",
                // Constructors
                sol::constructors <
                query_popup()
                > ()
            );
        ut["message"] = []( query_popup & popup, sol::variadic_args va ) {
            popup.message( "%s", fmt_lua_va( va ) );
        };
        ut["message_color"] = []( query_popup & popup, color_id col ) {
            popup.default_color( get_all_colors().get( col ) );
        };
        ut["allow_any_key"] = []( query_popup & popup, bool val ) {
            popup.allow_anykey( val );
        };
        ut["query"] = []( query_popup & popup ) {
            return popup.query().action;
        };
    }
}

#endif
