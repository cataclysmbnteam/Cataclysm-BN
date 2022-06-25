#include "map_setup_helpers.h"

#include "catch/catch.hpp"
#include "catacharset.h"
#include "string_formatter.h"

namespace map_helpers
{
const std::string &canvas_legend::entry( char32_t c ) const
{
    auto it = data.find( c );
    assert( it != data.end() );
    return it->second;
}

char32_t canvas_legend::key_for( const std::string &s ) const
{
    for( const auto &it : data ) {
        if( it.second == s ) {
            return it.first;
        }
    }
    return key_invalid;
}

canvas::canvas( const point &size )
{
    size_cache = size;
    data.resize( size.y, std::u32string( size.x, canvas_legend::key_invalid ) );
}

point canvas::calc_size() const
{
    if( data.empty() ) {
        return point_zero;
    } else {
        return point(
                   static_cast<int>( data[0].size() ),
                   static_cast<int>( data.size() )
               );
    }
}

void canvas::assert_size( const point &sz ) const
{
    assert( static_cast<int>( data.size() ) == sz.y );
    for( const auto &line : data ) {
        assert( static_cast<int>( line.size() ) == sz.x );
    }
}

std::string canvas::to_string() const
{
    std::string res;
    res += "\n";
    for( const auto &line : data ) {
        res += utf32_to_utf8( line );
        res += "\n";
    }
    return res;
}

std::vector<point> canvas::replace( char32_t what, char32_t with )
{
    std::vector<point> ret;
    point p;
    for( p.y = 0; p.y < size().y; p.y++ ) {
        for( p.x = 0; p.x < size().x; p.x++ ) {
            if( get( p ) == what ) {
                set( p, with );
                ret.push_back( p );
            }
        }
    }
    return ret;
}

point canvas::replace_unique( char32_t what, char32_t with )
{
    std::vector<point> candidates = replace( what, with );
    assert( candidates.size() == 1 );
    return candidates[0];
}

cata::optional<point> canvas::replace_opt( char32_t what, char32_t with )
{
    std::vector<point> candidates = replace( what, with );
    assert( candidates.size() <= 1 );
    if( candidates.empty() ) {
        return cata::nullopt;
    } else {
        return candidates.front();
    }
}

canvas canvas::rotated( int turns ) const
{
    const point new_size = size_cache.rotate( turns ).abs();
    canvas ret( new_size );
    point p;
    for( p.y = 0; p.y < size().y; p.y++ ) {
        for( p.x = 0; p.x < size().x; p.x++ ) {
            point new_p = p.rotate( turns, size_cache );
            ret.set( new_p, get( p ) );
        }
    }
    return ret;
}

void canvas_adapter::set_all( const canvas &c )
{
    assert( l );
    assert( setter );
    point p;
    for( p.y = 0; p.y < c.size().y; p.y++ ) {
        for( p.x = 0; p.x < c.size().x; p.x++ ) {
            setter( p, l->entry( c.get( p ) ) );
        }
    }
}

canvas canvas_adapter::extract_to_canvas( const point &sz )
{
    assert( l );
    assert( getter );
    canvas ret( sz );
    point p;
    for( p.y = 0; p.y < sz.y; p.y++ ) {
        for( p.x = 0; p.x < sz.x; p.x++ ) {
            ret.set( p, l->key_for( getter( p ) ) );
        }
    }
    return ret;
}

void canvas_adapter::check_matches_expected( const canvas &expected, bool require )
{
    struct mismatch_entry {
        point p;
        std::string exp;
        std::string got;
    };

    assert( l );
    assert( getter );

    std::vector<mismatch_entry> fails;
    point p;
    for( p.y = 0; p.y < expected.size().y; p.y++ ) {
        for( p.x = 0; p.x < expected.size().x; p.x++ ) {
            const std::string &exp = l->entry( expected.get( p ) );
            std::string got = getter( p );
            if( exp != got ) {
                fails.push_back( { p, exp, std::move( got ) } );
            }
        }
    }

    if( !fails.empty() ) {
        std::string fails_string = "\n";
        for( const auto &e : fails ) {
            fails_string += string_format( "%s exp:%s got:%s\n", e.p.to_string(), e.exp, e.got );
        }

        canvas canvas_got = extract_to_canvas( expected.size() );

        CAPTURE( expected.to_string() );
        CAPTURE( canvas_got.to_string() );
        CAPTURE( fails.size() );
        CAPTURE( fails_string );

        if( require ) {
            FAIL();
        } else {
            FAIL_CHECK();
        }
    } else {
        SUCCEED();
    }
}

} // namespace map_helpers

TEST_CASE( "map_test_setup_canvas_rotation", "[utility]" )
{
    SECTION( "even_sides" ) {
        map_helpers::canvas canvas = { {
                U"..",
                U"ab",
                U"cd",
                U"..",
            }
        };

        REQUIRE( canvas.rotated( 0 ) == canvas );
        {
            map_helpers::canvas exp = { {
                    U".ca.",
                    U".db.",
                }
            };
            REQUIRE( canvas.rotated( 1 ) == exp );
        }
        {
            map_helpers::canvas exp = { {
                    U"..",
                    U"dc",
                    U"ba",
                    U".."
                }
            };
            REQUIRE( canvas.rotated( 2 ) == exp );
        }
        {
            map_helpers::canvas exp = { {
                    U".bd.",
                    U".ac.",
                }
            };
            REQUIRE( canvas.rotated( 3 ) == exp );
        }
        REQUIRE( canvas.rotated( 4 ) == canvas );
    }
    SECTION( "not_even_sides" ) {
        map_helpers::canvas canvas = { {
                U"...",
                U"abc",
                U"...",
                U"def",
                U"...",
            }
        };

        REQUIRE( canvas.rotated( 0 ) == canvas );
        {
            map_helpers::canvas exp = { {
                    U".d.a.",
                    U".e.b.",
                    U".f.c.",
                }
            };
            REQUIRE( canvas.rotated( 1 ) == exp );
        }
        {
            map_helpers::canvas exp = { {
                    U"...",
                    U"fed",
                    U"...",
                    U"cba",
                    U"..."
                }
            };
            REQUIRE( canvas.rotated( 2 ) == exp );
        }
        {
            map_helpers::canvas exp = { {
                    U".c.f.",
                    U".b.e.",
                    U".a.d.",
                }
            };
            REQUIRE( canvas.rotated( 3 ) == exp );
        }
        REQUIRE( canvas.rotated( 4 ) == canvas );
    }
}
