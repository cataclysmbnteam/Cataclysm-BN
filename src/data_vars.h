#pragma once

#include <map>
#include <string>
#include "data_vars_cvt.h"

class data_vars
{
    public:
        using storage = std::map<std::string, std::string>;
        using key_type = storage::key_type;
        using value_type = storage::value_type;
        using mapped_type = storage::mapped_type;
        using iterator = storage::iterator;
        using const_iterator = storage::const_iterator;

        template<typename Value, typename Conv = detail::type_converter<Value>>
        bool try_get( const std::string &key, Value &out_val, const Conv &converter = {} ) const {
            const auto it = _data.find( key );
            if( it == _data.end() ) {
                return false;
            }
            return converter( it->second, out_val );
        }

        template <typename Value, typename Conv = detail::type_converter<Value>>
        Value get( const std::string &key, const Value &default_value, const Conv &converter = {} ) const {
            Value val;
            if( !data_vars::try_get<Value>( key, val, converter ) ) {
                return default_value;
            }
            return val;
        }

        template <typename Value, typename Conv = detail::type_converter<Value>>
        void set( const key_type &name, const Value &value, const Conv &converter = {} ) {
            std::string str;
            if( !converter( value, str ) ) {
                throw std::runtime_error( "failed to convert value to string" );
            }
            _data[name] = str;
        }

        bool try_get( const std::string &key, mapped_type &out_val ) const {
            const auto it = _data.find( key );
            if( it == _data.end() ) {
                return false;
            }
            out_val = it->second;
            return true;
        }

        mapped_type get( const std::string &key, const mapped_type &default_value ) const {
            std::string val;
            if( !try_get( key, val ) ) {
                return default_value;
            }
            return val;
        }

        void set( const key_type &name, const mapped_type &value ) {
            _data[name] = value;
        }

        bool operator==( const data_vars &other ) const { return ( _data ) == other._data; }
        mapped_type &operator[]( const key_type &name ) { return _data[name]; }

        bool empty() const { return _data.empty(); }
        bool contains( const key_type &key ) const { return _data.contains( key ); }
        iterator begin() { return _data.begin(); }
        const_iterator begin() const { return _data.begin(); }
        iterator end() { return _data.end(); }
        const_iterator end() const { return _data.end(); }
        iterator find( const key_type &key ) { return _data.find( key ); }
        const_iterator find( const key_type &key ) const { return _data.find( key ); }

        void clear() { _data.clear(); }
        void erase( const key_type &name ) { _data.erase( name ); }
        void erase( const iterator it ) { _data.erase( it ); }

    private:
        storage _data;
};