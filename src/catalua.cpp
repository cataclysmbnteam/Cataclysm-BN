#include "catalua.h"

#include "debug.h"

constexpr int LUA_API_VERSION = 1;

#ifndef LUA

#include "popup.h"

namespace cata
{

// It's a dud
struct lua_state {
    lua_state() = default;
    ~lua_state() = default;
};

bool has_lua()
{
    return false;
}

void startup_lua_test()
{
    // Nothing to do here
}

void show_lua_console()
{
    query_popup()
    .default_color( c_red )
    .allow_anykey( true )
    .message( "%s", "Can't open Lua console:\nthe game was compiled without Lua support." )
    .query();
}

void reload_lua_code()
{
    query_popup()
    .default_color( c_red )
    .allow_anykey( true )
    .message( "%s", "Can't reload Lua code:\nthe game was compiled without Lua support." )
    .query();
}

std::unique_ptr<lua_state, lua_state_deleter> make_wrapped_state()
{
    return std::unique_ptr<lua_state, lua_state_deleter>(
               new lua_state{}, lua_state_deleter{}
           );
}

void set_mod_list( lua_state &, const std::vector<mod_id> & ) {}
void set_mod_being_loaded( lua_state &, const mod_id & ) {}
void clear_mod_being_loaded( lua_state & ) {}
void run_mod_preload_script( lua_state &, const mod_id & ) {}
void run_mod_finalize_script( lua_state &, const mod_id & ) {}
void run_mod_main_script( lua_state &, const mod_id & ) {}
void reg_lua_iuse_actors( lua_state &, Item_factory & ) {}

template<typename... Args>
void run_hooks( Args &&... ) {}

} // namespace cata

#else // LUA

#include "catalua_sol.h"

#include "avatar.h"
#include "catalua_bindings.h"
#include "catalua_console.h"
#include "catalua_impl.h"
#include "catalua_iuse_actor.h"
#include "filesystem.h"
#include "init.h"
#include "item_factory.h"
#include "map.h"
#include "mod_manager.h"
#include "path_info.h"
#include "point.h"
#include "worldfactory.h"

namespace cata
{

bool has_lua()
{
    return true;
}

void startup_lua_test()
{
    sol::state lua = make_lua_state();
    std::string lua_startup_script = PATH_INFO::datadir() + "raw/on_game_start.lua";
    try {
        run_lua_script( lua, lua_startup_script );
    } catch( std::runtime_error &e ) {
        debugmsg( "%s", e.what() );
    }
}

void show_lua_console()
{
    cata::show_lua_console_impl();
}

void reload_lua_code()
{
    cata::lua_state &state = *DynamicDataLoader::get_instance().lua;
    const auto &packs = world_generator->active_world->active_mod_order;
    try {
        init::load_main_lua_scripts( state, packs );
    } catch( std::runtime_error &e ) {
        debugmsg( "%s", e.what() );
    }
    clear_mod_being_loaded( state );
}

std::unique_ptr<lua_state, lua_state_deleter> make_wrapped_state()
{
    std::unique_ptr<lua_state, lua_state_deleter> ret( new lua_state{ make_lua_state() },
            lua_state_deleter{} );

    sol::state &lua = ret->lua;

    sol::table game_table = lua.create_table();
    lua.globals()["game"] = game_table;

    return ret;
}

void set_mod_list( lua_state &state, const std::vector<mod_id> &modlist )
{
    sol::state &lua = state.lua;

    sol::table active_mods = lua.create_table();
    sol::table mod_runtime = lua.create_table();

    for( size_t i = 0; i < modlist.size(); i++ ) {
        active_mods[ i + 1 ] = modlist[i].str();
        mod_runtime[ modlist[i].str() ] = lua.create_table();
    }

    active_mods = make_readonly_table( lua, active_mods );
    mod_runtime = make_readonly_table( lua, mod_runtime );

    sol::table gt = lua.globals()["game"];

    gt["active_mods"] = active_mods;
    gt["mod_runtime"] = mod_runtime;

    gt["on_load_hooks"] = lua.create_table();
    gt["iuse_functions"] = lua.create_table();
    gt["on_mapgen_postprocess_hooks"] = lua.create_table();
}

void set_mod_being_loaded( lua_state &state, const mod_id &mod )
{
    sol::state &lua = state.lua;
    lua.globals()["game"]["current_mod"] = mod.str();
}

void clear_mod_being_loaded( lua_state &state )
{
    sol::state &lua = state.lua;
    lua.globals()["game"]["current_mod"] = sol::nil;
}

void run_mod_preload_script( lua_state &state, const mod_id &mod )
{
    std::string script_path = mod->path + "/" + "preload.lua";

    if( !file_exist( script_path ) ) {
        return;
    }

    run_lua_script( state.lua, script_path );
}

void run_mod_finalize_script( lua_state &state, const mod_id &mod )
{
    std::string script_path = mod->path + "/" + "finalize.lua";

    if( !file_exist( script_path ) ) {
        return;
    }

    run_lua_script( state.lua, script_path );
}

void run_mod_main_script( lua_state &state, const mod_id &mod )
{
    std::string script_path = mod->path + "/" + "main.lua";

    if( !file_exist( script_path ) ) {
        return;
    }

    run_lua_script( state.lua, script_path );
}

template<typename... Args>
void run_hooks( lua_state &state, std::string_view hooks_table, Args &&...args )
{
    sol::state &lua = state.lua;
    sol::table hooks = lua.globals()["game"][hooks_table];
    for( auto &ref : hooks ) {
        int idx = -1;
        try {
            idx = ref.first.as<int>();
            sol::protected_function func = ref.second;
            sol::protected_function_result res = func( std::forward<Args>( args )... );
            check_func_result( res );
        } catch( std::runtime_error &e ) {
            debugmsg( "Failed to run %s[%d]: %s", hooks_table, idx, e.what() );
            break;
        }
    }
}

void reg_lua_iuse_actors( lua_state &state, Item_factory &ifactory )
{
    sol::state &lua = state.lua;

    sol::table funcs = lua.globals()["game"]["iuse_functions"];

    for( auto &ref : funcs ) {
        std::string key;
        try {
            key = ref.first.as<std::string>();
            sol::protected_function func = ref.second;
            ifactory.add_actor( std::make_unique<lua_iuse_actor>( key, std::move( func ) ) );
        } catch( std::runtime_error &e ) {
            debugmsg( "Failed to extract iuse_functions k='%s': %s", key, e.what() );
            break;
        }
    }
}

} // namespace cata

#endif // LUA

namespace cata
{

int get_lua_api_version()
{
    return LUA_API_VERSION;
}

void lua_state_deleter::operator()( lua_state *state ) const
{
    delete state;
}

void run_on_load_hooks( lua_state &state )
{
    run_hooks( state, "on_load_hooks" );
}

void run_on_mapgen_postprocess_hooks( lua_state &state, map &m, const tripoint &p,
                                      const time_point &when )
{
    run_hooks( state, "on_mapgen_postprocess_hooks", m, p, when );
}

} // namespace cata
