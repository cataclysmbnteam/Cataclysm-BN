#include "dispersion.h"

#include <cmath>
#include "rng.h"

double dispersion_sources::roll() const {
    return std::abs( normal_roll( 0.0, stddev() ) );
}

double dispersion_sources::stddev() const
{
    return sdev * multiplier;
}

double dispersion_sources::cdf( double x ) const {
    static const double sqrt2 = std::sqrt( 2.0 );
    return std::erf( x / ( stddev() * sqrt2 ) );
}

