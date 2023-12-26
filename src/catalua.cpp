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

std::string get_lapi_version_string()
{
    return "<none>";
}

void startup_lua_test()
{
    // Nothing to do here
}

bool generate_lua_docs()
{
    // Nothing to do here
    return false;
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

void debug_write_lua_backtrace( std::ostream &/*out*/ )
{
    // Nothing to do here
}

bool save_world_lua_state( const std::string & )
{
    return true;
}

bool load_world_lua_state( const std::string & )
{
    return true;
}

std::unique_ptr<lua_state, lua_state_deleter> make_wrapped_state()
{
    return std::unique_ptr<lua_state, lua_state_deleter>(
               new lua_state{}, lua_state_deleter{}
           );
}

void init_global_state_tables( lua_state &, const std::vector<mod_id> & ) {}
void set_mod_being_loaded( lua_state &, const mod_id & ) {}
void clear_mod_being_loaded( lua_state & ) {}
void run_mod_preload_script( lua_state &, const mod_id & ) {}
void run_mod_finalize_script( lua_state &, const mod_id & ) {}
void run_mod_main_script( lua_state &, const mod_id & ) {}
void reg_lua_iuse_actors( lua_state &, Item_factory & ) {}

template<typename... Args>
void run_hooks( Args &&... ) {}

void run_on_every_x_hooks( lua_state & ) {}

} // namespace cata

#else // LUA

#include "catalua_sol.h"

#include "avatar.h"
#include "catalua_console.h"
#include "catalua_impl.h"
#include "catalua_iuse_actor.h"
#include "catalua_readonly.h"
#include "catalua_serde.h"
#include "filesystem.h"
#include "fstream_utils.h"
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

std::string get_lapi_version_string()
{
    return string_format( "%d", get_lua_api_version() );
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

bool generate_lua_docs()
{
    sol::state lua = make_lua_state();
    lua.globals()["doc_gen_func"] = lua.create_table();
    std::string lua_doc_script = PATH_INFO::datadir() + "raw/generate_docs.lua";
    try {
        run_lua_script( lua, lua_doc_script );
        sol::protected_function doc_gen_func = lua["doc_gen_func"]["impl"];
        sol::protected_function_result res = doc_gen_func();
        check_func_result( res );
        std::string ret = res;
        write_to_file( PATH_INFO::lua_doc_output(), [&]( std::ostream & s ) {
            s << ret;
        } );
    } catch( std::runtime_error &e ) {
        cata_printf( "%s\n", e.what() );
        return false;
    }
    return true;
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

void debug_write_lua_backtrace( std::ostream &out )
{
    cata::lua_state *state = DynamicDataLoader::get_instance().lua.get();
    if( !state ) {
        return;
    }
    sol::state container;

    luaL_traceback( container.lua_state(), state->lua.lua_state(), "=== Lua backtrace report ===", 0 );

    std::string data = sol::stack::pop<std::string>( container );
    out << data << '\n';
}

static sol::table get_mod_storage_table( lua_state &state )
{
    return state.lua.globals()["game"]["cata_internal"]["mod_storage"];
}

bool save_world_lua_state( const std::string &path )
{
    lua_state &state = *DynamicDataLoader::get_instance().lua;

    const mod_management::t_mod_list &mods = world_generator->active_world->active_mod_order;
    sol::table t = get_mod_storage_table( state );
    run_on_game_save_hooks( state );
    bool ret = write_to_file( path, [&]( std::ostream & stream ) {
        JsonOut jsout( stream );
        jsout.start_object();
        for( const mod_id &mod : mods ) {
            if( !mod.is_valid() ) {
                // The mod is missing from installation
                continue;
            }
            jsout.member( mod.str() );
            serialize_lua_table( t[mod.str()], jsout );
        }
        jsout.end_object();
    }, "world_lua_state" );
    return ret;
}

bool load_world_lua_state( const std::string &path )
{
    lua_state &state = *DynamicDataLoader::get_instance().lua;
    const mod_management::t_mod_list &mods = world_generator->active_world->active_mod_order;
    sol::table t = get_mod_storage_table( state );

    bool ret = read_from_file_optional( path, [&]( std::istream & stream ) {
        JsonIn jsin( stream );
        JsonObject jsobj = jsin.get_object();

        for( const mod_id &mod : mods ) {
            if( !jsobj.has_object( mod.str() ) ) {
                // Mod could have been added to existing save
                continue;
            }
            if( !mod.is_valid() ) {
                // Trying to load without the mod
                continue;
            }
            JsonObject mod_obj = jsobj.get_object( mod.str() );
            deserialize_lua_table( t[mod.str()], mod_obj );
        }
    } );

    run_on_game_load_hooks( state );
    return ret;
}

std::unique_ptr<lua_state, lua_state_deleter> make_wrapped_state()
{
    std::unique_ptr<lua_state, lua_state_deleter> ret(
        new lua_state{ make_lua_state() },
        lua_state_deleter{}
    );

    sol::state &lua = ret->lua;

    sol::table game_table = lua.create_table();
    lua.globals()["game"] = game_table;

    return ret;
}

void init_global_state_tables( lua_state &state, const std::vector<mod_id> &modlist )
{
    sol::state &lua = state.lua;

    sol::table active_mods = lua.create_table();
    sol::table mod_runtime = lua.create_table();
    sol::table mod_storage = lua.create_table();
    sol::table hooks = lua.create_table();

    for( size_t i = 0; i < modlist.size(); i++ ) {
        active_mods[ i + 1 ] = modlist[i].str();
        mod_runtime[ modlist[i].str() ] = lua.create_table();
        mod_storage[ modlist[i].str() ] = lua.create_table();
    }

    // Main game data table
    sol::table gt = lua.globals()["game"];

    // Internal table that bypasses read-only facades
    sol::table it = lua.create_table();
    gt["cata_internal"] = it;
    it["active_mods"] = active_mods;
    it["mod_runtime"] = mod_runtime;
    it["mod_storage"] = mod_storage;
    it["on_every_x_hooks"] = std::vector<cata::on_every_x_hooks>();
    gt["hooks"] = hooks;

    // Runtime infrastructure
    gt["active_mods"] = make_readonly_table( lua, active_mods );
    gt["mod_runtime"] = make_readonly_table( lua, mod_runtime );
    gt["mod_storage"] = make_readonly_table( lua, mod_storage );
    gt["hooks"] = make_readonly_table( lua, hooks );

    // iuse functions
    gt["iuse_functions"] = lua.create_table();

    // hooks
    hooks["on_game_load"] = lua.create_table();
    hooks["on_game_save"] = lua.create_table();
    hooks["on_mapgen_postprocess"] = lua.create_table();
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
    sol::table hooks = lua.globals()["game"]["hooks"][hooks_table];
    for( auto &ref : hooks ) {
        int idx = -1;
        try {
            idx = ref.first.as<int>();
            sol::protected_function func = ref.second;
            sol::protected_function_result res = func( std::forward<Args>( args )... );
            check_func_result( res );
        } catch( std::runtime_error &e ) {
            debugmsg( "Failed to run hook %s[%d]: %s", hooks_table, idx, e.what() );
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

void run_on_every_x_hooks( lua_state &state )
{
    std::vector<cata::on_every_x_hooks> &master_table =
        state.lua["game"]["cata_internal"]["on_every_x_hooks"];
    for( const auto &entry : master_table ) {
        if( calendar::once_every( entry.interval ) ) {
            for( auto &func : entry.functions ) {
                try {
                    sol::protected_function_result res = func();
                    check_func_result( res );
                } catch( std::runtime_error &e ) {
                    debugmsg(
                        "Failed to run hook on_every_x(interval = %s): %s",
                        to_string( entry.interval ), e.what()
                    );
                }
            }
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

void run_on_game_save_hooks( lua_state &state )
{
    run_hooks( state, "on_game_save" );
}

void run_on_game_load_hooks( lua_state &state )
{
    run_hooks( state, "on_game_load" );
}

void run_on_mapgen_postprocess_hooks( lua_state &state, map &m, const tripoint &p,
                                      const time_point &when )
{
    run_hooks( state, "on_mapgen_postprocess", m, p, when );
}

} // namespace cata
