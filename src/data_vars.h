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
    std::string operator()( const T &value ) const {
        std::stringstream ss;
        ss.imbue( std::locale::classic() );
        ss << value;
        if( ss.fail() ) {
            return "";
        }
        return ss.str();
    };

    T operator()( const std::string& key, const T &def ) const {
        T value = def;
        std::stringstream ss( key );
        ss.imbue( std::locale::classic() );
        ss >> value;
        if( ss.fail() ) {
            return def;
        }
        return value;
    }
};

struct data_var_tripoint_converter {
    std::string operator()( const tripoint &value ) const {
        return string_format( "%d,%d,%d", value.x, value.y, value.z );
    }
    tripoint operator()( const std::string &value, const tripoint & ) const {
        const std::vector<std::string> values = string_split( value, ',' );
        return tripoint( std::stoi( values[0] ),
                         std::stoi( values[1] ),
                         std::stoi( values[2] ) );
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
        void set( const key_type& name, const T& value, const Cvt& cvt = {} ) {
            data[name] = cvt( value );
        }

        template<typename T, typename Cvt = detail::data_var_converter_t<T> >
        T get( const key_type& name, const T& default_value = T{}, const Cvt& cvt = {} ) const {
            const auto it = find( name );
            if( it == end() ) {
                return default_value;
            }
            return static_cast<T>( cvt( it->second, default_value ) );
        }

        void set( const key_type& name, const mapped_type& value ) {
            data[name] = value;
        }

        mapped_type get( const key_type& name, const mapped_type& default_value = mapped_type{} ) const {
            const auto it = find( name );
            if( it == end() ) {
                return default_value;
            }
            return it->second;
        }

        bool operator==( const data_vars & other ) const { return ( data ) == other.data; }
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