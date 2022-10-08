#include "catacharset.h"

#include <cstdlib>
#include <cstring>
#include <array>

#include "options.h"
#include "output.h"
#include "wcwidth.h"

#if defined(_WIN32)
#include "platform_win.h"
#include "mmsystem.h"
#endif

//copied from SDL2_ttf code
//except type changed from unsigned to uint32_t
auto UTF8_getch( const char **src, int *srclen ) -> uint32_t
{
    const unsigned char *p = *reinterpret_cast<const unsigned char **>( src );
    int left = 0;
    bool overlong = false;
    bool underflow = false;
    uint32_t ch = UNKNOWN_UNICODE;

    if( *srclen == 0 ) {
        return UNKNOWN_UNICODE;
    }
    if( p[0] >= 0xFC ) {
        if( ( p[0] & 0xFE ) == 0xFC ) {
            if( p[0] == 0xFC && ( p[1] & 0xFC ) == 0x80 ) {
                overlong = true;
            }
            ch = static_cast<uint32_t>( p[0] & 0x01 );
            left = 5;
        }
    } else if( p[0] >= 0xF8 ) {
        if( ( p[0] & 0xFC ) == 0xF8 ) {
            if( p[0] == 0xF8 && ( p[1] & 0xF8 ) == 0x80 ) {
                overlong = true;
            }
            ch = static_cast<uint32_t>( p[0] & 0x03 );
            left = 4;
        }
    } else if( p[0] >= 0xF0 ) {
        if( ( p[0] & 0xF8 ) == 0xF0 ) {
            if( p[0] == 0xF0 && ( p[1] & 0xF0 ) == 0x80 ) {
                overlong = true;
            }
            ch = static_cast<uint32_t>( p[0] & 0x07 );
            left = 3;
        }
    } else if( p[0] >= 0xE0 ) {
        if( ( p[0] & 0xF0 ) == 0xE0 ) {
            if( p[0] == 0xE0 && ( p[1] & 0xE0 ) == 0x80 ) {
                overlong = true;
            }
            ch = static_cast<uint32_t>( p[0] & 0x0F );
            left = 2;
        }
    } else if( p[0] >= 0xC0 ) {
        if( ( p[0] & 0xE0 ) == 0xC0 ) {
            if( ( p[0] & 0xDE ) == 0xC0 ) {
                overlong = true;
            }
            ch = static_cast<uint32_t>( p[0] & 0x1F );
            left = 1;
        }
    } else {
        if( ( p[0] & 0x80 ) == 0x00 ) {
            ch = static_cast<uint32_t>( p[0] );
        }
    }
    ++*src;
    --*srclen;
    while( left > 0 && *srclen > 0 ) {
        ++p;
        if( ( p[0] & 0xC0 ) != 0x80 ) {
            ch = UNKNOWN_UNICODE;
            break;
        }
        ch <<= 6;
        ch |= ( p[0] & 0x3F );
        ++*src;
        --*srclen;
        --left;
    }
    if( left > 0 ) {
        underflow = true;
    }
    if( overlong || underflow ||
        ( ch >= 0xD800 && ch <= 0xDFFF ) ||
        ( ch == 0xFFFE || ch == 0xFFFF ) || ch > 0x10FFFF ) {
        ch = UNKNOWN_UNICODE;
    }
    return ch;
}

auto utf32_to_utf8( uint32_t ch ) -> std::string
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
        utf8Bytes = 3;
        ch = UNKNOWN_UNICODE;
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

//Calculate width of a Unicode string
//Latin characters have a width of 1
//CJK characters have a width of 2, etc
auto utf8_width( const char *s, const bool ignore_tags ) -> int
{
    if( ignore_tags ) {
        return utf8_width( remove_color_tags( s ) );
    }
    int len = strlen( s );
    const char *ptr = s;
    int w = 0;
    while( len > 0 ) {
        uint32_t ch = UTF8_getch( &ptr, &len );
        if( ch == UNKNOWN_UNICODE ) {
            continue;
        }
        w += mk_wcwidth( ch );
    }
    return w;
}

auto utf8_width( const std::string &str, const bool ignore_tags ) -> int
{
    return utf8_width( str.c_str(), ignore_tags );
}

auto utf8_width( const utf8_wrapper &str, const bool ignore_tags ) -> int
{
    return utf8_width( str.c_str(), ignore_tags );
}

auto left_justify( const std::string &str, const int width, const bool ignore_tags ) -> std::string
{
    int str_width = utf8_width( str, ignore_tags );
    if( str_width >= width ) {
        return str;
    } else {
        return str + std::string( width - str_width, ' ' );
    }
}

auto right_justify( const std::string &str, const int width, const bool ignore_tags ) -> std::string
{
    int str_width = utf8_width( str, ignore_tags );
    if( str_width >= width ) {
        return str;
    } else {
        return std::string( width - str_width, ' ' ) + str;
    }
}

auto utf8_justify( const std::string &str, const int width, const bool ignore_tags ) -> std::string
{
    if( width < 0 ) {
        return left_justify( str, -width, ignore_tags );
    } else {
        return right_justify( str, width, ignore_tags );
    }
}

//Convert cursor position to byte offset
//returns the first character position in bytes behind the cursor position.
//If the cursor is not on the first half of the character,
//prevpos (which points to the first byte of the cursor located char)
// should be a different value.
auto cursorx_to_position( const char *line, int cursorx, int *prevpos, int maxlen ) -> int
{
    int dummy;
    int i = 0;
    int c = 0;
    int *p = prevpos ? prevpos : &dummy;
    *p = 0;
    while( c < cursorx ) {
        const char *utf8str = line + i;
        int len = ANY_LENGTH;
        if( utf8str[0] == 0 ) {
            break;
        }
        uint32_t ch = UTF8_getch( &utf8str, &len );
        int cw = mk_wcwidth( ch );
        len = ANY_LENGTH - len;
        if( len <= 0 ) {
            len = 1;
        }
        if( maxlen >= 0 && maxlen < ( i + len ) ) {
            break;
        }
        i += len;
        if( cw <= 0 ) {
            cw = 0;
        }
        c += cw;
        if( c <= cursorx ) {
            *p = i;
        }
    }
    return i;
}

auto utf8_truncate( const std::string &s, size_t length ) -> std::string
{

    if( length == 0 || s.empty() ) {
        return s;
    }

    int last_pos = cursorx_to_position( s.c_str(), length, nullptr, -1 );

    return s.substr( 0, last_pos );
}

static const std::array<char, 64> base64_encoding_table = {{
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', '+', '-'
    }
};

static       std::array<char, 256> base64_decoding_table;
static const std::array<int, 3> mod_table = {{0, 2, 1}};

static void build_base64_decoding_table()
{
    static bool built = false;
    if( !built ) {
        for( int i = 0; i < 64; i++ ) {
            base64_decoding_table[static_cast<unsigned char>( base64_encoding_table[i] )] = i;
        }
        built = true;
    }
}

auto base64_encode( const std::string &str ) -> std::string
{
    //assume it is already encoded
    if( !str.empty() && str[0] == '#' ) {
        return str;
    }

    int input_length = str.length();
    int output_length = 4 * ( ( input_length + 2 ) / 3 );

    std::string encoded_data( output_length, '\0' );
    const unsigned char *data = reinterpret_cast<const unsigned char *>( str.c_str() );

    for( int i = 0, j = 0; i < input_length; ) {

        unsigned octet_a = i < input_length ? data[i++] : 0;
        unsigned octet_b = i < input_length ? data[i++] : 0;
        unsigned octet_c = i < input_length ? data[i++] : 0;

        unsigned triple = ( octet_a << 0x10 ) + ( octet_b << 0x08 ) + octet_c;

        encoded_data[j++] = base64_encoding_table[( triple >> 3 * 6 ) & 0x3F];
        encoded_data[j++] = base64_encoding_table[( triple >> 2 * 6 ) & 0x3F];
        encoded_data[j++] = base64_encoding_table[( triple >> 1 * 6 ) & 0x3F];
        encoded_data[j++] = base64_encoding_table[( triple >> 0 * 6 ) & 0x3F];
    }

    for( int i = 0; i < mod_table[input_length % 3]; i++ ) {
        encoded_data[output_length - 1 - i] = '=';
    }

    return "#" + encoded_data;
}

auto base64_decode( const std::string &str ) -> std::string
{
    // do not decode if it is not base64
    if( str.empty() || str[0] != '#' ) {
        return str;
    }

    build_base64_decoding_table();

    std::string instr = str.substr( 1 );

    int input_length = instr.length();

    if( input_length % 4 != 0 ) {
        return str;
    }

    int output_length = input_length / 4 * 3;
    const char *data = instr.c_str();

    if( data[input_length - 1] == '=' ) {
        output_length--;
    }
    if( data[input_length - 2] == '=' ) {
        output_length--;
    }

    std::string decoded_data( output_length, '\0' );

    for( int i = 0, j = 0; i < input_length; ) {

        unsigned sextet_a = data[i] == '=' ? 0 & i++ : base64_decoding_table[static_cast<unsigned char>
                            ( data[i++] )];
        unsigned sextet_b = data[i] == '=' ? 0 & i++ : base64_decoding_table[static_cast<unsigned char>
                            ( data[i++] )];
        unsigned sextet_c = data[i] == '=' ? 0 & i++ : base64_decoding_table[static_cast<unsigned char>
                            ( data[i++] )];
        unsigned sextet_d = data[i] == '=' ? 0 & i++ : base64_decoding_table[static_cast<unsigned char>
                            ( data[i++] )];

        unsigned triple = ( sextet_a << 3 * 6 )
                          + ( sextet_b << 2 * 6 )
                          + ( sextet_c << 1 * 6 )
                          + ( sextet_d << 0 * 6 );

        if( j < output_length ) {
            decoded_data[j++] = ( triple >> 2 * 8 ) & 0xFF;
        }
        if( j < output_length ) {
            decoded_data[j++] = ( triple >> 1 * 8 ) & 0xFF;
        }
        if( j < output_length ) {
            decoded_data[j++] = ( triple >> 0 * 8 ) & 0xFF;
        }
    }

    return decoded_data;
}

static void strip_trailing_nulls( std::wstring &str )
{
    while( !str.empty() && str.back() == '\0' ) {
        str.pop_back();
    }
}

static void strip_trailing_nulls( std::string &str )
{
    while( !str.empty() && str.back() == '\0' ) {
        str.pop_back();
    }
}

auto utf8_to_wstr( const std::string &str ) -> std::wstring
{
#if defined(_WIN32)
    int sz = MultiByteToWideChar( CP_UTF8, 0, str.c_str(), -1, nullptr, 0 ) + 1;
    std::wstring wstr( sz, '\0' );
    MultiByteToWideChar( CP_UTF8, 0, str.c_str(), -1, &wstr[0], sz );
    strip_trailing_nulls( wstr );
    return wstr;
#else
    std::size_t sz = std::mbstowcs( nullptr, str.c_str(), 0 ) + 1;
    std::wstring wstr( sz, '\0' );
    std::mbstowcs( &wstr[0], str.c_str(), sz );
    strip_trailing_nulls( wstr );
    return wstr;
#endif
}

auto wstr_to_utf8( const std::wstring &wstr ) -> std::string
{
#if defined(_WIN32)
    int sz = WideCharToMultiByte( CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr );
    std::string str( sz, '\0' );
    WideCharToMultiByte( CP_UTF8, 0, wstr.c_str(), -1, &str[0], sz, nullptr, nullptr );
    strip_trailing_nulls( str );
    return str;
#else
    std::size_t sz = std::wcstombs( nullptr, wstr.c_str(), 0 ) + 1;
    std::string str( sz, '\0' );
    std::wcstombs( &str[0], wstr.c_str(), sz );
    strip_trailing_nulls( str );
    return str;
#endif
}

auto utf32_to_utf8( const std::u32string &str ) -> std::string
{
    std::string ret;
    ret.reserve( str.length() );
    for( auto it = str.begin(); it < str.end(); ++it ) {
        ret += utf32_to_utf8( *it );
    }
    return ret;
}

auto utf8_to_utf32( const std::string &str ) -> std::u32string
{
    int len = str.length();
    const char *dat = str.data();
    std::u32string ret;
    ret.reserve( len );
    while( len > 0 ) {
        ret.push_back( UTF8_getch( &dat, &len ) );
    }
    ret.shrink_to_fit();
    return ret;
}

auto utf8_display_split( const std::string &s ) -> std::vector<std::string>
{
    std::vector<std::string> result;
    std::string current_glyph;
    const char *pos = s.c_str();
    int len = s.length();
    while( len > 0 ) {
        const char *old_pos = pos;
        const uint32_t ch = UTF8_getch( &pos, &len );
        const int width = mk_wcwidth( ch );
        if( width > 0 && !current_glyph.empty() ) {
            result.push_back( current_glyph );
            current_glyph.clear();
        }
        current_glyph += std::string( old_pos, pos );
    }
    result.push_back( current_glyph );
    return result;
}

auto center_text_pos( const char *text, int start_pos, int end_pos ) -> int
{
    int full_screen = end_pos - start_pos + 1;
    int str_len = utf8_width( text );
    int position = ( full_screen - str_len ) / 2;

    if( position <= 0 ) {
        return start_pos;
    }

    return start_pos + position;
}

auto center_text_pos( const std::string &text, int start_pos, int end_pos ) -> int
{
    return center_text_pos( text.c_str(), start_pos, end_pos );
}

auto center_text_pos( const utf8_wrapper &text, int start_pos, int end_pos ) -> int
{
    int full_screen = end_pos - start_pos + 1;
    int str_len = text.display_width();
    int position = ( full_screen - str_len ) / 2;

    if( position <= 0 ) {
        return start_pos;
    }

    return start_pos + position;
}

// In an attempt to maintain compatibility with gcc 4.6, use an initializer function
// instead of a delegated constructor.
// When we declare a hard dependency on gcc 4.7+, turn this back into a delegated constructor.
void utf8_wrapper::init_utf8_wrapper()
{
    const char *utf8str = _data.c_str();
    int len = _data.length();
    while( len > 0 ) {
        const uint32_t ch = UTF8_getch( &utf8str, &len );
        if( ch == UNKNOWN_UNICODE ) {
            continue;
        }
        _length++;
        _display_width += mk_wcwidth( ch );
    }
}

utf8_wrapper::utf8_wrapper( const std::string &d ) : _data( d ), _length( 0 ), _display_width( 0 )
{
    init_utf8_wrapper();
}

utf8_wrapper::utf8_wrapper( const char *d ) : _length( 0 ), _display_width( 0 )
{
    _data = std::string( d );
    init_utf8_wrapper();
}

auto utf8_wrapper::byte_start( size_t bstart, size_t start ) const -> size_t
{
    if( bstart >= _data.length() ) {
        return _data.length();
    }
    const char *utf8str = _data.c_str() + bstart;
    int len = _data.length() - bstart;
    while( len > 0 && start > 0 ) {
        const uint32_t ch = UTF8_getch( &utf8str, &len );
        if( ch == UNKNOWN_UNICODE ) {
            continue;
        }
        start--;
    }
    return utf8str - _data.c_str();
}

auto utf8_wrapper::byte_start_display( size_t bstart, size_t start ) const -> size_t
{
    if( bstart >= _data.length() ) {
        return _data.length();
    }
    if( start == 0 ) {
        return 0;
    }
    const char *utf8str = _data.c_str() + bstart;
    int len = _data.length() - bstart;
    while( len > 0 ) {
        const char *prevstart = utf8str;
        const uint32_t ch = UTF8_getch( &utf8str, &len );
        if( ch == UNKNOWN_UNICODE ) {
            continue;
        }
        const int width = mk_wcwidth( ch );
        if( static_cast<int>( start ) >= width ) {
            // If width is 0, include the code point (might be combination character)
            // Same when width is actually smaller than start
            start -= width;
        } else {
            // If width is 2 and start is 1, stop before the multi-cell code point
            // Same when width is 1 and start is 0.
            return prevstart - _data.c_str();
        }
    }
    return _data.length();
}

void utf8_wrapper::insert( size_t start, const utf8_wrapper &other )
{
    const size_t bstart = byte_start( 0, start );
    _data.insert( bstart, other._data );
    _length += other._length;
    _display_width += other._display_width;
}

auto utf8_wrapper::substr( size_t start, size_t length ) const -> utf8_wrapper
{
    return substr_byte( byte_start( 0, start ), length, false );
}

auto utf8_wrapper::substr_display( size_t start, size_t length ) const -> utf8_wrapper
{
    return substr_byte( byte_start_display( 0, start ), length, true );
}

auto utf8_wrapper::substr_byte( size_t bytestart, size_t length,
                                        bool use_display_width ) const -> utf8_wrapper
{
    if( length == 0 || bytestart >= _data.length() ) {
        return utf8_wrapper();
    }
    size_t bend;
    if( use_display_width ) {
        bend = byte_start_display( bytestart, length );
    } else {
        bend = byte_start( bytestart, length );
    }
    return utf8_wrapper( _data.substr( bytestart, bend - bytestart ) );
}

auto utf8_wrapper::at( size_t start ) const -> uint32_t
{
    const size_t bstart = byte_start( 0, start );
    const char *utf8str = _data.c_str() + bstart;
    int len = _data.length() - bstart;
    while( len > 0 ) {
        const uint32_t ch = UTF8_getch( &utf8str, &len );
        if( ch != UNKNOWN_UNICODE ) {
            return ch;
        }
    }
    return 0;
}

void utf8_wrapper::erase( size_t start, size_t length )
{
    const size_t bstart = byte_start( 0, start );
    const utf8_wrapper removed( substr_byte( bstart, length, false ) );
    _data.erase( bstart, removed._data.length() );
    _length -= removed._length;
    _display_width -= removed._display_width;
}

void utf8_wrapper::append( const utf8_wrapper &other )
{
    _data.append( other._data );
    _length += other._length;
    _display_width += other._display_width;
}

auto utf8_wrapper::replace_all( const utf8_wrapper &search, const utf8_wrapper &replace ) -> utf8_wrapper &
{
    for( std::string::size_type i = _data.find( search._data ); i != std::string::npos;
         i = _data.find( search._data, i ) ) {
        erase( i, search.length() );
        insert( i, replace );
        i += replace._data.length();
    }

    return *this;
}

auto utf8_wrapper::shorten( size_t maxlength ) const -> std::string
{
    if( display_width() <= maxlength ) {
        return str();
    }
    // 2026 is the utf8 for …
    return substr_display( 0, maxlength - 1 ).str() + "\u2026";
}
