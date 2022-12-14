#pragma once
#ifndef CATA_SRC_CATALUA_BINDINGS_H
#define CATA_SRC_CATALUA_BINDINGS_H

#include "catalua_sol_fwd.h"

sol::table make_readonly_table( sol::state_view &lua, sol::table read_from );
sol::table make_readonly_table( sol::state_view &lua, sol::table read_from,
                                const std::string &error_msg );

void reg_debug_logging( sol::state &lua );
void reg_game_bindings( sol::state &lua );
void reg_docced_bindings( sol::state &lua );

#endif // CATA_SRC_CATALUA_BINDINGS_H
