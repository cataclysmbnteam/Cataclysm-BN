#pragma once

#include <string_view>

class lua_state;

namespace cata
{

/// Run Lua hooks registered with given name.
/// an empty table needs to be initialized in `init_global_state_tables` first.
void run_hooks( std::string_view hooks_table );

/// Define all hooks that are used in the game.
void define_hooks( lua_state &state );

} // namespace cata
