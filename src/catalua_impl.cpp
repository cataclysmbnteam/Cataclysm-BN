#ifdef LUA
#include "catalua_impl.h"

#include "catalua_bindings.h"
#include "catalua_sol.h"
#include "string_formatter.h"

sol::state make_lua_state()
{
    sol::state lua;

    lua.open_libraries(
        sol::lib::base,
        sol::lib::debug,
        sol::lib::io,
        sol::lib::math,
        sol::lib::os,
        sol::lib::string,
        sol::lib::table
    );

    reg_debug_logging( lua );
    reg_game_bindings( lua );

    return lua;
}

void run_lua_script( sol::state &lua, const std::string &script_name )
{
    sol::load_result load_res = lua.load_file( script_name );

    if( !load_res.valid() ) {
        sol::error err = load_res;
        throw std::runtime_error(
            string_format( "Failed to load script %s: %s", script_name, err.what() )
        );
    }

    sol::protected_function exec = load_res;

    // Sandboxing: create environment that uses globals table as fallback
    // to prevent script from accidentally (or intentionally) modifying globals table.
    // All modifications will be stored in an environment which then gets discarded.
    sol::environment my_env( lua, sol::create, lua.globals() );
    sol::set_environment( my_env, exec );

    sol::protected_function_result exec_res = exec();

    if( !exec_res.valid() ) {
        sol::error err = exec_res;
        throw std::runtime_error(
            string_format( "Script runtime error in %s: %s", script_name, err.what() )
        );
    }
}

void check_func_result( sol::protected_function_result &res )
{
    if( !res.valid() ) {
        sol::error err = res;
        throw std::runtime_error(
            string_format( "Script runtime error: %s", err.what() )
        );
    }
}

#endif
