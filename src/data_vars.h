#pragma once

#include <string>
#include <map>
#include <sstream>

#include "point.h"
#include "string_formatter.h"
#include "string_utils.h"

namespace detail
{

template<typename T>
struct data_var_sstream_converter {
    bool operator()( const T &in_val, std::string &out_val ) const {
        std::stringstream ss;
        ss.imbue( std::locale::classic() );
        ss << in_val;
        if( ss.fail() ) {
            return false;
        }
        out_val = ss.str();
        return true;
    };

    bool operator()( const std::string &in_val, T &out_val ) const {
        std::stringstream ss( in_val );
        ss.imbue( std::locale::classic() );
        ss >> out_val;
        if( ss.fail() ) {
            return false;
        }
        return true;
    }
};

struct data_var_tripoint_converter {
    bool operator()( const tripoint &in_val, std::string &out_val ) const {
        out_val = string_format( "%d,%d,%d", in_val.x, in_val.y, in_val.z );
        return true;
    }
    bool operator()( const std::string &in_val, tripoint &out_val ) const {
        const std::vector<std::string> values = string_split( in_val, ',' );
        constexpr data_var_sstream_converter<int> cvt;
        tripoint p;
        if( cvt( values[0], p.x ) && cvt( values[1], p.y ) && cvt( values[2], p.z ) ) {
            out_val = p;
            return true;
        };
        return false;
    }
};

template<typename T>
struct data_var_converter {
    using type = void;
};

template<typename T>
using data_var_converter_t = typename data_var_converter<T>::type;

template<typename NumType>
requires std::is_integral_v<NumType> or std::is_floating_point_v<NumType>
struct data_var_converter<NumType> {
    using type = data_var_sstream_converter<NumType>;
};

template<>
struct data_var_converter<tripoint> {
    using type = data_var_tripoint_converter;
};

} // namespace detail

class data_vars
{
    public:
        using storage = std::map<std::string, std::string>;
        using key_type = storage::key_type;
        using value_type = storage::value_type;
        using mapped_type = storage::mapped_type;
        using iterator = storage::iterator;
        using const_iterator = storage::const_iterator;

        storage data;

        template<typename T, typename Cvt = detail::data_var_converter_t<T> >
        void set( const key_type &name, const T &value, const Cvt &cvt = {} ) {
            std::string strval;
            if( cvt( value, strval ) ) {
                data_vars::set( name, strval );
            } else {
                throw std::runtime_error( "failed to convert value to string" );
            }
        }

        template<typename T, typename Cvt = detail::data_var_converter_t<T> >
        bool try_get( const key_type &name, T &value, const Cvt &cvt = {} ) const {
            std::string strval;
            if( !data_vars::try_get( name, strval ) ) {
                return false;
            }
            return cvt( strval, value );
        }

        template<typename T, typename Cvt = detail::data_var_converter_t<T> >
        T get( const key_type &name, const T &default_value = T{}, const Cvt &cvt = {} ) const {
            T value;
            return try_get<T>( name, value, cvt )
                   ? value
                   : default_value;
        }

        void set( const key_type &name, const mapped_type &value );
        mapped_type get( const key_type &name, const mapped_type &default_value = mapped_type{} ) const;
        bool try_get( const key_type &name, mapped_type &value ) const;

        bool operator==( const data_vars &other ) const { return ( data ) == other.data; }
        mapped_type &operator[]( const key_type &name ) { return data[name]; }

        bool empty() const { return data.empty(); }
        bool contains( const key_type &key ) const { return data.contains( key ); }
        iterator begin() { return data.begin(); }
        const_iterator begin() const { return data.begin(); }
        iterator end() { return data.end(); }
        const_iterator end() const { return data.end(); }
        iterator find( const key_type &key ) { return data.find( key ); }
        const_iterator find( const key_type &key ) const { return data.find( key ); }

        void clear() { data.clear(); }
        void erase( const key_type &name ) { data.erase( name ); }
        void erase( iterator it ) { data.erase( it ); }
};