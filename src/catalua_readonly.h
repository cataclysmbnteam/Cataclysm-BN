#pragma once
#ifndef CATA_SRC_CATALUA_READONLY_H
#define CATA_SRC_CATALUA_READONLY_H

#include "catalua_sol_fwd.h"

namespace cata
{

sol::table make_readonly_table( sol::state_view &lua, sol::table read_from );
sol::table make_readonly_table( sol::state_view &lua, sol::table read_from,
                                const std::string &error_msg );

} // namespace cata

#endif // CATA_SRC_CATALUA_READONLY_H
