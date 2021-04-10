#pragma once
#ifndef CATA_SRC_UNITS_ENERGY_H
#define CATA_SRC_UNITS_ENERGY_H

#include "units_def.h"

namespace units
{

class energy_in_millijoule_tag
{
};

using energy = quantity<int, energy_in_millijoule_tag>;

const energy energy_min = units::energy( std::numeric_limits<units::energy::value_type>::min(),
                          units::energy::unit_type{} );

const energy energy_max = units::energy( std::numeric_limits<units::energy::value_type>::max(),
                          units::energy::unit_type{} );

template<typename value_type>
inline constexpr quantity<value_type, energy_in_millijoule_tag> from_millijoule(
    const value_type v )
{
    return quantity<value_type, energy_in_millijoule_tag>( v, energy_in_millijoule_tag{} );
}

template<typename value_type>
inline constexpr quantity<value_type, energy_in_millijoule_tag> from_joule( const value_type v )
{
    const value_type max_energy_joules = std::numeric_limits<value_type>::max() / 1000;
    // Check for overflow - if the energy provided is greater than max energy, then it
    // if overflow when converted to millijoules
    const value_type energy = v > max_energy_joules ? max_energy_joules : v;
    return from_millijoule<value_type>( energy * 1000 );
}

template<typename value_type>
inline constexpr quantity<value_type, energy_in_millijoule_tag> from_kilojoule( const value_type v )
{
    const value_type max_energy_joules = std::numeric_limits<value_type>::max() / 1000;
    // This checks for value_type overflow - if the energy we are given in Joules is greater
    // than the max energy in Joules, overflow will occur when it is converted to millijoules
    // The value we are given is in kJ, multiply by 1000 to convert it to joules, for use in from_joule
    value_type energy = v * 1000 > max_energy_joules ? max_energy_joules : v * 1000;
    return from_joule<value_type>( energy );
}

template<typename value_type>
inline constexpr value_type to_millijoule( const quantity<value_type, energy_in_millijoule_tag> &v )
{
    return v / from_millijoule<value_type>( 1 );
}

template<typename value_type>
inline constexpr value_type to_joule( const quantity<value_type, energy_in_millijoule_tag> &v )
{
    return to_millijoule( v ) / 1000.0;
}

template<typename value_type>
inline constexpr value_type to_kilojoule( const quantity<value_type, energy_in_millijoule_tag> &v )
{
    return to_joule( v ) / 1000.0;
}

} // namespace units

inline constexpr units::energy operator"" _mJ( const unsigned long long v )
{
    return units::from_millijoule( v );
}

inline constexpr units::quantity<double, units::energy_in_millijoule_tag> operator"" _mJ(
    const long double v )
{
    return units::from_millijoule( v );
}

inline constexpr units::energy operator"" _J( const unsigned long long v )
{
    return units::from_joule( v );
}

inline constexpr units::quantity<double, units::energy_in_millijoule_tag> operator"" _J(
    const long double v )
{
    return units::from_joule( v );
}

inline constexpr units::energy operator"" _kJ( const unsigned long long v )
{
    return units::from_kilojoule( v );
}

inline constexpr units::quantity<double, units::energy_in_millijoule_tag> operator"" _kJ(
    const long double v )
{
    return units::from_kilojoule( v );
}


#endif // CATA_SRC_UNITS_ENERGY_H