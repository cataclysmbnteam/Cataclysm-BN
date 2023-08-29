#pragma once
#ifndef CATA_SRC_CATALUA_IMPL_H
#define CATA_SRC_CATALUA_IMPL_H

#include "calendar.h"
#include "catalua_sol.h"

namespace cata
{
struct on_every_x_hooks {
    time_duration interval;
    std::vector<sol::protected_function> functions;
};

/**
 * Lua state handle.
 * Definition is hidden from outside code to prevent sol::state
 * usage and visibility outside catalua files.
 */
struct lua_state {
    sol::state lua;

    lua_state() = default;
    ~lua_state() = default;
};

} // namespace cata

sol::state make_lua_state();
void run_lua_script( sol::state &lua, const std::string &script_name );
void run_console_input( sol::state &lua, const std::string &chunk );
void check_func_result( sol::protected_function_result &res );

#endif // CATA_SRC_CATALUA_IMPL_H
