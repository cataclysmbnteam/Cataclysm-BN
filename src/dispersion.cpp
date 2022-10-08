#include "dispersion.h"

#include "rng.h"

auto dispersion_sources::roll() const -> double
{
    double this_roll = 0.0;
    for( const double &source : linear_sources ) {
        this_roll += rng_float( 0.0, source );
    }
    for( const double &source : normal_sources ) {
        this_roll += rng_normal( source );
    }
    for( const double &source : multipliers ) {
        this_roll *= source;
    }
    return this_roll;
}

auto dispersion_sources::max() const -> double
{
    double sum = 0.0;
    for( const double &source : linear_sources ) {
        sum += source;
    }
    for( const double &source : normal_sources ) {
        sum += source;
    }
    for( const double &source : multipliers ) {
        sum *= source;
    }
    return sum;
}

auto dispersion_sources::avg() const -> double
{
    return max() / 2.0;
}
