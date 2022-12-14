#ifdef LUA
#include "catalua_impl.h"

#include "catalua_log.h"
#include "catalua_sol.h"
#include "debug.h"
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

    reg_all_bindings( lua );

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

void run_console_input( sol::state &lua, const std::string &chunk )
{
    DebugLog( DL::Info, DC::Lua ) << "CONSOLE: " << chunk;
    cata::get_lua_log_instance().add(
        cata::LuaLogLevel::Input,
        std::string( chunk )
    );

    sol::load_result load_res = lua.load( chunk, "console input" );

    if( !load_res.valid() ) {
        // No need to use obnoxious debugmsgs, user will see the error in log
        sol::error err = load_res;
        cata::get_lua_log_instance().add(
            cata::LuaLogLevel::Error,
            string_format( "Failed to load: %s", err.what() )
        );
        return;
    }

    sol::protected_function exec = load_res;

    // No sandboxing here, we trust console user :)

    sol::protected_function_result exec_res = exec();

    if( !exec_res.valid() ) {
        // No need to use obnoxious debugmsgs, user will see the error in log
        sol::error err = exec_res;
        cata::get_lua_log_instance().add(
            cata::LuaLogLevel::Error,
            string_format( "Runtime error: %s", err.what() )
        );
        return;
    }

    try {
        sol::object retval = exec_res;
        if( retval == sol::nil ) {
            // No return - don't spam
            return;
        }
        std::string val = lua["tostring"]( retval );
        cata::get_lua_log_instance().add(
            cata::LuaLogLevel::Info,
            string_format( "# %s", val )
        );
    } catch( std::runtime_error &e ) {
        cata::get_lua_log_instance().add(
            cata::LuaLogLevel::Info,
            string_format( "# ??? %s", e.what() )
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
