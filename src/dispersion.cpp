#include "dispersion.h"

#include <math.h>
#include "rng.h"

double dispersion_sources::roll() const {
    return std::abs( rng_normal( stddev() ) );
}

double dispersion_sources::stddev() const
{
    return sdev * multiplier;
}

double dispersion_sources::cdf( double x ) const {
    constexpr double sqrt2 = std::sqrt( 2.0 );
    return std::erf( x / ( stddev() * sqrt2 ) );
}

