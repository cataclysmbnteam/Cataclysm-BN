#pragma once
#ifndef CATA_SRC_CATALUA_BINDINGS_H
#define CATA_SRC_CATALUA_BINDINGS_H

#include "catalua_sol_fwd.h"

void make_table_readonly( sol::state &lua, sol::table &t );

void reg_debug_logging( sol::state &lua );
void reg_game_bindings( sol::state &lua );

#endif // CATA_SRC_CATALUA_BINDINGS_H
