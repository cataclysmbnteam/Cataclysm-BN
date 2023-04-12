#if defined(LUA)
#include "catalua_serde.h"

#include "catalua_sol.h"
#include "debug.h"
#include "json.h"
#include "string_formatter.h"

#include <algorithm>
#include <vector>

namespace cata
{

void serialize_lua_table_internal( sol::table t, JsonOut &jsout, std::vector<sol::table> &stack )
{
    for( const sol::table &it : stack ) {
        if( it == t ) {
            debugmsg( "Tried to serialize recursive table structure." );
            jsout.write_null();
            return;
        }
    }

    stack.push_back( t );

    sol::state_view lua( t.lua_state() );

    jsout.start_object();

    std::vector<std::string> keys;

    t.for_each( [&]( sol::object key_obj, sol::object /*val_obj*/ ) {
        std::string key = key_obj.as<std::string>();
        keys.push_back( key );
    } );

    std::sort( keys.begin(), keys.end() );

    for( const std::string &key : keys ) {
        jsout.member( key );
        sol::object val = t[key];

        switch( val.get_type() ) {
            case sol::type::boolean: {
                jsout.write( val.as<bool>() );
                break;
            }
            case sol::type::number: {
                // Horrible hack
                sol::protected_function math_type = lua.script( "return math.type" );
                if( !math_type ) {
                    debugmsg( "Int/float serialization hack failed to acquare math.type" );
                    jsout.write_null();
                } else {
                    sol::protected_function_result res = math_type( val );
                    if( res.status() != sol::call_status::ok ) {
                        sol::error err = res;
                        debugmsg( "Int/float serialization hack failed to run math.type: %s", err.what() );
                        jsout.write_null();
                    } else {
                        sol::object retval = res;
                        if( !retval.valid() ) {
                            debugmsg( "Int/float serialization hack returned invalid value" );
                            jsout.write_null();
                        } else {
                            std::string mtype = res;
                            // There's no clear difference in JSON between int and float,
                            // so we have to be explicit to avoid subtle errors down the line
                            if( mtype == "integer" ) {
                                jsout.start_object();
                                jsout.member_as_string( "type", "int" );
                                jsout.member( "data" );
                                jsout.write( val.as<int64_t>() );
                                jsout.end_object();
                            } else if( mtype == "float" ) {
                                jsout.start_object();
                                jsout.member_as_string( "type", "float" );
                                jsout.member( "data" );
                                jsout.write( val.as<double>() );
                                jsout.end_object();
                            } else {
                                debugmsg( "Int/float serialization hack returned invalid value: %s", mtype );
                                jsout.write_null();
                            }
                        }
                    }
                }
                break;
            }
            case sol::type::string: {
                jsout.write( val.as<std::string>() );
                break;
            }
            case sol::type::table: {
                jsout.start_object();
                jsout.member_as_string( "type", "lua_table" );
                jsout.member( "data" );
                serialize_lua_table_internal( val.as<sol::table>(), jsout, stack );
                jsout.end_object();
                break;
            }
            case sol::type::userdata: {
                sol::table table_val = val.as<sol::table>();
                if( !table_val.valid() ) {
                    debugmsg( "Sanity check failed - serializable type can be converted to table." );
                    jsout.write_null();
                    break;
                }
                try {
                    if( !table_val["get_luna_type"].valid() ) {
                        debugmsg( "Tried to serialize usertype that was not registered with luna." );
                        jsout.write_null();
                        break;
                    }
                } catch( sol::error &e ) {
                    debugmsg( "Tried to serialize userdata that was not registered as usertype." );
                    jsout.write_null();
                    break;
                }
                sol::protected_function get_luna_type_func = table_val["get_luna_type"];
                sol::protected_function serialize_func = table_val["serialize"];

                if( !get_luna_type_func ) {
                    debugmsg( "Tried to serialize usertype that has not been registered through luna." );
                    jsout.write_null();
                    break;
                }
                if( !serialize_func ) {
                    debugmsg( "Tried to serialize usertype that does not allow serialization." );
                    jsout.write_null();
                    break;
                }
                std::string kind = get_luna_type_func( val );
                jsout.start_object();
                jsout.member_as_string( "type", "userdata" );
                jsout.member_as_string( "kind", kind );
                jsout.member( "data" );
                sol::protected_function_result res = serialize_func( val, jsout );
                if( res.status() != sol::call_status::ok ) {
                    sol::error err = res;
                    debugmsg( "Failed to serialize type '%s': %s", kind, err.what() );
                }
                jsout.end_object();
                break;
            }
            default: {
                // TODO: throw error
                debugmsg( "Unsupported type encountered when serializing Lua table." );
                jsout.write_null();
                break;
            }
        }
    }

    jsout.end_object();

    stack.pop_back();
}

void serialize_lua_table( sol::table t, JsonOut &jsout )
{
    std::vector<sol::table> stack;
    serialize_lua_table_internal( t, jsout, stack );
}

void deserialize_lua_table( sol::table t, JsonObject &obj )
{
    sol::state_view lua( t.lua_state() );

    for( JsonMember it : obj ) {
        std::string key = it.name();
        if( it.test_bool() ) {
            t[key] = it.get_bool();
        } else if( it.test_string() ) {
            t[key] = it.get_string();
        } else if( it.test_object() ) {
            JsonObject jo = it.get_object();
            std::string entry_type = jo.get_member( "type" );
            if( entry_type == "int" ) {
                int data = jo.get_int( "data" );
                t[key] = data;
            } else if( entry_type == "float" ) {
                double data = jo.get_float( "data" );
                t[key] = data;
            } else if( entry_type == "lua_table" ) {
                JsonObject data = jo.get_object( "data" );
                sol::table new_table = lua.create_table();
                deserialize_lua_table( new_table, data );
                t[key] = new_table;
            } else if( entry_type == "userdata" ) {
                std::string kind = jo.get_member( "kind" );
                JsonIn &data_raw = *jo.get_raw( "data" );
                // Horrible hack ahead
                std::string script = string_format( "return %s.new()", kind );
                sol::protected_function_result res = lua.script( script, "deserialize_init", sol::load_mode::any );
                if( res.status() != sol::call_status::ok ) {
                    debugmsg( "Failed to init container for deserializable type '%s'", kind );
                } else {
                    sol::object obj = res;
                    sol::table obj_table = obj.as<sol::table>();
                    if( !obj_table ) {
                        debugmsg( "Failed to init container for deserializable type '%s'", kind );
                    } else {
                        sol::protected_function deserialize_func = obj_table["deserialize"];
                        if( !deserialize_func ) {
                            debugmsg( "Failed to deserialize type '%s': not deserializable.", kind );
                        } else {
                            sol::protected_function_result res = deserialize_func( obj, data_raw );
                            if( res.status() != sol::call_status::ok ) {
                                sol::error err = res;
                                debugmsg( "Failed to deserialize type '%s': %s", kind, err.what() );
                            } else {
                                t[key] = obj;
                            }
                        }
                    }
                }
            } else {
                debugmsg( "Deserialization of record type '%s' is not implemented.", entry_type );
            }
        } else if( it.test_array() ) {
            debugmsg( "Arrays are not supported when deserializing Lua table." );
        }
    }
}

} // namespace cata

#endif

