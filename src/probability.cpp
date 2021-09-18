#include "probability.h"

#include <cmath>
#include "cata_utility.h"

// https://stackoverflow.com/a/40260471
double erfinv_approx( double x )
{
    double sign = x < 0 ? -1.0 : 1.0;

    // It seems more stable than 1 - x * x
    double one_minus_x_squared = ( 1.0 + x ) * ( 1.0 - x );
    double ln = std::log( one_minus_x_squared );

    constexpr double a = 0.15449436008930206298828125;
    // constexpr double a = 0.147;
    double tt1 = 2.0 / ( M_PI * a ) + 0.5f * ln;
    double tt2 = 1.0 / a * ln;

    return sign * std::sqrt( -tt1 + std::sqrt( tt1 * tt1 - tt2 ) );
}

namespace probit
{

double approx( double x )
{
    return M_SQRT2 * erfinv_approx( 2 * x - 1 );
}

/**
 * Rescales probit to include (approximately) points (0, 0), (N - 1, 1)
 */
template<size_t N>
double probit_integer( size_t i )
{
    constexpr double mul = 1.0 / N;
    double x = i * mul;
    return ( 1 + probit::approx( x ) / ( M_SQRT2 * 2.0 ) ) / 2.0;
}

// The error is ~0.002, which is just a bit above 1.0 / 512.0
constexpr size_t n = 512;
extern const std::array<double, n> lookup_table;
const std::array<double, n> lookup_table = ( []()
{
    std::array<double, n> lookup;
    for( size_t i = 0; i < n; i++ ) {
        lookup[i] = clamp( probit_integer<n>( i ), 0.0, 1.0 );
    }
    return lookup;
} )();

/**
 * Rescales probit to include (approximately) points (0, 0), (1, 1)
 */
double rescaled_to_zero_to_one( double x )
{
    double clamped_x = clamp( x, 0.0, 1.0 );
    return probit::lookup_table[static_cast<size_t>( clamped_x * ( probit::n - 1 ) )];
}

} // namespace probit
