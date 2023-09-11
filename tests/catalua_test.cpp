#ifdef LUA
#include "catch/catch.hpp"

#include "avatar.h"
#include "catacharset.h"
#include "catalua_impl.h"
#include "catalua_serde.h"
#include "catalua_sol.h"
#include "clzones.h"
#include "debug.h"
#include "faction.h"
#include "fstream_utils.h"
#include "json.h"
#include "mapdata.h"
#include "options.h"
#include "point.h"
#include "string_formatter.h"
#include "type_id.h"

static void run_lua_test_script( sol::state &lua, const std::string &script_name )
{
    std::string full_script_name = "tests/lua/" + script_name;

    run_lua_script( lua, full_script_name );
}

TEST_CASE( "lua_class_members", "[lua]" )
{
    sol::state lua = make_lua_state();

    // Create global table for test
    sol::table test_data = lua.create_table();
    lua.globals()["test_data"] = test_data;

    // Set input
    test_data["in"] = point( -10, 10 );

    // Run Lua script
    run_lua_test_script( lua, "class_members_test.lua" );

    // Get test output
    std::string res = test_data["out"];

    REQUIRE( res == "result is Point(12,13)" );
}

TEST_CASE( "lua_global_functions", "[lua]" )
{
    sol::state lua = make_lua_state();

    // Create global table for test
    sol::table test_data = lua.create_table();
    lua.globals()["test_data"] = test_data;

    // Randomize avatar name
    get_avatar().pick_name();
    std::string expected = get_avatar().name;

    // Run Lua script
    run_lua_test_script( lua, "global_functions_test.lua" );

    // Get test output
    std::string res = test_data["out"];

    REQUIRE( res == expected );
}

TEST_CASE( "lua_called_from_cpp", "[lua]" )
{
    sol::state lua = make_lua_state();

    // Create global table for test
    sol::table test_data = lua.create_table();
    lua.globals()["test_data"] = test_data;

    // Run Lua script
    run_lua_test_script( lua, "called_from_cpp_test.lua" );

    // Get Lua function
    REQUIRE( test_data["func"].valid() );
    sol::protected_function lua_func = test_data["func"];

    // Get test output
    REQUIRE( test_data["out"].valid() );
    sol::table out_data = test_data["out"];
    int ret = 0;

    REQUIRE( out_data["i"].valid() );
    REQUIRE( out_data["s"].valid() );

    CHECK( out_data["i"] == 0 );
    CHECK( out_data.get<std::string>( "s" ) == "" );

    // Execute function
    ret = lua_func( 4, "Bright " );

    CHECK( ret == 8 );
    CHECK( out_data["i"] == 4 );
    CHECK( out_data.get<std::string>( "s" ) == "Bright " );

    // Execute function again
    ret = lua_func( 6, "Nights" );

    CHECK( ret == 12 );
    CHECK( out_data["i"] == 10 );
    CHECK( out_data.get<std::string>( "s" ) == "Bright Nights" );

    // And again, but this time with 1 parameter
    ret = lua_func( 1 );

    CHECK( ret == 2 );
    CHECK( out_data["i"] == 11 );
    CHECK( out_data.get<std::string>( "s" ) == "Bright Nights" );
}

TEST_CASE( "lua_runtime_error", "[lua]" )
{
    sol::state lua = make_lua_state();

    // Running Lua script that has a runtime error
    // ends up throwing std::runtime_error on C++ side

    const std::string expected =
        "Script runtime error in tests/lua/runtime_error.lua: "
        "tests/lua/runtime_error.lua:3: attempt to index a nil value (global 'table_with_typo')\n"
        "stack traceback:\n"
        "\ttests/lua/runtime_error.lua:3: in main chunk";

    REQUIRE_THROWS_MATCHES(
        run_lua_test_script( lua, "runtime_error.lua" ),
        std::runtime_error,
        Catch::Message( expected )
    );
}

TEST_CASE( "lua_called_error_on_lua_side", "[lua]" )
{
    sol::state lua = make_lua_state();

    // Running Lua script that calls error()
    // ends up throwing std::runtime_error on C++ side

    const std::string expected =
        "Script runtime error in tests/lua/called_error_on_lua_side.lua: "
        "tests/lua/called_error_on_lua_side.lua:3: Error called on Lua side!\n"
        "stack traceback:\n"
        "\t[C]: in function 'base.error'\n"
        "\ttests/lua/called_error_on_lua_side.lua:3: in main chunk";

    REQUIRE_THROWS_MATCHES(
        run_lua_test_script( lua, "called_error_on_lua_side.lua" ),
        std::runtime_error,
        Catch::Message( expected )
    );
}

static void cpp_call_error( sol::this_state L )
{
    luaL_error( L.lua_state(), "Error called on Cpp side!" );
}

TEST_CASE( "lua_called_error_on_cpp_side", "[lua]" )
{
    sol::state lua = make_lua_state();

    lua.globals()["cpp_call_error"] = cpp_call_error;

    // Running Lua script that calls C++ function that calls error()
    // ends up throwing std::runtime_error on C++ side

    const std::string expected =
        "Script runtime error in tests/lua/called_error_on_cpp_side.lua: "
        "tests/lua/called_error_on_cpp_side.lua:3: Error called on Cpp side!\n"
        "stack traceback:\n"
        "\t[C]: in function 'base.cpp_call_error'\n"
        "\ttests/lua/called_error_on_cpp_side.lua:3: in main chunk";

    REQUIRE_THROWS_MATCHES(
        run_lua_test_script( lua, "called_error_on_cpp_side.lua" ),
        std::runtime_error,
        Catch::Message( expected )
    );
}

[[ noreturn ]]
static void cpp_throw_exception()
{
    throw std::runtime_error( "Exception thrown on Cpp side!" );
}

TEST_CASE( "lua_called_cpp_func_throws", "[lua]" )
{
    sol::state lua = make_lua_state();

    lua.globals()["cpp_throw_exception"] = cpp_throw_exception;

    // Running Lua script that calls C++ function that throws std::runtime_error
    // ends up throwing another std::runtime_error

    const std::string expected =
        "Script runtime error in tests/lua/called_cpp_func_throws.lua: "
        "Exception thrown on Cpp side!\n"
        "stack traceback:\n"
        "\t[C]: in function 'base.cpp_throw_exception'\n"
        "\ttests/lua/called_cpp_func_throws.lua:3: in main chunk";

    REQUIRE_THROWS_MATCHES(
        run_lua_test_script( lua, "called_cpp_func_throws.lua" ),
        std::runtime_error,
        Catch::Message( expected )
    );
}

TEST_CASE( "lua_table_serde", "[lua]" )
{
    sol::state lua = make_lua_state();

    sol::table st = lua.create_table();
    st["inner_val"] = 4;

    sol::table t = lua.create_table();
    t["member_bool"] = false;
    t["member_float"] = 16.0;
    t["member_int"] = 11;
    t["member_string"] = "fuckoff";
    t["member_usertype"] = tripoint( 7, 5, 3 );
    t["subtable"] = st;

    std::string data = serialize_wrapper( [&]( JsonOut & jsout ) {
        cata::serialize_lua_table( t, jsout );
    } );

    static const std::string data_expected =
        R"({"member_bool":false,"member_float":{"type":"float","data":16.000000},"member_int":{"type":"int","data":11},"member_string":"fuckoff","member_usertype":{"type":"userdata","kind":"Tripoint","data":[7,5,3]},"subtable":{"type":"lua_table","data":{"inner_val":{"type":"int","data":4}}}})";

    REQUIRE( data == data_expected );

    sol::table nt = lua.create_table();
    deserialize_wrapper( [&]( JsonIn & jsin ) {
        JsonObject jsobj = jsin.get_object();
        cata::deserialize_lua_table( nt, jsobj );
    }, data );

    // Sanity check: field does not exist
    sol::object mem_none = nt["member_the_best"];
    REQUIRE( !mem_none.valid() );

    sol::object mem_bool = nt["member_bool"];
    REQUIRE( mem_bool.valid() );
    REQUIRE( mem_bool.is<bool>() );
    CHECK( mem_bool.as<bool>() == false );

    sol::object mem_float = nt["member_float"];
    REQUIRE( mem_float.valid() );
    REQUIRE( mem_float.is<double>() );
    CHECK( mem_float.as<double>() == Approx( 16.0 ) );

    sol::object mem_int = nt["member_int"];
    REQUIRE( mem_int.valid() );
    CHECK( mem_int.is<double>() );
    REQUIRE( mem_int.is<int>() );
    CHECK( mem_int.as<int>() == 11 );

    sol::object mem_string = nt["member_string"];
    REQUIRE( mem_string.valid() );
    REQUIRE( mem_string.is<std::string>() );
    CHECK( mem_string.as<std::string>() == "fuckoff" );

    sol::object mem_usertype = nt["member_usertype"];
    REQUIRE( mem_usertype.valid() );
    REQUIRE( mem_usertype.is<tripoint>() );
    CHECK( mem_usertype.as<tripoint>() == tripoint( 7, 5, 3 ) );

    sol::object mem_table = nt["subtable"];
    REQUIRE( mem_table.valid() );
    REQUIRE( mem_table.is<sol::table>() );

    // Subtable
    sol::table nts = mem_table;
    sol::object inner_val = nts["inner_val"];
    REQUIRE( inner_val.valid() );
    REQUIRE( inner_val.is<int>() );
    CHECK( inner_val.as<int>() == 4 );

    // And for the good measure - serialize back to JSON
    std::string data2 = serialize_wrapper( [&]( JsonOut & jsout ) {
        cata::serialize_lua_table( nt, jsout );
    } );
    CHECK( data2 == data_expected );
}

struct custom_udata {
    int unused = 0;
};

TEST_CASE( "lua_table_serde_error_no_reg", "[lua]" )
{
    sol::state lua = make_lua_state();

    sol::table t = lua.create_table();
    t["my_member"] = custom_udata{};

    // Trying to serialize unregistered type results in error
    std::string data;
    std::string dmsg = capture_debugmsg_during( [&]() {
        data = serialize_wrapper( [&]( JsonOut & jsout ) {
            cata::serialize_lua_table( t, jsout );
        } );
    } );

    CHECK( dmsg == "Tried to serialize userdata that was not registered as usertype." );
    CHECK( data == R"({"my_member":null})" );
}

TEST_CASE( "lua_table_serde_error_no_luna", "[lua]" )
{
    sol::state lua = make_lua_state();

    lua.new_usertype<custom_udata>( "CustomUData" );

    sol::table t = lua.create_table();
    t["my_member"] = custom_udata{};

    // Trying to serialize type that was not registered with luna results in error
    std::string data;
    std::string dmsg = capture_debugmsg_during( [&]() {
        data = serialize_wrapper( [&]( JsonOut & jsout ) {
            cata::serialize_lua_table( t, jsout );
        } );
    } );

    CHECK( dmsg == "Tried to serialize usertype that was not registered with luna." );
    CHECK( data == R"({"my_member":null})" );
}

TEST_CASE( "lua_table_serde_error_no_ser", "[lua]" )
{
    sol::state lua = make_lua_state();

    sol::table t = lua.create_table();
    avatar *av_ptr = &get_avatar();
    t["my_member"] = av_ptr;

    // Trying to serialize unserializable type results in error
    std::string data;
    std::string dmsg = capture_debugmsg_during( [&]() {
        data = serialize_wrapper( [&]( JsonOut & jsout ) {
            cata::serialize_lua_table( t, jsout );
        } );
    } );

    CHECK( data == R"({"my_member":null})" );
    CHECK( dmsg == "Tried to serialize usertype that does not allow serialization." );
}

TEST_CASE( "lua_table_serde_error_rec_table", "[lua]" )
{
    sol::state lua = make_lua_state();

    sol::table t1 = lua.create_table();
    sol::table t2 = lua.create_table();
    sol::table t3 = lua.create_table();
    sol::table t4 = lua.create_table();
    sol::table t5 = lua.create_table();

    /*
        t1 -> t2 -> t3 -> t5
        ^      |
        |      \--> t4 -\
        |               |
        \---------------/
    */
    t1["t2"] = t2;
    t2["t3"] = t3;
    t2["t4"] = t4;
    t3["t5"] = t5;
    t4["t1"] = t1;

    // Trying to serialize recursive table results in error
    std::string data;
    std::string dmsg = capture_debugmsg_during( [&]() {
        data = serialize_wrapper( [&]( JsonOut & jsout ) {
            cata::serialize_lua_table( t1, jsout );
        } );
    } );

    CHECK( data ==
           R"({"t2":{"type":"lua_table","data":{"t3":{"type":"lua_table","data":{"t5":{"type":"lua_table","data":{}}}},"t4":{"type":"lua_table","data":{"t1":{"type":"lua_table","data":null}}}}}})" );
    CHECK( dmsg == "Tried to serialize recursive table structure." );
}

TEST_CASE( "id_conversions", "[lua]" )
{
    sol::state lua = make_lua_state();

    sol::table t = lua.create_table();

    // The functions don't need to do anything, we're just checking type conversion
    t["func_raw"] = []( const ter_t & ) {

    };
    t["func_int_id"] = []( const ter_id & ) {

    };
    t["func_str_id"] = []( const ter_str_id & ) {

    };

    static const ter_str_id t_fragile_roof( "t_fragile_roof" );
    REQUIRE( t_fragile_roof.is_valid() );

    const ter_t *raw_ptr = &t_fragile_roof.obj();

    t["str_id"] = t_fragile_roof;
    t["int_id"] = t_fragile_roof.id();
    t["raw_ptr"] = raw_ptr;

    lua.globals()["test_data"] = t;

    run_lua_test_script( lua, "id_conversions.lua" );
}

TEST_CASE( "id_conversions_no_int_id", "[lua]" )
{
    sol::state lua = make_lua_state();

    sol::table t = lua.create_table();

    // The functions don't need to do anything, we're just checking type conversion
    t["func_raw"] = []( const faction & ) {

    };
    t["func_str_id"] = []( const faction_id & ) {

    };

    REQUIRE( your_fac.is_valid() );

    const faction *raw_ptr = &your_fac.obj();

    t["str_id"] = your_fac;
    t["raw_ptr"] = raw_ptr;

    lua.globals()["test_data"] = t;

    run_lua_test_script( lua, "id_conversions_no_int_id.lua" );
}

TEST_CASE( "catalua_regression_sol_1444", "[lua]" )
{
    // Regression test for https://github.com/ThePhD/sol2/issues/1444
    sol::state lua = make_lua_state();
    run_lua_test_script( lua, "regression_sol_1444.lua" );
}

#endif
