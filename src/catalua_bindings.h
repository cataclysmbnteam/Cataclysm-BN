#pragma once
#ifndef CATA_SRC_CATALUA_BINDINGS_H
#define CATA_SRC_CATALUA_BINDINGS_H

#include "catalua_sol_fwd.h"

#include <string>

namespace cata
{
namespace detail
{
std::string fmt_lua_va( sol::variadic_args va );
void override_default_print( sol::state &lua );
void forbid_unsafe_functions( sol::state &lua );

void reg_colors( sol::state &lua );
void reg_constants( sol::state &lua );
void reg_coords_library( sol::state &lua );
void reg_creature_family( sol::state &lua );
void reg_debug_api( sol::state &lua );
void reg_distribution_grid( sol::state &lua );
void reg_enums( sol::state &lua );
void reg_game_api( sol::state &lua );
void reg_locale_api( sol::state &lua );
void reg_hooks_examples( sol::state &lua );
void reg_item( sol::state &lua );
void reg_map( sol::state &lua );
void reg_point_tripoint( sol::state &lua );
void reg_testing_library( sol::state &lua );
void reg_time_types( sol::state &lua );
void reg_game_ids( sol::state &lua );
void reg_types( sol::state &lua );
void reg_ui_elements( sol::state &lua );

} // namespace detail

void reg_all_bindings( sol::state &lua );

} // namespace cata

#endif // CATA_SRC_CATALUA_BINDINGS_H
