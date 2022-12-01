#pragma once
#ifndef CATA_SRC_CATALUA_H
#define CATA_SRC_CATALUA_H

#include "type_id.h"

#include <memory>

namespace cata
{
struct lua_state;
struct lua_state_deleter {
    void operator()( lua_state *state ) const;
};

bool has_lua();
int get_lua_api_version();
void startup_lua_test();

std::unique_ptr<lua_state, lua_state_deleter> make_wrapped_state();

void set_mod_list( lua_state &state, const std::vector<mod_id> &modlist );
void set_mod_being_loaded( lua_state &state, const mod_id &mod );
void run_mod_preload_script( lua_state &state, const mod_id &mod );
void run_mod_finalize_script( lua_state &state, const mod_id &mod );

} // namespace cata

#endif // CATA_SRC_CATALUA_H
