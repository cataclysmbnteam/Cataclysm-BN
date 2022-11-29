#include "catalua.h"

#include "debug.h"

#ifndef LUA

namespace cata
{

bool has_lua()
{
    return false;
}

void startup_lua_test()
{
    // Nothing to do here
}

} // namespace cata

#else

#include "catalua_sol.h"

#include "avatar.h"
#include "catalua_impl.h"
#include "path_info.h"

namespace cata
{

bool has_lua()
{
    return true;
}

void startup_lua_test()
{
    sol::state lua = make_lua_state();
    std::string lua_startup_script = PATH_INFO::datadir() + "raw/on_game_start.lua";
    try {
        run_lua_script( lua, lua_startup_script );
    } catch( std::runtime_error &e ) {
        debugmsg( "%s", e.what() );
    }
}

} // namespace cata

#endif
