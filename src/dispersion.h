#pragma once
#ifndef DISPERSION_H
#define DISPERSION_H

#include <iosfwd>
#include <vector>

class dispersion_sources
{
    private:
        double sdev = 0.0;
        double multiplier = 1.0 / dispersion_sigmas;
    public:
        dispersion_sources( double normal_source = 0.0 ) {
            sdev = normal_source;
        }
        void add_range( double new_source ) {
            sdev += new_source;
        }
        void add_multiplier( double new_multiplier ) {
            multiplier *= new_multiplier;
        }
        double stddev() const;
        double roll() const;
        /**
         * Cumulative distribution function.
         * That is, chance that @ref roll will return no more than x.
         */
        double cdf( double x ) const;

        static constexpr double dispersion_sigmas = 1.3;

        friend std::ostream &operator<<( std::ostream &stream, const dispersion_sources &sources );
};

#endif
