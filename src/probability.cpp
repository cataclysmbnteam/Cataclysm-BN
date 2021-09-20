#include "probability.h"

#include <cmath>
#include <limits>
#include "cata_utility.h"

namespace normal
{

inline double cdf( double x, double mean, double stddev )
{
    return clamp( 0.5 * ( 1.0 + std::erf( ( x - mean ) / ( stddev * M_SQRT2 ) ) ), 0.0, 1.0 );
}

double truncated_cdf( double x, double mean, double stddev, double min, double max )
{
    if( x <= min ) {
        return 0.0;
    } else if( x >= max ) {
        return 1.0;
    }

    double eps = ( x - mean ) / stddev;
    double alpha = ( min - mean ) / stddev;
    double beta = ( max - mean ) / stddev;
    double z = cdf( beta, 0, 1 ) - cdf( alpha, 0, 1 );
    return ( cdf( eps, 0, 1 ) - cdf( alpha, 0, 1 ) ) / z;
}

constexpr size_t n = 4096;
// Normalized to [0, 1).
// CDF can be linearly transformed, just keep (x - mean) / stddev constant.
extern const std::array<double, n> truncated_cdf_lookup_table;
const std::array<double, n> truncated_cdf_lookup_table = ( []()
{
    std::array<double, n> lookup;
    for( size_t i = 0; i < n; i++ ) {
        double x = static_cast<double>( i ) / n;
        lookup[i] = truncated_cdf( x, 0.5, 0.25, 0, 1 );
    }
    return lookup;
} )();

}

namespace truncated_probit
{

const std::array<double, n> lookup_table = ( []()
{
    std::array<double, n> lookup = {};
    double last_x = 0.0;
    for( size_t i = 1; i < n; i++ ) {
        double x = last_x;
        double step = 0.5;
        size_t j = n + 1;
        while( step > 0.0 && i != j ) {
            double y = normal::truncated_cdf( x, 0.5, 0.25, 0.0, 1.0 );
            j = y * n;

            x = j < i ? ( x + step ) : ( x - step );
            step *= 0.5;
        }

        lookup[i] = x;
        last_x = x;
    }
    return lookup;
} )();

/**
 * Rescales truncated probit to include (approximately) points (0, 0), (1, 1)
 */
double at( double x )
{
    if( x <= 0.0 ) {
        return 0.0;
    } else if( x >= 1 ) {
        return 1.0;
    }

    size_t n = truncated_probit::n;
    size_t i = static_cast<size_t>( x * n );
    size_t clamped_i = std::min( i, n - 1 );
    return lookup_table[clamped_i];
}

} // namespace probit
