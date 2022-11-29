#ifdef LUA
#include "catch/catch.hpp"

#include "avatar.h"
#include "catalua_impl.h"
#include "catalua_sol.h"
#include "options.h"
#include "point.h"
#include "string_formatter.h"

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

#endif
