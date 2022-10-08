#include "json.h"

#include <algorithm>
#include <bitset>
#include <cmath> // pow
#include <cstdint>
#include <cstdio>
#include <cstdlib> // strtoul
#include <cstring> // strcmp
#include <exception>
#include <iterator>
#include <limits>
#include <locale> // ensure user's locale doesn't interfere with output
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "cached_options.h"
#include "debug.h"
#include "string_formatter.h"
#include "string_utils.h"

// JSON parsing and serialization tools for Cataclysm-DDA.
// For documentation, see the included header, json.h.

static auto is_whitespace( char ch ) -> bool
{
    // These are all the valid whitespace characters allowed by RFC 4627.
    return ( ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r' );
}

// for parsing \uxxxx escapes
static auto utf16_to_utf8( uint32_t ch ) -> std::string
{
    char out[5];
    char *buf = out;
    static const unsigned char utf8FirstByte[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
    int utf8Bytes;
    if( ch < 0x80 ) {
        utf8Bytes = 1;
    } else if( ch < 0x800 ) {
        utf8Bytes = 2;
    } else if( ch < 0x10000 ) {
        utf8Bytes = 3;
    } else if( ch <= 0x10FFFF ) {
        utf8Bytes = 4;
    } else {
        std::stringstream err;
        err << "unknown unicode: " << ch;
        throw std::runtime_error( err.str() );
    }

    buf += utf8Bytes;
    switch( utf8Bytes ) {
        case 4:
            *--buf = ( ch | 0x80 ) & 0xBF;
            ch >>= 6;
        /* fallthrough */
        case 3:
            *--buf = ( ch | 0x80 ) & 0xBF;
            ch >>= 6;
        /* fallthrough */
        case 2:
            *--buf = ( ch | 0x80 ) & 0xBF;
            ch >>= 6;
        /* fallthrough */
        case 1:
            *--buf = ch | utf8FirstByte[utf8Bytes];
    }
    out[utf8Bytes] = '\0';
    return out;
}

/* class JsonObject
 * represents a JSON object,
 * providing access to the underlying data.
 */
JsonObject::JsonObject( JsonIn &j )
{
    jsin = &j;
    start = jsin->tell();
    // cache the position of the value for each member
    jsin->start_object();
    while( !jsin->end_object() ) {
        std::string n = jsin->get_member_name();
        int p = jsin->tell();
        if( positions.count( n ) > 0 ) {
            j.error( "duplicate entry in json object" );
        }
        positions[n] = p;
        jsin->skip_value();
    }
    end_ = jsin->tell();
    final_separator = jsin->get_ate_separator();
}

void JsonObject::mark_visited( const std::string &name ) const
{
#ifndef CATA_IN_TOOL
    visited_members.emplace( name );
#else
    static_cast<void>( name );
#endif
}

void JsonObject::report_unvisited() const
{
#ifndef CATA_IN_TOOL
    if(
        json_report_strict
        && report_unvisited_members
        && !reported_unvisited_members
        && !std::uncaught_exception()
    ) {
        reported_unvisited_members = true;
        for( const std::pair<const std::string, int> &p : positions ) {
            const std::string &name = p.first;
            if( !visited_members.count( name ) && !string_starts_with( name, "//" ) ) {
                try {
                    throw_error( string_format( "Invalid or misplaced field name \"%s\" in JSON data", name ), name );
                } catch( const JsonError &e ) {
                    debugmsg( "(json-error)\n%s", e.what() );
                }
            }
        }
    }
#endif
}

void JsonObject::finish()
{
    report_unvisited();
    if( jsin && jsin->good() ) {
        jsin->seek( end_ );
        jsin->set_ate_separator( final_separator );
    }
}

auto JsonObject::size() const -> size_t
{
    return positions.size();
}
auto JsonObject::empty() const -> bool
{
    return positions.empty();
}

void JsonObject::allow_omitted_members() const
{
#ifndef CATA_IN_TOOL
    report_unvisited_members = false;
#endif
}

auto JsonObject::verify_position( const std::string &name,
                                 const bool throw_exception ) const -> int
{
    if( !jsin ) {
        if( throw_exception ) {
            throw JsonError( std::string( "member lookup on empty object: " ) + name );
        }
        // 0 is always the opening brace,
        // so it will never indicate a valid member position
        return 0;
    }
    const auto iter = positions.find( name );
    if( iter == positions.end() ) {
        if( throw_exception ) {
            jsin->seek( start );
            jsin->error( "member not found: " + name );
        }
        // 0 is always the opening brace,
        // so it will never indicate a valid member position
        return 0;
    }
    return iter->second;
}

auto JsonObject::has_member( const std::string &name ) const -> bool
{
    return positions.count( name ) > 0;
}

auto JsonObject::line_number() const -> std::string
{
    jsin->seek( start );
    return jsin->line_number();
}

auto JsonObject::str() const -> std::string
{
    // If we're getting the string form, we might be re-parsing later, so don't
    // complain about unvisited members.
    allow_omitted_members();

    if( jsin && end_ >= start ) {
        return jsin->substr( start, end_ - start );
    } else {
        return "{}";
    }
}

void JsonObject::throw_error( std::string err, const std::string &name ) const
{
    if( !jsin ) {
        throw JsonError( err );
    }
    jsin->seek( verify_position( name, false ) );
    jsin->error( err );
}

void JsonArray::throw_error( std::string err )
{
    if( !jsin ) {
        throw JsonError( err );
    }
    jsin->error( err );
}

void JsonArray::throw_error( std::string err, int idx )
{
    if( !jsin ) {
        throw JsonError( err );
    }
    if( idx >= 0 && size_t( idx ) < positions.size() ) {
        jsin->seek( positions[idx] );
    }
    jsin->error( err );
}

void JsonObject::throw_error( std::string err ) const
{
    if( !jsin ) {
        throw JsonError( err );
    }
    jsin->error( err );
}

auto JsonObject::get_raw( const std::string &name ) const -> JsonIn *
{
    int pos = verify_position( name );
    mark_visited( name );
    jsin->seek( pos );
    return jsin;
}

auto JsonObject::get_source_location() const -> json_source_location
{
    if( !jsin ) {
        throw JsonError( "JsonObject::get_source_location called when stream is null" );
    }
    json_source_location loc;
    loc.path = jsin->get_path();
    if( !loc.path ) {
        jsin->seek( start );
        jsin->error( "JsonObject::get_source_location called but the path is unknown" );
    }
    loc.offset = start;
    return loc;
}

/* returning values by name */

auto JsonObject::get_bool( const std::string &name ) const -> bool
{
    return get_member( name ).get_bool();
}

auto JsonObject::get_bool( const std::string &name, const bool fallback ) const -> bool
{
    int pos = verify_position( name, false );
    if( !pos ) {
        return fallback;
    }
    mark_visited( name );
    jsin->seek( pos );
    return jsin->get_bool();
}

auto JsonObject::get_int( const std::string &name ) const -> int
{
    return get_member( name ).get_int();
}

auto JsonObject::get_int( const std::string &name, const int fallback ) const -> int
{
    int pos = verify_position( name, false );
    if( !pos ) {
        return fallback;
    }
    mark_visited( name );
    jsin->seek( pos );
    return jsin->get_int();
}

auto JsonObject::get_float( const std::string &name ) const -> double
{
    return get_member( name ).get_float();
}

auto JsonObject::get_float( const std::string &name, const double fallback ) const -> double
{
    int pos = verify_position( name, false );
    if( !pos ) {
        return fallback;
    }
    mark_visited( name );
    jsin->seek( pos );
    return jsin->get_float();
}

auto JsonObject::get_string( const std::string &name ) const -> std::string
{
    return get_member( name ).get_string();
}

auto JsonObject::get_string( const std::string &name, const std::string &fallback ) const -> std::string
{
    int pos = verify_position( name, false );
    if( !pos ) {
        return fallback;
    }
    mark_visited( name );
    jsin->seek( pos );
    return jsin->get_string();
}

/* returning containers by name */

auto JsonObject::get_array( const std::string &name ) const -> JsonArray
{
    int pos = verify_position( name, false );
    if( !pos ) {
        return JsonArray();
    }
    mark_visited( name );
    jsin->seek( pos );
    return JsonArray( *jsin );
}

auto JsonObject::get_int_array( const std::string &name ) const -> std::vector<int>
{
    std::vector<int> ret;
    for( const int entry : get_array( name ) ) {
        ret.push_back( entry );
    }
    return ret;
}

auto JsonObject::get_string_array( const std::string &name ) const -> std::vector<std::string>
{
    std::vector<std::string> ret;
    for( const std::string entry : get_array( name ) ) {
        ret.push_back( entry );
    }
    return ret;
}

auto JsonObject::get_object( const std::string &name ) const -> JsonObject
{
    int pos = verify_position( name, false );
    if( !pos ) {
        return JsonObject();
    }
    mark_visited( name );
    jsin->seek( pos );
    return jsin->get_object();
}

/* non-fatal member existence and type testing */

auto JsonObject::has_null( const std::string &name ) const -> bool
{
    int pos = verify_position( name, false );
    if( !pos ) {
        return false;
    }
    mark_visited( name );
    jsin->seek( pos );
    return jsin->test_null();
}

auto JsonObject::has_bool( const std::string &name ) const -> bool
{
    int pos = verify_position( name, false );
    if( !pos ) {
        return false;
    }
    jsin->seek( pos );
    return jsin->test_bool();
}

auto JsonObject::has_number( const std::string &name ) const -> bool
{
    int pos = verify_position( name, false );
    if( !pos ) {
        return false;
    }
    jsin->seek( pos );
    return jsin->test_number();
}

auto JsonObject::has_string( const std::string &name ) const -> bool
{
    int pos = verify_position( name, false );
    if( !pos ) {
        return false;
    }
    jsin->seek( pos );
    return jsin->test_string();
}

auto JsonObject::has_array( const std::string &name ) const -> bool
{
    int pos = verify_position( name, false );
    if( !pos ) {
        return false;
    }
    jsin->seek( pos );
    return jsin->test_array();
}

auto JsonObject::has_object( const std::string &name ) const -> bool
{
    int pos = verify_position( name, false );
    if( !pos ) {
        return false;
    }
    jsin->seek( pos );
    return jsin->test_object();
}

/* class JsonArray
 * represents a JSON array,
 * providing access to the underlying data.
 */
JsonArray::JsonArray( JsonIn &j )
{
    jsin = &j;
    start = jsin->tell();
    index = 0;
    // cache the position of each element
    jsin->start_array();
    while( !jsin->end_array() ) {
        positions.push_back( jsin->tell() );
        jsin->skip_value();
    }
    end_ = jsin->tell();
    final_separator = jsin->get_ate_separator();
}

JsonArray::JsonArray( const JsonArray &ja )
{
    jsin = ja.jsin;
    start = ja.start;
    index = 0;
    positions = ja.positions;
    end_ = ja.end_;
    final_separator = ja.final_separator;
}

auto JsonArray::operator=( const JsonArray &ja ) -> JsonArray &
{
    jsin = ja.jsin;
    start = ja.start;
    index = 0;
    positions = ja.positions;
    end_ = ja.end_;
    final_separator = ja.final_separator;

    return *this;
}

void JsonArray::finish()
{
    if( jsin && jsin->good() ) {
        jsin->seek( end_ );
        jsin->set_ate_separator( final_separator );
    }
}

auto JsonArray::has_more() const -> bool
{
    return index < positions.size();
}
auto JsonArray::size() const -> size_t
{
    return positions.size();
}
auto JsonArray::empty() -> bool
{
    return positions.empty();
}

auto JsonArray::str() -> std::string
{
    if( jsin ) {
        return jsin->substr( start, end_ - start );
    } else {
        return "[]";
    }
}

void JsonArray::verify_index( const size_t i ) const
{
    if( !jsin ) {
        throw JsonError( "tried to access empty array." );
    } else if( i >= positions.size() ) {
        jsin->seek( start );
        std::stringstream err;
        err << "bad index value: " << i;
        jsin->error( err.str() );
    }
}

/* iterative access */

auto JsonArray::next_bool() -> bool
{
    verify_index( index );
    jsin->seek( positions[index++] );
    return jsin->get_bool();
}

auto JsonArray::next_int() -> int
{
    verify_index( index );
    jsin->seek( positions[index++] );
    return jsin->get_int();
}

auto JsonArray::next_float() -> double
{
    verify_index( index );
    jsin->seek( positions[index++] );
    return jsin->get_float();
}

auto JsonArray::next_string() -> std::string
{
    verify_index( index );
    jsin->seek( positions[index++] );
    return jsin->get_string();
}

auto JsonArray::next_array() -> JsonArray
{
    verify_index( index );
    jsin->seek( positions[index++] );
    return jsin->get_array();
}

auto JsonArray::next_object() -> JsonObject
{
    verify_index( index );
    jsin->seek( positions[index++] );
    return jsin->get_object();
}

void JsonArray::skip_value()
{
    verify_index( index );
    ++index;
}

/* static access */

auto JsonArray::get_bool( const size_t i ) const -> bool
{
    verify_index( i );
    jsin->seek( positions[i] );
    return jsin->get_bool();
}

auto JsonArray::get_int( const size_t i ) const -> int
{
    verify_index( i );
    jsin->seek( positions[i] );
    return jsin->get_int();
}

auto JsonArray::get_float( const size_t i ) const -> double
{
    verify_index( i );
    jsin->seek( positions[i] );
    return jsin->get_float();
}

auto JsonArray::get_string( const size_t i ) const -> std::string
{
    verify_index( i );
    jsin->seek( positions[i] );
    return jsin->get_string();
}

auto JsonArray::get_array( const size_t i ) const -> JsonArray
{
    verify_index( i );
    jsin->seek( positions[i] );
    return jsin->get_array();
}

auto JsonArray::get_object( const size_t i ) const -> JsonObject
{
    verify_index( i );
    jsin->seek( positions[i] );
    return jsin->get_object();
}

/* iterative type checking */

auto JsonArray::test_null() const -> bool
{
    if( !has_more() ) {
        return false;
    }
    jsin->seek( positions[index] );
    return jsin->test_null();
}

auto JsonArray::test_bool() const -> bool
{
    if( !has_more() ) {
        return false;
    }
    jsin->seek( positions[index] );
    return jsin->test_bool();
}

auto JsonArray::test_number() const -> bool
{
    if( !has_more() ) {
        return false;
    }
    jsin->seek( positions[index] );
    return jsin->test_number();
}

auto JsonArray::test_string() const -> bool
{
    if( !has_more() ) {
        return false;
    }
    jsin->seek( positions[index] );
    return jsin->test_string();
}

auto JsonArray::test_bitset() const -> bool
{
    if( !has_more() ) {
        return false;
    }
    jsin->seek( positions[index] );
    return jsin->test_bitset();
}

auto JsonArray::test_array() const -> bool
{
    if( !has_more() ) {
        return false;
    }
    jsin->seek( positions[index] );
    return jsin->test_array();
}

auto JsonArray::test_object() const -> bool
{
    if( !has_more() ) {
        return false;
    }
    jsin->seek( positions[index] );
    return jsin->test_object();
}

/* random-access type checking */

auto JsonArray::has_null( const size_t i ) const -> bool
{
    verify_index( i );
    jsin->seek( positions[i] );
    return jsin->test_null();
}

auto JsonArray::has_bool( const size_t i ) const -> bool
{
    verify_index( i );
    jsin->seek( positions[i] );
    return jsin->test_bool();
}

auto JsonArray::has_number( const size_t i ) const -> bool
{
    verify_index( i );
    jsin->seek( positions[i] );
    return jsin->test_number();
}

auto JsonArray::has_string( const size_t i ) const -> bool
{
    verify_index( i );
    jsin->seek( positions[i] );
    return jsin->test_string();
}

auto JsonArray::has_array( const size_t i ) const -> bool
{
    verify_index( i );
    jsin->seek( positions[i] );
    return jsin->test_array();
}

auto JsonArray::has_object( const size_t i ) const -> bool
{
    verify_index( i );
    jsin->seek( positions[i] );
    return jsin->test_object();
}

void add_array_to_set( std::set<std::string> &s, const JsonObject &json, const std::string &name )
{
    for( const std::string line : json.get_array( name ) ) {
        s.insert( line );
    }
}

auto JsonIn::tell() -> int
{
    return stream->tellg();
}
auto JsonIn::peek() -> char
{
    return static_cast<char>( stream->peek() );
}
auto JsonIn::good() -> bool
{
    return stream->good();
}

void JsonIn::seek( int pos )
{
    stream->clear();
    stream->seekg( pos );
    ate_separator = false;
}

void JsonIn::eat_whitespace()
{
    while( is_whitespace( peek() ) ) {
        stream->get();
    }
}

void JsonIn::uneat_whitespace()
{
    while( tell() > 0 ) {
        stream->seekg( -1, std::istream::cur );
        if( !is_whitespace( peek() ) ) {
            break;
        }
    }
}

void JsonIn::end_value()
{
    ate_separator = false;
    skip_separator();
}

void JsonIn::skip_member()
{
    skip_string();
    skip_pair_separator();
    skip_value();
}

void JsonIn::skip_separator()
{
    eat_whitespace();
    signed char ch = peek();
    if( ch == ',' ) {
        if( ate_separator ) {
            error( "duplicate comma" );
        }
        stream->get();
        ate_separator = true;
    } else if( ch == ']' || ch == '}' || ch == ':' ) {
        // okay
        if( ate_separator ) {
            std::stringstream err;
            err << "comma should not be found before '" << ch << "'";
            uneat_whitespace();
            error( err.str() );
        }
        ate_separator = false;
    } else if( ch == EOF ) {
        // that's okay too... probably
        if( ate_separator ) {
            uneat_whitespace();
            error( "comma at end of file not allowed" );
        }
        ate_separator = false;
    } else {
        // not okay >:(
        uneat_whitespace();
        error( "missing comma", 1 );
    }
}

void JsonIn::skip_pair_separator()
{
    char ch;
    eat_whitespace();
    stream->get( ch );
    if( ch != ':' ) {
        std::stringstream err;
        err << "expected pair separator ':', not '" << ch << "'";
        error( err.str(), -1 );
    } else if( ate_separator ) {
        error( "duplicate pair separator ':' not allowed", -1 );
    }
    ate_separator = true;
}

void JsonIn::skip_string()
{
    char ch;
    eat_whitespace();
    stream->get( ch );
    if( ch != '"' ) {
        std::stringstream err;
        err << "expecting string but found '" << ch << "'";
        error( err.str(), -1 );
    }
    while( stream->good() ) {
        stream->get( ch );
        if( ch == '\\' ) {
            stream->get( ch );
            continue;
        } else if( ch == '"' ) {
            break;
        } else if( ch == '\r' || ch == '\n' ) {
            error( "string not closed before end of line", -1 );
        }
    }
    end_value();
}

void JsonIn::skip_value()
{
    eat_whitespace();
    char ch = peek();
    // it's either a string '"'
    if( ch == '"' ) {
        skip_string();
        // or an object '{'
    } else if( ch == '{' ) {
        skip_object();
        // or an array '['
    } else if( ch == '[' ) {
        skip_array();
        // or a number (-0123456789)
    } else if( ch == '-' || ( ch >= '0' && ch <= '9' ) ) {
        skip_number();
        // or "true", "false" or "null"
    } else if( ch == 't' ) {
        skip_true();
    } else if( ch == 'f' ) {
        skip_false();
    } else if( ch == 'n' ) {
        skip_null();
        // or an error.
    } else {
        std::stringstream err;
        err << "expected JSON value but got '" << ch << "'";
        error( err.str() );
    }
    // skip_* end value automatically
}

void JsonIn::skip_object()
{
    start_object();
    while( !end_object() ) {
        skip_member();
    }
    // end_value called by end_object
}

void JsonIn::skip_array()
{
    start_array();
    while( !end_array() ) {
        skip_value();
    }
    // end_value called by end_array
}

void JsonIn::skip_true()
{
    char text[5];
    eat_whitespace();
    stream->get( text, 5 );
    if( strcmp( text, "true" ) != 0 ) {
        std::stringstream err;
        err << R"(expected "true", but found ")" << text << "\"";
        error( err.str(), -4 );
    }
    end_value();
}

void JsonIn::skip_false()
{
    char text[6];
    eat_whitespace();
    stream->get( text, 6 );
    if( strcmp( text, "false" ) != 0 ) {
        std::stringstream err;
        err << R"(expected "false", but found ")" << text << "\"";
        error( err.str(), -5 );
    }
    end_value();
}

void JsonIn::skip_null()
{
    char text[5];
    eat_whitespace();
    stream->get( text, 5 );
    if( strcmp( text, "null" ) != 0 ) {
        std::stringstream err;
        err << R"(expected "null", but found ")" << text << "\"";
        error( err.str(), -4 );
    }
    end_value();
}

void JsonIn::skip_number()
{
    char ch;
    eat_whitespace();
    // skip all of (+-0123456789.eE)
    while( stream->good() ) {
        stream->get( ch );
        if( ch != '+' && ch != '-' && ( ch < '0' || ch > '9' ) &&
            ch != 'e' && ch != 'E' && ch != '.' ) {
            stream->unget();
            break;
        }
    }
    end_value();
}

auto JsonIn::get_member_name() -> std::string
{
    std::string s = get_string();
    skip_pair_separator();
    return s;
}

static auto get_escaped_or_unicode( std::istream &stream, std::string &s, std::string &err ) -> bool
{
    if( !stream.good() ) {
        err = "stream not good";
        return false;
    }
    char ch;
    stream.get( ch );
    if( !stream.good() ) {
        err = "read operation failed";
        return false;
    }
    if( ch == '\\' ) {
        // converting \", \\, \/, \b, \f, \n, \r, \t and \uxxxx according to JSON spec.
        stream.get( ch );
        if( !stream.good() ) {
            err = "read operation failed";
            return false;
        }
        switch( ch ) {
            case '\\':
                s += '\\';
                break;
            case '"':
                s += '"';
                break;
            case '/':
                s += '/';
                break;
            case 'b':
                s += '\b';
                break;
            case 'f':
                s += '\f';
                break;
            case 'n':
                s += '\n';
                break;
            case 'r':
                s += '\r';
                break;
            case 't':
                s += '\t';
                break;
            case 'u': {
                    uint32_t u = 0;
                    for( int i = 0; i < 4; ++i ) {
                        stream.get( ch );
                        if( !stream.good() ) {
                            err = "read operation failed";
                            return false;
                        }
                        if( ch >= '0' && ch <= '9' ) {
                            u = ( u << 4 ) | ( ch - '0' );
                        } else if( ch >= 'a' && ch <= 'f' ) {
                            u = ( u << 4 ) | ( ch - 'a' + 10 );
                        } else if( ch >= 'A' && ch <= 'F' ) {
                            u = ( u << 4 ) | ( ch - 'A' + 10 );
                        } else {
                            err = "expected hex digit";
                            return false;
                        }
                    }
                    try {
                        s += utf16_to_utf8( u );
                    } catch( const std::exception &e ) {
                        err = e.what();
                        return false;
                    }
                }
                break;
            default:
                err = "invalid escape sequence";
                return false;
        }
    } else if( ch == '\r' || ch == '\n' ) {
        err = "reached end of line without closing string";
        return false;
    } else if( ch == '"' ) {
        // the caller is supposed to handle the ending quote
        err = "unexpected ending quote";
        return false;
    } else if( static_cast<unsigned char>( ch ) < 0x20 ) {
        err = "invalid character inside string";
        return false;
    } else {
        unsigned char uc = static_cast<unsigned char>( ch );
        uint32_t unicode = 0;
        int n = 0;
        if( uc >= 0xFC ) {
            unicode = uc & 0x01;
            n = 5;
        } else if( uc >= 0xF8 ) {
            unicode = uc & 0x03;
            n = 4;
        } else if( uc >= 0xF0 ) {
            unicode = uc & 0x07;
            n = 3;
        } else if( uc >= 0xE0 ) {
            unicode = uc & 0x0f;
            n = 2;
        } else if( uc >= 0xC0 ) {
            unicode = uc & 0x1f;
            n = 1;
        } else if( uc >= 0x80 ) {
            err = "invalid utf8 sequence";
            return false;
        } else {
            unicode = uc;
            n = 0;
        }
        s += ch;
        for( ; n > 0; --n ) {
            stream.get( ch );
            if( !stream.good() ) {
                err = "read operation failed";
                return false;
            }
            uc = static_cast<unsigned char>( ch );
            if( uc < 0x80 || uc >= 0xC0 ) {
                err = "invalid utf8 sequence";
                return false;
            }
            unicode = ( unicode << 6 ) | ( uc & 0x3f );
            s += ch;
        }
        if( unicode > 0x10FFFF ) {
            err = "invalid unicode codepoint";
            return false;
        }
    }
    return true;
}

auto JsonIn::get_string() -> std::string
{
    eat_whitespace();
    std::string s;
    char ch;
    std::string err;
    bool success = false;
    do {
        // the first character had better be a '"'
        stream->get( ch );
        if( !stream->good() ) {
            err = "read operation failed";
            break;
        }
        if( ch != '"' ) {
            err = "expected string but got '" + std::string( 1, ch ) + "'";
            break;
        }
        // add chars to the string, one at a time
        do {
            ch = stream->peek();
            if( !stream->good() ) {
                err = "read operation failed";
                break;
            }
            if( ch == '"' ) {
                stream->ignore();
                success = true;
                break;
            }
            if( !get_escaped_or_unicode( *stream, s, err ) ) {
                break;
            }
        } while( stream->good() );
    } while( false );
    if( success ) {
        end_value();
        return s;
    }
    if( stream->eof() ) {
        error( "couldn't find end of string, reached EOF." );
    } else if( stream->fail() ) {
        error( "stream failure while reading string." );
    } else {
        error( err, -1 );
    }
}

// These functions get -INT_MIN and -INT64_MIN while very carefully avoiding any overflow.
constexpr static auto neg_INT_MIN() -> uint64_t
{
    static_assert( sizeof( int ) <= sizeof( int64_t ),
                   "neg_INT_MIN() assumed sizeof( int ) <= sizeof( int64_t )" );
    constexpr int x = std::numeric_limits<int>::min() + std::numeric_limits<int>::max();
    static_assert( x >= 0 || x + std::numeric_limits<int>::max() >= 0,
                   "neg_INT_MIN assumed INT_MIN + INT_MAX >= -INT_MAX" );
    if( x < 0 ) {
        return static_cast<uint64_t>( std::numeric_limits<int>::max() ) + static_cast<uint64_t>( -x );
    } else {
        return static_cast<uint64_t>( std::numeric_limits<int>::max() ) - static_cast<uint64_t>( x );
    }
}
constexpr static auto neg_INT64_MIN() -> uint64_t
{
    constexpr int64_t x = std::numeric_limits<int64_t>::min() + std::numeric_limits<int64_t>::max();
    static_assert( x >= 0 || x + std::numeric_limits<int64_t>::max() >= 0,
                   "neg_INT64_MIN assumed INT64_MIN + INT64_MAX >= -INT64_MAX" );
    if( x < 0 ) {
        return static_cast<uint64_t>( std::numeric_limits<int64_t>::max() ) + static_cast<uint64_t>( -x );
    } else {
        return static_cast<uint64_t>( std::numeric_limits<int64_t>::max() ) - static_cast<uint64_t>( x );
    }
}

auto JsonIn::get_any_int() -> number_sci_notation
{
    number_sci_notation n = get_any_number();
    if( n.exp < 0 ) {
        error( "Integers cannot have a decimal point or negative order of magnitude." );
    }
    // Manually apply scientific notation, since std::pow converts to double under the hood.
    for( int64_t i = 0; i < n.exp; i++ ) {
        if( n.number > std::numeric_limits<uint64_t>::max() / 10ULL ) {
            error( "Specified order of magnitude too large -- encountered overflow applying it." );
        }
        n.number *= 10ULL;
    }
    n.exp = 0;
    return n;
}

auto JsonIn::get_int() -> int
{
    static_assert( sizeof( int ) <= sizeof( int64_t ),
                   "JsonIn::get_int() assumed sizeof( int ) <= sizeof( int64_t )" );
    number_sci_notation n = get_any_int();
    if( !n.negative && n.number > static_cast<uint64_t>( std::numeric_limits<int>::max() ) ) {
        error( "Found a number greater than " + std::to_string( std::numeric_limits<int>::max() ) +
               " which is unsupported in this context." );
    } else if( n.negative && n.number > neg_INT_MIN() ) {
        error( "Found a number less than " + std::to_string( std::numeric_limits<int>::min() ) +
               " which is unsupported in this context." );
    }
    if( n.negative ) {
        static_assert( neg_INT_MIN() <= static_cast<uint64_t>( std::numeric_limits<int>::max() )
                       || neg_INT_MIN() - static_cast<uint64_t>( std::numeric_limits<int>::max() )
                       <= static_cast<uint64_t>( std::numeric_limits<int>::max() ),
                       "JsonIn::get_int() assumed -INT_MIN - INT_MAX <= INT_MAX" );
        if( n.number > static_cast<uint64_t>( std::numeric_limits<int>::max() ) ) {
            const uint64_t x = n.number - static_cast<uint64_t>( std::numeric_limits<int>::max() );
            return -std::numeric_limits<int>::max() - static_cast<int>( x );
        } else {
            return -static_cast<int>( n.number );
        }
    } else {
        return static_cast<int>( n.number );
    }
}

auto JsonIn::get_uint() -> unsigned int
{
    number_sci_notation n = get_any_int();
    if( n.number > std::numeric_limits<unsigned int>::max() ) {
        error( "Found a number greater than " +
               std::to_string( std::numeric_limits<unsigned int>::max() ) +
               " which is unsupported in this context." );
    }
    if( n.negative ) {
        error( "Unsigned integers cannot have a negative sign." );
    }
    return static_cast<unsigned int>( n.number );
}

auto JsonIn::get_int64() -> int64_t
{
    number_sci_notation n = get_any_int();
    if( !n.negative && n.number > static_cast<uint64_t>( std::numeric_limits<int64_t>::max() ) ) {
        error( "Signed integers greater than " +
               std::to_string( std::numeric_limits<int64_t>::max() ) + " not supported." );
    } else if( n.negative && n.number > neg_INT64_MIN() ) {
        error( "Integers less than "
               + std::to_string( std::numeric_limits<int64_t>::min() ) + " not supported." );
    }
    if( n.negative ) {
        static_assert( neg_INT64_MIN() <= static_cast<uint64_t>( std::numeric_limits<int64_t>::max() )
                       || neg_INT64_MIN() - static_cast<uint64_t>( std::numeric_limits<int64_t>::max() )
                       <= static_cast<uint64_t>( std::numeric_limits<int64_t>::max() ),
                       "JsonIn::get_int64() assumed -INT64_MIN - INT64_MAX <= INT64_MAX" );
        if( n.number > static_cast<uint64_t>( std::numeric_limits<int64_t>::max() ) ) {
            const uint64_t x = n.number - static_cast<uint64_t>( std::numeric_limits<int64_t>::max() );
            return -std::numeric_limits<int64_t>::max() - static_cast<int64_t>( x );
        } else {
            return -static_cast<int64_t>( n.number );
        }
    } else {
        return static_cast<int64_t>( n.number );
    }
}

auto JsonIn::get_uint64() -> uint64_t
{
    number_sci_notation n = get_any_int();
    if( n.negative ) {
        error( "Unsigned integers cannot have a negative sign." );
    }
    return n.number;
}

auto JsonIn::get_float() -> double
{
    number_sci_notation n = get_any_number();
    return n.number * std::pow( 10.0f, n.exp ) * ( n.negative ? -1.f : 1.f );
}

auto JsonIn::get_any_number() -> number_sci_notation
{
    // this could maybe be prettier?
    char ch;
    number_sci_notation ret;
    int mod_e = 0;
    eat_whitespace();
    stream->get( ch );
    if( ( ret.negative = ch == '-' ) ) {
        stream->get( ch );
    } else if( ch != '.' && ( ch < '0' || ch > '9' ) ) {
        // not a valid float
        std::stringstream err;
        err << "expecting number but found '" << ch << "'";
        error( err.str(), -1 );
    }
    if( ch == '0' ) {
        // allow a single leading zero in front of a '.' or 'e'/'E'
        stream->get( ch );
        if( ch >= '0' && ch <= '9' ) {
            error( "leading zeros not allowed", -1 );
        }
    }
    while( ch >= '0' && ch <= '9' ) {
        ret.number *= 10;
        ret.number += ( ch - '0' );
        stream->get( ch );
    }
    if( ch == '.' ) {
        stream->get( ch );
        while( ch >= '0' && ch <= '9' ) {
            ret.number *= 10;
            ret.number += ( ch - '0' );
            mod_e -= 1;
            stream->get( ch );
        }
    }
    if( ch == 'e' || ch == 'E' ) {
        stream->get( ch );
        bool neg;
        if( ( neg = ch == '-' ) ) {
            stream->get( ch );
        } else if( ch == '+' ) {
            stream->get( ch );
        }
        while( ch >= '0' && ch <= '9' ) {
            ret.exp *= 10;
            ret.exp += ( ch - '0' );
            stream->get( ch );
        }
        if( neg ) {
            ret.exp *= -1;
        }
    }
    // unget the final non-number character (probably a separator)
    stream->unget();
    end_value();
    ret.exp += mod_e;
    return ret;
}

auto JsonIn::get_bool() -> bool
{
    char ch;
    char text[5];
    std::stringstream err;
    eat_whitespace();
    stream->get( ch );
    if( ch == 't' ) {
        stream->get( text, 4 );
        if( strcmp( text, "rue" ) == 0 ) {
            end_value();
            return true;
        } else {
            err << R"(not a boolean.  expected "true", but got ")";
            err << ch << text << "\"";
            error( err.str(), -4 );
        }
    } else if( ch == 'f' ) {
        stream->get( text, 5 );
        if( strcmp( text, "alse" ) == 0 ) {
            end_value();
            return false;
        } else {
            err << R"(not a boolean.  expected "false", but got ")";
            err << ch << text << "\"";
            error( err.str(), -5 );
        }
    }
    err << "not a boolean value!  expected 't' or 'f' but got '" << ch << "'";
    error( err.str(), -1 );
    throw JsonError( "warnings are silly" );
}

auto JsonIn::get_object() -> JsonObject
{
    return JsonObject( *this );
}
auto JsonIn::get_array() -> JsonArray
{
    return JsonArray( *this );
}

void JsonIn::start_array()
{
    eat_whitespace();
    if( peek() == '[' ) {
        stream->get();
        ate_separator = false;
        return;
    } else {
        // expecting an array, so this is an error
        std::stringstream err;
        err << "tried to start array, but found '";
        err << peek() << "', not '['";
        error( err.str() );
    }
}

auto JsonIn::end_array() -> bool
{
    eat_whitespace();
    if( peek() == ']' ) {
        if( ate_separator ) {
            uneat_whitespace();
            error( "comma not allowed at end of array" );
        }
        stream->get();
        end_value();
        return true;
    } else {
        // not the end yet, so just return false?
        return false;
    }
}

void JsonIn::start_object()
{
    eat_whitespace();
    if( peek() == '{' ) {
        stream->get();
        ate_separator = false; // not that we want to
        return;
    } else {
        // expecting an object, so fail loudly
        std::stringstream err;
        err << "tried to start object, but found '";
        err << peek() << "', not '{'";
        error( err.str() );
    }
}

auto JsonIn::end_object() -> bool
{
    eat_whitespace();
    if( peek() == '}' ) {
        if( ate_separator ) {
            uneat_whitespace();
            error( "comma not allowed at end of object" );
        }
        stream->get();
        end_value();
        return true;
    } else {
        // not the end yet, so just return false?
        return false;
    }
}

auto JsonIn::test_null() -> bool
{
    eat_whitespace();
    return peek() == 'n';
}

auto JsonIn::test_bool() -> bool
{
    eat_whitespace();
    const char ch = peek();
    return ch == 't' || ch == 'f';
}

auto JsonIn::test_number() -> bool
{
    eat_whitespace();
    const char ch = peek();
    return ch == '-' || ch == '+' || ch == '.' || ( ch >= '0' && ch <= '9' );
}

auto JsonIn::test_string() -> bool
{
    eat_whitespace();
    return peek() == '"';
}

auto JsonIn::test_bitset() -> bool
{
    eat_whitespace();
    return peek() == '"';
}

auto JsonIn::test_array() -> bool
{
    eat_whitespace();
    return peek() == '[';
}

auto JsonIn::test_object() -> bool
{
    eat_whitespace();
    return peek() == '{';
}

/* non-fatal value setting by reference */

auto JsonIn::read( bool &b, bool throw_on_error ) -> bool
{
    if( !test_bool() ) {
        return error_or_false( throw_on_error, "Expected bool" );
    }
    b = get_bool();
    return true;
}

auto JsonIn::read( char &c, bool throw_on_error ) -> bool
{
    if( !test_number() ) {
        return error_or_false( throw_on_error, "Expected number" );
    }
    c = get_int();
    return true;
}

auto JsonIn::read( signed char &c, bool throw_on_error ) -> bool
{
    if( !test_number() ) {
        return error_or_false( throw_on_error, "Expected number" );
    }
    // TODO: test for overflow
    c = get_int();
    return true;
}

auto JsonIn::read( unsigned char &c, bool throw_on_error ) -> bool
{
    if( !test_number() ) {
        return error_or_false( throw_on_error, "Expected number" );
    }
    // TODO: test for overflow
    c = get_int();
    return true;
}

auto JsonIn::read( short unsigned int &s, bool throw_on_error ) -> bool
{
    if( !test_number() ) {
        return error_or_false( throw_on_error, "Expected number" );
    }
    // TODO: test for overflow
    s = get_int();
    return true;
}

auto JsonIn::read( short int &s, bool throw_on_error ) -> bool
{
    if( !test_number() ) {
        return error_or_false( throw_on_error, "Expected number" );
    }
    // TODO: test for overflow
    s = get_int();
    return true;
}

auto JsonIn::read( int &i, bool throw_on_error ) -> bool
{
    if( !test_number() ) {
        return error_or_false( throw_on_error, "Expected number" );
    }
    i = get_int();
    return true;
}

auto JsonIn::read( std::int64_t &i, bool throw_on_error ) -> bool
{
    if( !test_number() ) {
        return error_or_false( throw_on_error, "Expected number" );
    }
    i = get_int64();
    return true;
}

auto JsonIn::read( std::uint64_t &i, bool throw_on_error ) -> bool
{
    if( !test_number() ) {
        return error_or_false( throw_on_error, "Expected number" );
    }
    i = get_uint64();
    return true;
}

auto JsonIn::read( unsigned int &u, bool throw_on_error ) -> bool
{
    if( !test_number() ) {
        return error_or_false( throw_on_error, "Expected number" );
    }
    u = get_uint();
    return true;
}

auto JsonIn::read( float &f, bool throw_on_error ) -> bool
{
    if( !test_number() ) {
        return error_or_false( throw_on_error, "Expected number" );
    }
    f = get_float();
    return true;
}

auto JsonIn::read( double &d, bool throw_on_error ) -> bool
{
    if( !test_number() ) {
        return error_or_false( throw_on_error, "Expected number" );
    }
    d = get_float();
    return true;
}

auto JsonIn::read( std::string &s, bool throw_on_error ) -> bool
{
    if( !test_string() ) {
        return error_or_false( throw_on_error, "Expected string" );
    }
    s = get_string();
    return true;
}

template<size_t N>
auto JsonIn::read( std::bitset<N> &b, bool throw_on_error ) -> bool
{
    if( !test_bitset() ) {
        return error_or_false( throw_on_error, "Expected bitset" );
    }
    std::string tmp_string = get_string();
    if( tmp_string.length() > N ) {
        // If the loaded string contains more bits than expected, skip the most significant bits
        tmp_string.erase( 0, tmp_string.length() - N );
    }
    b = std::bitset<N> ( tmp_string );
    return true;
}

auto JsonIn::read( JsonDeserializer &j, bool throw_on_error ) -> bool
{
    // can't know what type of json object it will deserialize from,
    // so just try to deserialize, catching any error.
    // TODO: non-verbose flag for JsonIn errors so try/catch is faster here
    try {
        j.deserialize( *this );
        return true;
    } catch( const JsonError & ) {
        if( throw_on_error ) {
            throw;
        }
        return false;
    }
}

/* error display */

// WARNING: for occasional use only.
auto JsonIn::line_number( int offset_modifier ) -> std::string
{
    const std::string &name = path ? *path : "<unknown source file>";
    if( stream && stream->eof() ) {
        return name + ":EOF";
    } else if( !stream || stream->fail() ) {
        return name + ":???";
    } // else stream is fine
    int pos = tell();
    int line = 1;
    int offset = 1;
    char ch;
    seek( 0 );
    for( int i = 0; i < pos + offset_modifier; ++i ) {
        stream->get( ch );
        if( !stream->good() ) {
            break;
        }
        if( ch == '\r' ) {
            offset = 1;
            ++line;
            if( peek() == '\n' ) {
                stream->get();
                ++i;
            }
        } else if( ch == '\n' ) {
            offset = 1;
            ++line;
        } else {
            ++offset;
        }
    }
    seek( pos );
    std::stringstream ret;
    ret << name << ":" << line << ":" << offset;
    return ret.str();
}

void JsonIn::error( const std::string &message, int offset )
{
    std::ostringstream err;
    err << "Json error: " << line_number( offset ) << ": " << message;
    // if we can't get more info from the stream don't try
    if( !stream->good() ) {
        throw JsonError( err.str() );
    }
    // also print surrounding few lines of context, if not too large
    err << "\n\n";
    stream->seekg( offset, std::istream::cur );
    size_t pos = tell();
    rewind( 3, 240 );
    size_t startpos = tell();
    std::string buffer( pos - startpos, '\0' );
    stream->read( &buffer[0], pos - startpos );
    auto it = buffer.begin();
    for( ; it < buffer.end() && ( *it == '\r' || *it == '\n' ); ++it ) {
        // skip starting newlines
    }
    for( ; it < buffer.end(); ++it ) {
        if( *it == '\r' ) {
            err << '\n';
            if( it + 1 < buffer.end() && *( it + 1 ) == '\n' ) {
                ++it;
            }
        } else {
            err << *it;
        }
    }
    if( !is_whitespace( peek() ) ) {
        err << peek();
    }
    // display a pointer to the position
    rewind( 1, 240 );
    startpos = tell();
    err << '\n';
    if( pos > startpos ) {
        err << std::string( pos - startpos, ' ' );
    }
    err << "^\n";
    seek( pos );
    // if that wasn't the end of the line, continue underneath pointer
    char ch = stream->get();
    if( ch == '\r' ) {
        if( peek() == '\n' ) {
            stream->get();
        }
    } else if( ch == '\n' ) {
        // pass
    } else if( peek() != '\r' && peek() != '\n' && !stream->eof() ) {
        for( size_t i = 0; i < pos - startpos + 1; ++i ) {
            err << ' ';
        }
    }
    // print the next couple lines as well
    int line_count = 0;
    for( int i = 0; line_count < 3 && stream->good() && i < 240; ++i ) {
        stream->get( ch );
        if( !stream->good() ) {
            break;
        }
        if( ch == '\r' ) {
            ch = '\n';
            ++line_count;
            if( stream->peek() == '\n' ) {
                stream->get( ch );
            }
        } else if( ch == '\n' ) {
            ++line_count;
        }
        err << ch;
    }
    std::string msg = err.str();
    if( !msg.empty() && msg.back() != '\n' ) {
        msg.push_back( '\n' );
    }
    throw JsonError( msg );
}

void JsonIn::string_error( const std::string &message, const int offset )
{
    if( test_string() ) {
        // skip quote mark
        stream->ignore();
        std::string s;
        std::string err;
        for( int i = 0; i < offset; ++i ) {
            if( !get_escaped_or_unicode( *stream, s, err ) ) {
                break;
            }
        }
    }
    error( message, -1 );
}

auto JsonIn::error_or_false( bool throw_, const std::string &message, int offset ) -> bool
{
    if( throw_ ) {
        error( message, offset );
    }
    return false;
}

void JsonIn::rewind( int max_lines, int max_chars )
{
    if( max_lines < 0 && max_chars < 0 ) {
        // just rewind to the beginning i guess
        seek( 0 );
        return;
    }
    if( tell() == 0 ) {
        return;
    }
    int lines_found = 0;
    stream->seekg( -1, std::istream::cur );
    for( int i = 0; i < max_chars; ++i ) {
        size_t tellpos = tell();
        if( peek() == '\n' ) {
            ++lines_found;
            if( tellpos > 0 ) {
                stream->seekg( -1, std::istream::cur );
                if( peek() != '\r' ) {
                    stream->seekg( 1, std::istream::cur );
                } else {
                    --tellpos;
                }
            }
        } else if( peek() == '\r' ) {
            ++lines_found;
        }
        if( lines_found == max_lines ) {
            // don't include the last \n or \r
            if( peek() == '\n' ) {
                stream->seekg( 1, std::istream::cur );
            } else if( peek() == '\r' ) {
                stream->seekg( 1, std::istream::cur );
                if( peek() == '\n' ) {
                    stream->seekg( 1, std::istream::cur );
                }
            }
            break;
        } else if( tellpos == 0 ) {
            break;
        }
        stream->seekg( -1, std::istream::cur );
    }
}

auto JsonIn::substr( size_t pos, size_t len ) -> std::string
{
    std::string ret;
    if( len == std::string::npos ) {
        stream->seekg( 0, std::istream::end );
        size_t end = tell();
        len = end - pos;
    }
    ret.resize( len );
    stream->seekg( pos );
    stream->read( &ret[0], len );
    return ret;
}

JsonOut::JsonOut( std::ostream &s, bool pretty, int depth ) :
    stream( &s ), pretty_print( pretty ), indent_level( depth )
{
    // ensure consistent and locale-independent formatting of numerals
    stream->imbue( std::locale::classic() );
    stream->setf( std::ios_base::showpoint );
    stream->setf( std::ios_base::dec, std::ostream::basefield );
    stream->setf( std::ios_base::fixed, std::ostream::floatfield );

    // automatically stringify bool to "true" or "false"
    stream->setf( std::ios_base::boolalpha );
}

auto JsonOut::tell() -> int
{
    return stream->tellp();
}

void JsonOut::seek( int pos )
{
    stream->clear();
    stream->seekp( pos );
    need_separator = false;
}

void JsonOut::write_indent()
{
    std::fill_n( std::ostream_iterator<char>( *stream ), indent_level * 2, ' ' );
}

void JsonOut::write_separator()
{
    if( !need_separator ) {
        return;
    }
    stream->put( ',' );
    if( pretty_print ) {
        // Wrap after seperator between objects and between members of top-level objects.
        if( indent_level < 2 || need_wrap.back() ) {
            stream->put( '\n' );
            write_indent();
        } else {
            // Otherwise pad after commas.
            stream->put( ' ' );
        }
    }
    need_separator = false;
}

void JsonOut::write_member_separator()
{
    if( pretty_print ) {
        stream->write( ": ", 2 );
    } else {
        stream->put( ':' );
    }
    need_separator = false;
}

void JsonOut::start_pretty()
{
    if( pretty_print ) {
        indent_level += 1;
        // Wrap after top level object and array opening.
        if( indent_level < 2 || need_wrap.back() ) {
            stream->put( '\n' );
            write_indent();
        } else {
            // Otherwise pad after opening.
            stream->put( ' ' );
        }
    }
}

void JsonOut::end_pretty()
{
    if( pretty_print ) {
        indent_level -= 1;
        // Wrap after ending top level array and object.
        // Also wrap in the special case of exiting an array containing an object.
        if( indent_level < 1 || need_wrap.back() ) {
            stream->put( '\n' );
            write_indent();
        } else {
            // Otherwise pad after ending.
            stream->put( ' ' );
        }
    }
}

void JsonOut::start_object( bool wrap )
{
    if( need_separator ) {
        write_separator();
    }
    stream->put( '{' );
    need_wrap.push_back( wrap );
    start_pretty();
    need_separator = false;
}

void JsonOut::end_object()
{
    end_pretty();
    need_wrap.pop_back();
    stream->put( '}' );
    need_separator = true;
}

void JsonOut::start_array( bool wrap )
{
    if( need_separator ) {
        write_separator();
    }
    stream->put( '[' );
    need_wrap.push_back( wrap );
    start_pretty();
    need_separator = false;
}

void JsonOut::end_array()
{
    end_pretty();
    need_wrap.pop_back();
    stream->put( ']' );
    need_separator = true;
}

void JsonOut::write_null()
{
    if( need_separator ) {
        write_separator();
    }
    stream->write( "null", 4 );
    need_separator = true;
}

void JsonOut::write( const std::string &val )
{
    if( need_separator ) {
        write_separator();
    }
    stream->put( '"' );
    for( const auto &i : val ) {
        unsigned char ch = i;
        if( ch == '"' ) {
            stream->write( "\\\"", 2 );
        } else if( ch == '\\' ) {
            stream->write( "\\\\", 2 );
        } else if( ch == '/' ) {
            // don't technically need to escape this
            stream->put( '/' );
        } else if( ch == '\b' ) {
            stream->write( "\\b", 2 );
        } else if( ch == '\f' ) {
            stream->write( "\\f", 2 );
        } else if( ch == '\n' ) {
            stream->write( "\\n", 2 );
        } else if( ch == '\r' ) {
            stream->write( "\\r", 2 );
        } else if( ch == '\t' ) {
            stream->write( "\\t", 2 );
        } else if( ch < 0x20 ) {
            // convert to "\uxxxx" unicode escape
            stream->write( "\\u00", 4 );
            stream->put( ( ch < 0x10 ) ? '0' : '1' );
            char remainder = ch & 0x0F;
            if( remainder < 0x0A ) {
                stream->put( '0' + remainder );
            } else {
                stream->put( 'A' + ( remainder - 0x0A ) );
            }
        } else {
            stream->put( ch );
        }
    }
    stream->put( '"' );
    need_separator = true;
}

template<size_t N>
void JsonOut::write( const std::bitset<N> &b )
{
    if( need_separator ) {
        write_separator();
    }
    std::string converted = b.to_string();
    stream->put( '"' );
    for( auto &i : converted ) {
        unsigned char ch = i;
        stream->put( ch );
    }
    stream->put( '"' );
    need_separator = true;
}

void JsonOut::write( const JsonSerializer &thing )
{
    if( need_separator ) {
        write_separator();
    }
    thing.serialize( *this );
    need_separator = true;
}

void JsonOut::member( const std::string &name )
{
    write( name );
    write_member_separator();
}

void JsonOut::null_member( const std::string &name )
{
    member( name );
    write_null();
}

JsonError::JsonError( const std::string &msg )
    : std::runtime_error( msg )
{
}

auto operator<<( std::ostream &stream, const JsonError &err ) -> std::ostream &
{
    return stream << err.what();
}

// Need to instantiate those template to make them available for other compilation units.
// Currently only bitsets of size 12 are loaded / stored, if you need other sizes, either explicitly
// instantiate them here, or move the templated read/write functions into the header.
template void JsonOut::write<12>( const std::bitset<12> & );
template bool JsonIn::read<12>( std::bitset<12> &, bool throw_on_error );

auto JsonValue::seek() const -> JsonIn &
{
    jsin_.seek( pos_ );
    return jsin_;
}

auto JsonObject::get_member( const std::string &name ) const -> JsonValue
{
    const auto iter = positions.find( name );
    if( !jsin || iter == positions.end() ) {
        throw_error( "missing required field \"" + name + "\" in object: " + str() );
    }
    mark_visited( name );
    return JsonValue( *jsin, iter->second );
}
