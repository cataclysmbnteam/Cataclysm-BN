#pragma once
#ifndef CATA_SRC_UNITS_ANGLE_H
#define CATA_SRC_UNITS_ANGLE_H

#include <algorithm>

#include "units_def.h"
#include "math_defines.h"

namespace units
{

class angle_in_radians_tag
{
};

using angle = quantity<double, angle_in_radians_tag>;

template<typename value_type>
inline constexpr quantity<value_type, angle_in_radians_tag> from_radians( const value_type v )
{
    return quantity<value_type, angle_in_radians_tag>( v, angle_in_radians_tag{} );
}

inline constexpr double to_radians( const units::angle v )
{
    return v.value();
}

template<typename value_type>
inline constexpr quantity<double, angle_in_radians_tag> from_degrees( const value_type v )
{
    return from_radians( v * M_PI / 180 );
}

inline constexpr double to_degrees( const units::angle v )
{
    return to_radians( v ) * 180 / M_PI;
}

template<typename value_type>
inline constexpr quantity<double, angle_in_radians_tag> from_arcmin( const value_type v )
{
    return from_degrees( v / 60.0 );
}

inline constexpr double to_arcmin( const units::angle v )
{
    return to_degrees( v ) * 60;
}

inline double sin( angle a )
{
    return std::sin( to_radians( a ) );
}

inline double cos( angle a )
{
    return std::cos( to_radians( a ) );
}

inline double tan( angle a )
{
    return std::tan( to_radians( a ) );
}

inline units::angle atan2( double y, double x )
{
    return from_radians( std::atan2( y, x ) );
}

} // namespace units

inline constexpr units::angle operator"" _radians( const long double v )
{
    return units::from_radians( v );
}

inline constexpr units::angle operator"" _radians( const unsigned long long v )
{
    return units::from_radians( v );
}

inline constexpr units::angle operator"" _pi_radians( const long double v )
{
    return units::from_radians( v * M_PI );
}

inline constexpr units::angle operator"" _pi_radians( const unsigned long long v )
{
    return units::from_radians( v * M_PI );
}

inline constexpr units::angle operator"" _degrees( const long double v )
{
    return units::from_degrees( v );
}

inline constexpr units::angle operator"" _degrees( const unsigned long long v )
{
    return units::from_degrees( v );
}

inline constexpr units::angle operator"" _arcmin( const long double v )
{
    return units::from_arcmin( v );
}

inline constexpr units::angle operator"" _arcmin( const unsigned long long v )
{
    return units::from_arcmin( v );
}

#endif // CATA_SRC_UNITS_ANGLE_H
