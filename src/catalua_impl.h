#pragma once
#ifndef CATA_SRC_CATALUA_IMPL_H
#define CATA_SRC_CATALUA_IMPL_H

#include "catalua_sol_fwd.h"

sol::state make_lua_state();
void run_lua_script( sol::state &lua, const std::string &script_name );

#endif // CATA_SRC_CATALUA_IMPL_H
