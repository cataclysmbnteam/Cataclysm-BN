#include "catalua.h"

#include "debug.h"

constexpr int LUA_API_VERSION = 1;

#ifndef LUA

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

std::unique_ptr<lua_state, lua_state_deleter> make_wrapped_state()
{
    return std::unique_ptr<lua_state, lua_state_deleter>(
               new lua_state{}, lua_state_deleter{}
           );
}

void set_mod_list( lua_state &, const std::vector<mod_id> & ) {}
void set_mod_being_loaded( lua_state &, const mod_id & ) {}
void run_mod_preload_script( lua_state &, const mod_id & ) {}
void run_mod_finalize_script( lua_state &, const mod_id & ) {}

} // namespace cata

#else // LUA

#include "catalua_sol.h"

#include "avatar.h"
#include "catalua_impl.h"
#include "catalua_bindings.h"
#include "filesystem.h"
#include "mod_manager.h"
#include "path_info.h"

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

    make_table_readonly( lua, active_mods );
    make_table_readonly( lua, active_mods );

    lua.globals()["game"]["active_mods"] = active_mods;
    lua.globals()["game"]["mod_runtime"] = mod_runtime;
}

void set_mod_being_loaded( lua_state &state, const mod_id &mod )
{
    sol::state &lua = state.lua;

    lua.globals()["game"]["current_mod"] = mod.str();
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

} // namespace cata
