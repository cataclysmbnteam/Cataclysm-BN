#pragma once
#ifndef CATA_TESTS_MAP_SETUP_HELPERS_H
#define CATA_TESTS_MAP_SETUP_HELPERS_H

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "point.h"
#include "optional.h"

namespace map_helpers
{
struct canvas_legend {
    private:
        std::unordered_map<char32_t, std::string> data;

    public:
        canvas_legend( std::unordered_map<char32_t, std::string> &&data ) : data( data ) {}
        ~canvas_legend() = default;

        static constexpr char32_t key_invalid = U'?';

        const std::string &entry( char32_t c ) const;
        char32_t key_for( const std::string &s ) const;
};

struct canvas {
    private:
        point size_cache;
        std::vector<std::u32string> data;

        point calc_size() const;

    public:
        canvas() = default;
        canvas( const point &size );
        canvas( std::vector<std::u32string> &&data ) : data( data ) {
            size_cache = calc_size();
            assert_size( size() );
        }
        canvas( const canvas & ) = default;
        canvas( canvas && ) = default;
        ~canvas() = default;

        bool operator==( const canvas &rhs ) const {
            return data == rhs.data;
        }

        inline const point &size() const {
            return size_cache;
        }

        void assert_size( const point &sz ) const;
        std::string to_string() const;

        inline void set( const point &p, char32_t val ) {
            assert( p.x < size().x );
            assert( p.y < size().y );
            data[p.y][p.x] = val;
        }
        inline char32_t get( const point &p ) const {
            assert( p.x < size().x );
            assert( p.y < size().y );
            return data[p.y][p.x];
        }

        std::vector<point> replace( char32_t what, char32_t with );
        point replace_unique( char32_t what, char32_t with );
        cata::optional<point> replace_opt( char32_t what, char32_t with );

        canvas rotated( int turns ) const;
};

struct canvas_adapter {
    private:
        const canvas_legend *l = nullptr;
        std::function<std::string( const point & )> getter;
        std::function<void( const point &, const std::string & )> setter;

    public:
        canvas_adapter() = default;
        canvas_adapter( const canvas_legend &l ) {
            with_legend( l );
        };
        ~canvas_adapter() = default;

        inline canvas_adapter &with_legend( const canvas_legend &l ) {
            this->l = &l;
            return *this;
        }
        inline canvas_adapter &with_getter( std::function<std::string( const point & )> f ) {
            getter = f;
            return *this;
        }
        inline canvas_adapter &with_setter( std::function<void( const point &, const std::string & )> f ) {
            setter = f;
            return *this;
        }

        void set_all( const canvas &c );

        canvas extract_to_canvas( const point &sz );

        void check_matches_expected( const canvas &expected, bool require );
};
} // namespace map_helpers

#endif // CATA_TESTS_MAP_SETUP_HELPERS_H
