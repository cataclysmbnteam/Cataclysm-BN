#ifdef LUA
#include "catalua_bindings.h"

#include "avatar.h"
#include "catalua_sol.h"
#include "character.h"
#include "creature.h"
#include "monster.h"
#include "npc.h"
#include "player.h"
#include "point.h"

static int deny_table_readonly( sol::this_state L )
{
    return luaL_error( L.lua_state(), "This table is read-only." );
}

void make_table_readonly( sol::state &lua, sol::table &t )
{
    sol::table meta;
    if( t[sol::metatable_key].valid() ) {
        meta = t[sol::metatable_key];
    } else {
        meta = lua.create_table();
    }
    meta[sol::meta_function::new_index] = deny_table_readonly;
    meta[sol::meta_function::index] = meta;
    t[sol::metatable_key] = meta;
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
}

static void lua_log_warn_impl( sol::variadic_args va )
{
    std::string msg = fmt_lua_va( va );

    DebugLog( DL::Warn, DC::Lua ) << msg;
}

static void lua_log_error_impl( sol::variadic_args va )
{
    std::string msg = fmt_lua_va( va );

    DebugLog( DL::Error, DC::Lua ) << msg;
}

static void lua_debugmsg_impl( sol::variadic_args va )
{
    std::string msg = fmt_lua_va( va );

    debugmsg( "%s", msg );
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
}

void reg_game_bindings( sol::state &lua )
{
    // Register creature class family to be used in Lua.
    {
        // Specifying base classes here allows us to pass derived classes
        // from Lua to C++ functions that expect base class.
        lua.new_usertype<Creature>(
            "Creature",
            sol::no_constructor
        );
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

    // Register some global functions to be used in Lua
    {
        // Global function that returns global avatar instance
        lua["get_avatar"] = &get_avatar;
        // We can use both lambdas and static functions
        lua["get_character_name"] = []( const Character & you ) -> std::string {
            return you.name;
        };
    }
}

#endif
