#pragma once

#include <functional>
#include <string_view>

#include "catalua_sol.h"

class lua_state;

namespace cata
{

/// Run Lua hooks registered with given name.
/// an empty table needs to be initialized in `init_global_state_tables` first.
void run_hooks( std::string_view hooks_table );

/// Run Lua hooks registered with given name.
/// an empty table needs to be initialized in `init_global_state_tables` first.
///
/// To pass parameters to the hook, use `init` function and set values to table.
void run_hooks( std::string_view hooks_table,
                std::function < auto( sol::table &params ) -> void > init );

/// Define all hooks that are used in the game.
void define_hooks( lua_state &state );

} // namespace cata
