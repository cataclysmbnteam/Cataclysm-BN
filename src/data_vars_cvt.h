#pragma once

#include <sstream>
#include <iomanip>

#include "point.h"
#include "json.h"
#include "cata_utility.h"

namespace data_vars
{

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
concept is_reference = std::is_reference_v<T>;

template<typename T>
concept is_numeric = std::disjunction_v<std::is_floating_point<T>, std::is_integral<T>>;

template<typename T>
concept is_tuple_like = requires { typename std::tuple_size<T>::type; };

// Helpers

template<typename T>
struct stream_value_io;

template<typename First, typename... Rest>
static bool stream_write_values( std::ostream &os, const char delim, const First &val, const Rest &... rest )
{
    constexpr auto io = detail::stream_value_io<First>{};
    io(os, delim, val);

    if( os.fail() ) {
        return false;
    }
    os.clear();

    if constexpr( sizeof...( rest ) == 0 ) {
        return true;
    } else {
        os << delim;
        return detail::stream_write_values( os, delim, rest... );
    }
}

template<typename First, typename... Rest>
static bool stream_read_values( std::istream &is, const char delim, First &val, Rest &... rest )
{
    constexpr auto io = detail::stream_value_io<First>{};
    io(is, delim, val);

    if( is.fail() ) {
        return false;
    }
    is.clear();

    if constexpr( sizeof...( rest ) == 0 ) {
        is >> std::ws;
        return true;
    } else {
        char c;
        is >> std::ws >> c;
        if( c != delim ) {
            return false;
        }
        return detail::stream_read_values( is, delim, rest... );
    }
}

template<typename T, typename Fn, size_t ... Is>
static bool stream_write_indexed( std::ostream &os, const char delim, const T &val, Fn && getter, std::index_sequence<Is...> )
{
    return detail::stream_write_values( os, delim, getter.template operator()<Is>(val)... );
}

template<typename T, typename Fn, size_t ... Is>
static bool stream_read_indexed( std::istream &is, const char delim, T &val, Fn && getter, std::index_sequence<Is...> )
{
    return detail::stream_read_values( is, delim, getter.template operator()<Is>(val)... );
}

} // namespace detail

// Converters

namespace converters
{
template<typename T>
struct basic_converter {

    using value_type = T;

    static void configure_stream(std::ios_base& ss) {
        ss.imbue( std::locale::classic() );
        ss.setf( std::ios_base::showpoint );
        ss.setf( std::ios_base::dec, std::ostream::basefield );
    }

    static void configure_stream(std::ios_base& ss, const T& in_val) {
        configure_stream( ss );
        if constexpr( std::is_floating_point_v<T> ) {
            constexpr auto max_digits = std::numeric_limits<T>::digits10;
            constexpr auto max_repr = pow10<double, max_digits>();
            const auto float_flags = in_val >= max_repr ? std::ios_base::scientific : std::ios_base::fixed;
            ss.precision( max_digits );
            ss.setf(float_flags, std::ios_base::floatfield);
        }
    }

    bool operator()( const T &in_val, std::string &out_val ) const {
        std::ostringstream os;
        basic_converter::configure_stream(os, in_val);

        if( !detail::stream_write_values( os, ',', in_val ) ) {
            return false;
        }

        out_val = os.str();
        return true;
    };

    bool operator()( const std::string &in_val, T &out_val ) const {
        std::istringstream is( in_val );
        basic_converter::configure_stream(is);

        T tmp;
        if( !detail::stream_read_values( is, ',', tmp ) ) {
            return false;
        }

        out_val = tmp;
        return true;
    }
};

template<typename T>
struct json_converter {
    using value_type = T;

    // Json parser shits itself with 17 digits of precision

    bool operator()( const T &in_val, std::string &out_val ) const
    {
        std::ostringstream os;
        JsonOut jsout{os};
        jsout.write( in_val );
        out_val = os.str();
        return true;
    }

    bool operator()( const std::string &in_val, T &out_val ) const {
        std::istringstream is{in_val};
        JsonIn jsin{is};
        return jsin.read( out_val, false );
    }
};

} // namespace converters

namespace detail
{

// Fallback
template<typename T>
struct stream_value_io {
    bool operator()(std::ostream &ss, const char, const T &val) const {
        if constexpr (has_ostream_write<T>) {
            ss << val;
            return true;
        } else {
            []<bool f = false>(){ static_assert(f, "no matching ostream operator for type"); }();
            return false;
        }
    }

    bool operator()(std::istream& ss, const char, T& val) {
        if constexpr (has_istream_read<T>) {
            ss >> val;
            return true;
        } else {
            []<bool f = false>(){ static_assert(f, "no matching istream operator for type"); }();
            return false;
        }
    }
};

// String

template<>
struct stream_value_io<std::string> {
    using T = std::string;

    static constexpr std::string RESERVED_CHARS = "[],\"";

    bool operator()(std::ostream &os, const char, const T &val) const {
        os << std::quoted(val);
        return true;
    }

    bool operator()(std::istream &is, const char, T &val) const {
        is >> std::quoted(val);
        return true;
    }
};

// Numeric

template<is_numeric T>
struct stream_value_io<T> {
    bool operator()(std::ostream &ss, const char, const T &val) const {
        ss << val;
        return true;
    }

    bool operator()(std::istream &ss, const char, T &val) const {
        ss >> val;
        return true;
    }
};

// Tripoint

template<>
struct stream_value_io<tripoint> {
    bool operator()(std::ostream &ss, const char delim, const tripoint &val) const {
        return stream_write_values(ss, delim, val.x, val.y, val.z);
    }
    bool operator()(std::istream &ss, const char delim, tripoint &val) const {
        return stream_read_values(ss, delim, val.x, val.y, val.z);
    }
};

// Point

template<>
struct stream_value_io<point> {
    bool operator()(std::ostream &ss, const char delim, const point &val) const {
        return stream_write_values(ss, delim, val.x, val.y);
    }
    bool operator()(std::istream &ss, const char delim, point &val) const {
        return stream_read_values(ss, delim, val.x, val.y);
    }
};

// Tuple-Like
template<is_tuple_like T>
struct stream_value_io<T> {
    struct tuple_get {
        template<size_t I>
        auto& operator()(T& val) { return std::get<I>(val); }

        template<size_t I>
        const auto& operator()(const T& val) { return std::get<I>(val); }
    };

    bool operator()(std::ostream &os, const char delim, const T &val) const {
        os << '[';
        detail::stream_write_indexed(os, delim, val, tuple_get{} , std::make_index_sequence<std::tuple_size_v<T>>());
        os << ']';
        return true;
    }
    bool operator()(std::istream &is, const char delim, T &val) const {
        char c;
        is >> c >> std::ws;
        if (c != '[')
            return false;
        detail::stream_read_indexed(is, delim, val, tuple_get{}, std::make_index_sequence<std::tuple_size_v<T>>());
        is >> c >> std::ws;
        if (c != ']')
            return false;
        return true;
    }
};

} // namespace detail

template<typename T>
struct type_converter {
    using type = converters::json_converter<T>;
};

} // namespace data_vars
