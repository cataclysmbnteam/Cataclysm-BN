#pragma once
#ifndef CATA_SRC_CATALUA_IMPL_H
#define CATA_SRC_CATALUA_IMPL_H

#include "catalua_sol.h"

namespace cata
{
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
void run_lua_func( sol::object &func );

#endif // CATA_SRC_CATALUA_IMPL_H
