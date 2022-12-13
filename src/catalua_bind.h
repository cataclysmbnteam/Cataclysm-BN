#pragma once
#ifndef CATA_SRC_CATALUA_BIND_H
#define CATA_SRC_CATALUA_BIND_H

#include <functional>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "catalua_sol.h"

#define LUNA_VAL( Class, Name )                         \
    namespace luna::detail {                            \
    template<>                                          \
    struct luna_traits<Class> {                         \
        constexpr static bool impl = true;              \
        constexpr static std::string_view name = Name;  \
    };                                                  \
    } // namespace luna::detail

namespace luna
{

namespace detail
{

constexpr std::string_view KEY_TYPES = "#types";
constexpr std::string_view KEY_TYPE_IMPL = "#type_impl";
constexpr std::string_view KEY_DOCTABLE = "catadoc";
constexpr std::string_view KEY_CONSTRUCT = "#construct";
constexpr std::string_view KEY_GET_TYPE = "get_luna_type";

template<typename T>
struct luna_traits {
    constexpr static bool impl = false;
    constexpr static std::string_view name = "<value>";
};

template<typename Val>
std::string doc_value()
{
    return std::string( luna_traits<Val>::name );
}

template<typename RetVal, typename ...Args>
std::vector<std::string> doc_one_constructor( std::function<RetVal( Args... )> )
{
    std::vector<std::string> ret;

    ( (
          ret.push_back(
              doc_value<typename std::remove_cv<typename std::remove_reference<Args>::type>::type>() )
      ), ... );

    return ret;
}

template<typename ...Args>
void doc_constructors( sol::table &dt, const sol::constructor_list<Args...> & )
{
    std::vector<std::vector<std::string>> ctors;

    ( (
          ctors.push_back( doc_one_constructor( std::function<Args>( nullptr ) ) )
      ), ... );

    dt[KEY_CONSTRUCT] = ctors;
}

inline void doc_constructors( sol::table &dt, const sol::no_construction & )
{
    std::vector<std::string> ctors;
    dt[KEY_CONSTRUCT] = ctors;
}

inline sol::table get_global_doctable( sol::state_view &lua )
{
    sol::object obj = lua[detail::KEY_DOCTABLE];
    if( obj.valid() ) {
        return obj;
    } else {
        sol::table dt = lua.create_table();
        dt[detail::KEY_TYPES] = lua.create_table();
        lua[detail::KEY_DOCTABLE] = dt;
        return dt;
    }
}

} // namespace detail

template<typename Class, typename ConstructorScheme>
sol::usertype<Class> new_usertype(
    sol::state_view &lua,
    ConstructorScheme &&constructor
)
{
    static_assert( detail::luna_traits<Class>::impl, "Type must implement luna_traits<T>" );

    // Ensure global doctable exists
    sol::table global_dt = detail::get_global_doctable( lua );

    constexpr std::string_view name = detail::luna_traits<Class>::name;

    // Register Sol usertype
    sol::usertype<Class> ut = lua.new_usertype<Class>( name, constructor );

    // Create doctable for this type
    sol::table type_dt = lua.create_table();
    global_dt[detail::KEY_TYPES][name] = type_dt;

    // Document Sol usertype
    type_dt[detail::KEY_TYPE_IMPL] = ut;

    // Document constructors (or lack thereof)
    detail::doc_constructors( type_dt, constructor );

    // Add helper method to get name under which usertype is bound in Lua
    ut[detail::KEY_GET_TYPE] = []() -> std::string_view {
        return detail::luna_traits<Class>::name;
    };

    return ut;
}

} // namespace luna

#endif // CATA_SRC_CATALUA_BIND_H
