#pragma once

#include <iostream>
#include <sstream>

#include "point.h"

namespace detail
{

// Concepts

template<typename T>
concept has_istream_read = requires( T &in_val, std::ostream &os )
{
    { os << in_val }
    -> std::convertible_to<std::ostream &>;
};

template<typename T>
concept has_ostream_write = requires( T &in_val, std::istream &is )
{
    { is >> in_val }
    -> std::convertible_to<std::istream &>;
};

template<typename T>
concept has_iostream_read_write = has_istream_read<T> &&has_ostream_write<T>;

template<typename T>
concept is_reference = std::is_reference_v<T>;

template<typename T>
concept is_numeric = std::disjunction_v<std::is_floating_point<T>, std::is_integral<T>>;

template<typename T, size_t N>
concept has_get_ref = requires( T &t ) { { std::get<N>( t ) } -> is_reference; };

template<typename T, size_t N>
struct has_get_ref_n : std::integral_constant < bool, has_get_ref<T, N> &&has_get_ref_n < T,
    N - 1 >::value > {};

template<typename T>
struct has_get_ref_n<T, 0> : std::integral_constant<bool, has_get_ref<T, 0>> {};

template<typename T>
concept is_tuple_like = requires { typename std::tuple_size<T>::type; } &&has_get_ref_n < T,
        std::tuple_size_v<T> - 1 >::value;

// Helpers

template<typename First, typename... Rest>
requires( has_ostream_write<First> &&( has_ostream_write<Rest> &&... ) )
static bool ostream_write_values( std::ostream &ss, const char delim, const First &val,
                                  const Rest &... rest )
{
    ss << val;
    if( ss.fail() ) {
        return false;
    }
    if constexpr( sizeof...( rest ) > 0 ) {
        ss << delim;
        return detail::ostream_write_values( ss, delim, rest... );
    } else {
        return true;
    }
}

template<typename Value>
requires( has_istream_read<Value> )
static bool istream_read_values( std::istream &ss, const char, Value &val )
{
    ss >> std::ws >> val;
    if( ss.fail() ) {
        return false;
    }
    ss >> std::ws;
    return ss.eof();
}

template<typename First, typename... Rest>
requires( has_istream_read<First> &&( has_istream_read<Rest> &&... ) )
static bool istream_read_values( std::istream &ss, const char delim, First &val, Rest &... rest )
{
    ss >> std::ws >> val;
    if( ss.fail() ) {
        return false;
    }

    char c;
    ss >> std::ws >> c;
    if( c != delim ) {
        return false;
    }

    return detail::istream_read_values( ss, delim, rest... );
}

template<typename T, size_t ... Is>
static bool ostream_write_seq_get( std::ostream &ss, const char delim, const T &val,
                                   std::index_sequence<Is...> )
{
    return detail::ostream_write_values( ss, delim, std::get<Is>( val )... );
}

template<typename T, size_t ... Is>
static bool istream_read_seq_get( std::istream &ss, const char delim, T &val,
                                  std::index_sequence<Is...> )
{
    return detail::istream_read_values( ss, delim, std::get<Is>( val )... );
}

// Converters

template<typename T>
struct type_converter;

template<typename T>
requires( has_iostream_read_write<T> )
struct basic_converter {

    bool operator()( const T &in_val, std::string &out_val ) const {
        std::ostringstream ss;
        if( !ostream_write_values( ss, '\0', in_val ) ) {
            return false;
        }
        out_val = ss.str();
        return true;
    };

    bool operator()( const std::string &in_val, T &out_val ) const {
        std::istringstream ss( in_val );
        T tmp;
        if( !istream_read_values( ss, '\0', tmp ) ) {
            return false;
        }
        out_val = tmp;
        return true;
    }
};

template<typename T, size_t N = std::tuple_size_v<T>>
requires( has_get_ref_n < T, N - 1 >::value )
struct tuple_converter {

    bool operator()( const T &in_val, std::string &out_val ) const {
        std::ostringstream ss;
        if( !ostream_write_seq_get( ss, ',', in_val, std::make_index_sequence<N>() ) ) {
            return false;
        }
        out_val = ss.str();
        return true;
    }

    bool operator()( const std::string &in_val, T &out_val ) const {
        std::istringstream ss( in_val );
        T tmp;
        if( !istream_read_seq_get( ss, ',', tmp, std::make_index_sequence<N>() ) ) {
            return false;
        }
        out_val = tmp;
        return true;
    }
};

struct tripoint_converter {
    using RealType = tripoint;
    using ProxyType = std::tuple<int, int, int>;
    using ProxyConverter = tuple_converter<ProxyType>;

    static ProxyType to_proxy( const RealType &v ) {
        auto& [x, y, z] = v;
        return ProxyType{x, y, z};
    }
    static RealType from_proxy( const ProxyType &v ) {
        auto& [x, y, z] = v;
        return RealType( x, y, z );
    }

    bool operator()( const tripoint &in_val, std::string &out_val ) const {
        constexpr auto cvt = ProxyConverter{};
        return cvt( to_proxy( in_val ), out_val );
    }
    bool operator()( const std::string &in_val, tripoint &out_val ) const {
        constexpr auto cvt = ProxyConverter{};
        ProxyType tmp;
        if( !cvt( in_val, tmp ) ) {
            return false;
        }
        out_val = from_proxy( tmp );
        return true;
    }
};

template<is_tuple_like T>
struct type_converter<T> : tuple_converter<T> {};

template<has_iostream_read_write T>
struct type_converter<T> : basic_converter<T> {};

template<>
struct type_converter<tripoint> : tripoint_converter {};

} // namespace detail