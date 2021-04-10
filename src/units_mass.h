#pragma once
#ifndef CATA_SRC_UNITS_MASS_H
#define CATA_SRC_UNITS_MASS_H

#include "units_def.h"

namespace units
{

class mass_in_milligram_tag
{
};

using mass = quantity<std::int64_t, mass_in_milligram_tag>;

const mass mass_min = units::mass( std::numeric_limits<units::mass::value_type>::min(),
                                   units::mass::unit_type{} );

const mass mass_max = units::mass( std::numeric_limits<units::mass::value_type>::max(),
                                   units::mass::unit_type{} );

template<typename value_type>
inline constexpr quantity<value_type, mass_in_milligram_tag> from_milligram(
    const value_type v )
{
    return quantity<value_type, mass_in_milligram_tag>( v, mass_in_milligram_tag{} );
}

template<typename value_type>
inline constexpr quantity<value_type, mass_in_milligram_tag> from_gram(
    const value_type v )
{
    return from_milligram( v * 1000 );
}

template<typename value_type>
inline constexpr quantity<value_type, mass_in_milligram_tag> from_kilogram(
    const value_type v )
{
    return from_gram( v * 1000 );
}

template<typename value_type>
inline constexpr value_type to_milligram( const quantity<value_type, mass_in_milligram_tag> &v )
{
    return v.value();
}

template<typename value_type>
inline constexpr value_type to_gram( const quantity<value_type, mass_in_milligram_tag> &v )
{
    return v.value() / 1000.0;
}

inline constexpr double to_kilogram( const mass &v )
{
    return v.value() / 1000000.0;
}

} // namespace units

// Implicitly converted to mass, which has int as value_type!
inline constexpr units::mass operator"" _milligram( const unsigned long long v )
{
    return units::from_milligram( v );
}
inline constexpr units::mass operator"" _gram( const unsigned long long v )
{
    return units::from_gram( v );
}

inline constexpr units::mass operator"" _kilogram( const unsigned long long v )
{
    return units::from_kilogram( v );
}

inline constexpr units::quantity<double, units::mass_in_milligram_tag> operator"" _milligram(
    const long double v )
{
    return units::from_milligram( v );
}

inline constexpr units::quantity<double, units::mass_in_milligram_tag> operator"" _gram(
    const long double v )
{
    return units::from_gram( v );
}

inline constexpr units::quantity<double, units::mass_in_milligram_tag> operator"" _kilogram(
    const long double v )
{
    return units::from_kilogram( v );
}


#endif // CATA_SRC_UNITS_MASS_H