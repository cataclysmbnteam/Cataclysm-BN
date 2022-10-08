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
inline constexpr auto from_radians( const value_type v ) -> quantity<value_type, angle_in_radians_tag>
{
    return quantity<value_type, angle_in_radians_tag>( v, angle_in_radians_tag{} );
}

inline constexpr auto to_radians( const units::angle v ) -> double
{
    return v.value();
}

template<typename value_type>
inline constexpr auto from_degrees( const value_type v ) -> quantity<double, angle_in_radians_tag>
{
    return from_radians( v * M_PI / 180 );
}

inline constexpr auto to_degrees( const units::angle v ) -> double
{
    return to_radians( v ) * 180 / M_PI;
}

template<typename value_type>
inline constexpr auto from_arcmin( const value_type v ) -> quantity<double, angle_in_radians_tag>
{
    return from_degrees( v / 60.0 );
}

inline constexpr auto to_arcmin( const units::angle v ) -> double
{
    return to_degrees( v ) * 60;
}

inline auto sin( angle a ) -> double
{
    return std::sin( to_radians( a ) );
}

inline auto cos( angle a ) -> double
{
    return std::cos( to_radians( a ) );
}

inline auto tan( angle a ) -> double
{
    return std::tan( to_radians( a ) );
}

inline auto atan2( double y, double x ) -> units::angle
{
    return from_radians( std::atan2( y, x ) );
}

} // namespace units

inline constexpr auto operator"" _radians( const long double v ) -> units::angle
{
    return units::from_radians( v );
}

inline constexpr auto operator"" _radians( const unsigned long long v ) -> units::angle
{
    return units::from_radians( v );
}

inline constexpr auto operator"" _pi_radians( const long double v ) -> units::angle
{
    return units::from_radians( v * M_PI );
}

inline constexpr auto operator"" _pi_radians( const unsigned long long v ) -> units::angle
{
    return units::from_radians( v * M_PI );
}

inline constexpr auto operator"" _degrees( const long double v ) -> units::angle
{
    return units::from_degrees( v );
}

inline constexpr auto operator"" _degrees( const unsigned long long v ) -> units::angle
{
    return units::from_degrees( v );
}

inline constexpr auto operator"" _arcmin( const long double v ) -> units::angle
{
    return units::from_arcmin( v );
}

inline constexpr auto operator"" _arcmin( const unsigned long long v ) -> units::angle
{
    return units::from_arcmin( v );
}

#endif // CATA_SRC_UNITS_ANGLE_H
