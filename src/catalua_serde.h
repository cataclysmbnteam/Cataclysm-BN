#pragma once
#ifndef CATA_SRC_CATALUA_SERDE_H
#define CATA_SRC_CATALUA_SERDE_H

#include "catalua_sol_fwd.h"

class JsonObject;
class JsonOut;

namespace cata
{

void serialize_lua_table( const sol::table &t, JsonOut &jsout );
void deserialize_lua_table( sol::table t, JsonObject &obj );

} // namespace cata

#endif // CATA_SRC_CATALUA_SERDE_H
