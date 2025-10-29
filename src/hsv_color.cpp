#include "hsv_color.h"

auto median( const uint8_t a, const uint8_t b, const uint8_t c )
{
    if( ( a > b ) ^ ( a > c ) ) {
        return a;
    }
    if( ( b < a ) ^ ( b < c ) ) {
        return b;
    }
    return c;
};

auto hsv2rgb( HSVColor color ) -> RGBColor
{
    constexpr auto E = ( 1 << 16 ) - 1;

    auto [H, S, V, A] = color;

    if( S == 0 || V == 0 ) {
        return RGBColor{V, V, V, A};
    }

    uint8_t I;
    if( H < E ) {
        I = 0;
    } else if( H < 2 * E ) {
        I = 1;
    } else if( H < 3 * E ) {
        I = 2;
    } else if( H < 4 * E ) {
        I = 3;
    } else if( H < 5 * E ) {
        I = 4;
    } else {
        I = 5;
    }

    auto F = ( H - ( E * I ) );
    if( F == 0 ) {
        ++F;
    }

    if( I % 2 != 0 ) {
        F = ( E - F );
    }

    const auto d = ( ( S * V ) >> 16 ) + 1;
    const auto m = static_cast<uint8_t>( V - d );
    const auto c = static_cast<uint8_t>( ( ( F * d ) >> 16 ) + m );

    switch( I ) {
        case 0:
            return {V, c, m, A};
        case 1:
            return {c, V, m, A};
        case 2:
            return {m, V, c, A};
        case 3:
            return {m, c, V, A};
        case 4:
            return {c, m, V, A};
        case 5:
            return {V, m, c, A};
        default:
            return {0, 0, 0, A};
    }
}

auto rgb2hsv( RGBColor color ) -> HSVColor
{


    const auto [R, G, B, A] = color;
    const auto min = std::min( R, std::min( G, B ) );
    const auto max = std::max( R, std::max( G, B ) );
    const auto med = median( R, G, B );

    const auto V = max;

    const auto d = max - min;
    if( d == 0 || max == 0 ) {
        return HSVColor{0, 0, V, A};
    }

    const auto S = static_cast<uint16_t>( ( ( d << 16 ) - 1 ) / V );

    int I;
    if( max == R && min == B ) {
        I = 0;
    } else if( max == G && min == B ) {
        I = 1;
    } else if( max == G && min == R ) {
        I = 2;
    } else if( max == B && min == R ) {
        I = 3;
    } else if( max == B && min == G ) {
        I = 4;
    } else {
        I = 5;
    }

    constexpr auto E = ( 1 << 16 ) - 1;
    auto F = ( ( ( med - min ) << 16 ) / d ) + 1;
    if( I % 2 != 0 ) {
        F = E - F;
    }

    const auto H = static_cast<uint32_t>( ( E * I ) + F );

    return HSVColor{H, S, V, A};
}