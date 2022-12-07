#if defined(LUA)
#include "catalua_serde.h"

#include "catalua_sol.h"
#include "json.h"

namespace cata
{

void serialize_lua_table( sol::table t, JsonOut &jsout )
{
    sol::state_view lua( t.lua_state() );

    jsout.start_object();

    t.for_each( [&]( sol::object key_obj, sol::object val ) {
        std::string key = key_obj.as<std::string>();
        jsout.member( key );

        switch( val.get_type() ) {
            case sol::type::boolean: {
                jsout.write( val.as<bool>() );
                break;
            }
            case sol::type::number: {
                // TODO: distinguish int vs float to preserve number subtype
                if( val.is<int64_t>() ) {
                    jsout.write( static_cast<double>( val.as<int64_t>() ) );
                } else {
                    jsout.write( val.as<double>() );
                }
                break;
            }
            case sol::type::string: {
                jsout.write( val.as<std::string>() );
                break;
            }
            case sol::type::table: {
                // TODO: check for recursive tables
                serialize_lua_table( val.as<sol::table>(), jsout );
                break;
            }
            case sol::type::userdata: {
                // TODO: usertypes
                debugmsg( "Usertype serialization from Lua table is not implemented." );
                jsout.write_null();
                break;
            }
            default: {
                // TODO: throw error
                debugmsg( "Unsupported type encountered when serializing Lua table." );
                jsout.write_null();
                break;
            }
        }
    } );

    jsout.end_object();
}

void deserialize_lua_table( sol::table t, JsonObject &obj )
{
    sol::state_view lua( t.lua_state() );

    for( JsonMember it : obj ) {
        std::string key = it.name();
        // TODO: distinguish int vs float to preserve number subtype
        if( it.test_float() ) {
            t[key] = it.get_float();
        } else if( it.test_bool() ) {
            t[key] = it.get_bool();
        } else if( it.test_string() ) {
            t[key] = it.get_string();
        } else if( it.test_object() ) {
            // TODO: usertypes
            JsonObject jo = it.get_object();
            t[key] = lua.create_table();
            deserialize_lua_table( t, jo );
        } else if( it.test_array() ) {
            // TODO: throw instead?
            debugmsg( "Arrays are not supported when deserializing Lua table." );
        }
    }
}

} // namespace cata

#endif

