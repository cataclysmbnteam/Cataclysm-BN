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

struct no_bases_t {};
constexpr static no_bases_t no_bases;

template<typename ...Args>
using bases = sol::bases<Args...>;

constexpr sol::no_construction no_constructor;

template<typename ...Args>
using constructors = sol::constructors<Args...>;

namespace detail
{

constexpr std::string_view KEY_TYPES = "#types";
constexpr std::string_view KEY_TYPE_IMPL = "#type_impl";
constexpr std::string_view KEY_DOCTABLE = "catadoc";
constexpr std::string_view KEY_BASES = "#bases";
constexpr std::string_view KEY_CONSTRUCT = "#construct";
constexpr std::string_view KEY_MEMBER = "#member";
constexpr std::string_view KEY_MEMBER_TYPE = "type";
constexpr std::string_view KEY_MEMBER_VARIABLE_TYPE = "vartype";
constexpr std::string_view KEY_MEMBER_ARGS = "args";
constexpr std::string_view KEY_MEMBER_RETVAL = "retval";
constexpr std::string_view KEY_MEMBER_NAME = "name";
constexpr std::string_view KEY_GET_TYPE = "get_luna_type";

constexpr std::string_view MEMBER_IS_VAR = "var";
constexpr std::string_view MEMBER_IS_FUNC = "func";
constexpr std::string_view MEMBER_IS_CONST_FUNC = "const_func";

template<typename T>
struct luna_traits {
    constexpr static bool impl = false;
    constexpr static std::string_view name = "<value>";
};

template<typename T>
struct member_type { };

// TODO: these were copied from string_formatter.h, find a way to share between files
// Checks whether T is instance of TMPL
template <typename, template <typename> class>
struct is_instance_of : std::false_type {};
template <typename T, template <typename> class TMPL>
struct is_instance_of<TMPL<T>, TMPL> : std::true_type {};
// Checks whether T is instance of std::function<T2>
template<typename T>
using is_std_function = typename std::conditional <
                        is_instance_of<typename std::decay<T>::type, std::function>::value,
                        std::true_type, std::false_type >::type;

template<typename Val>
std::string doc_value()
{
    //static_assert(luna_traits<Val>::impl, "Type must inplement luna_traits" );
    return std::string( luna_traits<Val>::name );
}

template<typename ...Args>
std::vector<std::string> doc_arg_list()
{
    std::vector<std::string> ret;

    ( (
          ret.push_back(
              doc_value<typename std::remove_cv<typename std::remove_reference<Args>::type>::type>() )
      ), ... );

    return ret;
}

template<typename RetVal, typename ...Args>
std::vector<std::string> doc_one_constructor( std::function<RetVal( Args... )> )
{
    return doc_arg_list<Args...>();
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
    std::vector<std::vector<std::string>> ctors;
    dt[KEY_CONSTRUCT] = ctors;
}

template<typename ...Args>
void doc_bases( sol::table &dt, const sol::bases<Args...> & )
{
    std::vector<std::string> bases;

    ( (
          bases.push_back( doc_value<Args>() )
      ), ... );

    dt[KEY_BASES] = bases;
}

inline void doc_bases( sol::table &dt, const no_bases_t & )
{
    std::vector<std::string> ctors;
    dt[KEY_BASES] = ctors;
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

template<typename Class>
inline sol::table get_type_doctable( sol::state_view &lua )
{
    static_assert( detail::luna_traits<Class>::impl, "Type must implement luna_traits<T>" );

    sol::table gdt = get_global_doctable( lua );
    return gdt[detail::KEY_TYPES][detail::luna_traits<Class>::name];
}

template<typename Class, typename Value>
void doc_member( sol::table &dt, member_type<Value Class::*> && )
{
    dt[KEY_MEMBER_TYPE] = MEMBER_IS_VAR;
    dt[KEY_MEMBER_VARIABLE_TYPE] = doc_value<Value>();
}

template<typename Class, typename RetVal, typename ...Args>
void doc_member( sol::table &dt, member_type<std::function<RetVal( Args... )>> && )
{
    dt[KEY_MEMBER_TYPE] = MEMBER_IS_FUNC;
    dt[KEY_MEMBER_RETVAL] = doc_value<RetVal>();
    dt[KEY_MEMBER_ARGS] = doc_arg_list<Args...>();
}

template<typename Class, bool add_self_arg, typename RetVal, typename ...Args>
void doc_member_fx( sol::table &dt, member_type<RetVal> &&, member_type<sol::types<Args...>> && )
{
    dt[KEY_MEMBER_TYPE] = MEMBER_IS_FUNC;
    dt[KEY_MEMBER_RETVAL] = doc_value<RetVal>();
    if constexpr( add_self_arg ) {
        dt[KEY_MEMBER_ARGS] = doc_arg_list<Class, Args...>();
    } else {
        dt[KEY_MEMBER_ARGS] = doc_arg_list<Args...>();
    }
}

/*
template<typename Class, typename Value>
void doc_member( sol::table &dt, member_type<Value> && )
{
    dt[KEY_MEMBER_TYPE] = MEMBER_IS_VAR;
    dt[KEY_MEMBER_VARIABLE_TYPE] = "<lambda>";

    auto f = std::function<Value>( nullptr );
}
*/

/*
template<typename Class, typename Value >
inline typename std::enable_if < !is_std_function<Value>::value, void >::type
doc_member( sol::table &dt, member_type<Value> && )
{
    doc_member<Class>( dt, member_type<std::function<Value>>() );
}

template<typename Class, typename RetVal, typename ...Args>
void doc_member( sol::table &dt, member_type<RetVal( Class::* )( Args... )> && )
{
    dt[KEY_MEMBER_TYPE] = MEMBER_IS_FUNC;
    dt[KEY_MEMBER_RETVAL] = doc_value<RetVal>();
    dt[KEY_MEMBER_ARGS] = doc_arg_list<Args...>();
}

template<typename Class, typename RetVal, typename ...Args>
void doc_member( sol::table &dt, member_type<RetVal( Class::* )( Args... ) const> && )
{
    dt[KEY_MEMBER_TYPE] = MEMBER_IS_CONST_FUNC;
    dt[KEY_MEMBER_RETVAL] = doc_value<RetVal>();
    dt[KEY_MEMBER_ARGS] = doc_arg_list<Args...>();
}

template<typename Class, typename RetVal, typename ...Args>
void doc_member( sol::table &dt, member_type<std::function<RetVal( Args... )>> && )
{
    dt[KEY_MEMBER_TYPE] = MEMBER_IS_FUNC;
    dt[KEY_MEMBER_RETVAL] = doc_value<RetVal>();
    dt[KEY_MEMBER_ARGS] = doc_arg_list<Args...>();
}

template<typename Class, typename Value>
void doc_member( sol::table &dt, member_type<Value Class::*> && )
{
    dt[KEY_MEMBER_TYPE] = MEMBER_IS_VAR;
    dt[KEY_MEMBER_VARIABLE_TYPE] = doc_value<Value>();
}
*/

} // namespace detail

template<typename Class, typename ConstructorScheme, typename Bases>
sol::usertype<Class> new_usertype(
    sol::state_view &lua,
    Bases &&bases,
    ConstructorScheme &&constructor
)
{
    static_assert( detail::luna_traits<Class>::impl, "Type must implement luna_traits<T>" );

    // Ensure global doctable exists
    sol::table global_dt = detail::get_global_doctable( lua );

    constexpr std::string_view name = detail::luna_traits<Class>::name;

    // Register Sol usertype
    sol::usertype<Class> ut;
    using BasesBare = typename std::remove_cv<typename std::remove_reference<Bases>::type>::type;
    if constexpr( std::is_same_v<BasesBare, no_bases_t> ) {
        ut = lua.new_usertype<Class>( name, constructor );
    } else {
        ut = lua.new_usertype<Class>( name, constructor, sol::base_classes, bases );
    }

    // Create doctable for this type
    sol::table type_dt = lua.create_table();
    global_dt[detail::KEY_TYPES][name] = type_dt;

    // Document Sol usertype
    type_dt[detail::KEY_TYPE_IMPL] = ut;

    // Init members table
    type_dt[detail::KEY_MEMBER] = std::vector<sol::object>();

    // Document constructors (or lack thereof)
    detail::doc_constructors( type_dt, constructor );

    // Document bases (or lack thereof)
    detail::doc_bases( type_dt, bases );

    // Add helper method to get name under which usertype is bound in Lua
    ut[detail::KEY_GET_TYPE] = []() -> std::string_view {
        return detail::luna_traits<Class>::name;
    };

    return ut;
}

template<typename Signature>
using fx_traits = sol::meta::meta_detail::fx_traits<Signature>;

template<typename Class, typename Key, typename Func >
void set_fx(
    sol::usertype<Class> &ut,
    Key &&key,
    Func value
)
{
    ut.set( key, std::forward<Func>( value ) );

    sol::state_view lua( ut.lua_state() );
    sol::table type_dt = detail::get_type_doctable<Class>( lua );

    std::vector<sol::object> &members = type_dt[ detail::KEY_MEMBER ];

    sol::table member_dt = lua.create_table();
    member_dt[detail::KEY_MEMBER_NAME] = key;
    members.push_back( member_dt );

    using RetVal = typename fx_traits<Func>::return_type;
    using Args = typename fx_traits<Func>::args_list;
    constexpr bool add_self_arg = !fx_traits<Func>::is_member_function;
    detail::doc_member_fx<Class, add_self_arg>( member_dt, detail::member_type<RetVal>(),
            detail::member_type<Args>() );
}

template<typename Class, typename Key, typename Value>
void set(
    sol::usertype<Class> &ut,
    Key &&key,
    Value &&value
)
{
    ut.set( key, std::forward<Value>( value ) );

    sol::state_view lua( ut.lua_state() );
    sol::table type_dt = detail::get_type_doctable<Class>( lua );

    std::vector<sol::object> &members = type_dt[ detail::KEY_MEMBER ];

    sol::table member_dt = lua.create_table();
    member_dt[detail::KEY_MEMBER_NAME] = key;
    members.push_back( member_dt );

    detail::doc_member<Class>( member_dt, detail::member_type<Value>() );
}

} // namespace luna

#endif // CATA_SRC_CATALUA_BIND_H
