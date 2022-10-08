#pragma once
#ifndef CATA_SRC_LINE_H
#define CATA_SRC_LINE_H

#include <cmath>
#include <functional>
#include <string>
#include <vector>
#include <algorithm>

#include "math_defines.h"
#include "point.h"
#include "cached_options.h"
#include "units_angle.h"

/**
 * Calculate base of an isosceles triangle
 * @param distance one of the equal lengths
 * @param vertex the unequal angle
 * @returns base in equivalent units to distance
 */
auto iso_tangent( double distance, units::angle vertex ) -> double;

//! This compile-time usable function combines the sign of each (x, y, z) component into a single integer
//! to allow simple runtime and compile-time mapping of (x, y, z) tuples to @ref direction enumerators.
//! Specifically, (0, -, +) => (0, 1, 2); a base-3 number.
//! This only works correctly for inputs between -1,-1,-1 and 1,1,1.
//! For numbers outside that range, use make_xyz().
inline constexpr auto make_xyz_unit( const tripoint &p ) noexcept -> unsigned
{
    return ( ( p.x > 0 ) ? 2u : ( p.x < 0 ) ? 1u : 0u ) * 1u +
           ( ( p.y > 0 ) ? 2u : ( p.y < 0 ) ? 1u : 0u ) * 3u +
           ( ( p.z > 0 ) ? 2u : ( p.z < 0 ) ? 1u : 0u ) * 9u;
}

// This more general version of this function gives correct values for larger inputs.
auto make_xyz( const tripoint & ) -> unsigned;

enum class direction : unsigned {
    ABOVENORTHWEST = make_xyz_unit( tripoint_above + tripoint_north_west ),
    NORTHWEST      = make_xyz_unit( tripoint_north_west ),
    BELOWNORTHWEST = make_xyz_unit( tripoint_below + tripoint_north_west ),
    ABOVENORTH     = make_xyz_unit( tripoint_above + tripoint_north ),
    NORTH          = make_xyz_unit( tripoint_north ),
    BELOWNORTH     = make_xyz_unit( tripoint_below + tripoint_north ),
    ABOVENORTHEAST = make_xyz_unit( tripoint_above + tripoint_north_east ),
    NORTHEAST      = make_xyz_unit( tripoint_north_east ),
    BELOWNORTHEAST = make_xyz_unit( tripoint_below + tripoint_north_east ),

    ABOVEWEST      = make_xyz_unit( tripoint_above + tripoint_west ),
    WEST           = make_xyz_unit( tripoint_west ),
    BELOWWEST      = make_xyz_unit( tripoint_below + tripoint_west ),
    ABOVECENTER    = make_xyz_unit( tripoint_above ),
    CENTER         = make_xyz_unit( tripoint_zero ),
    BELOWCENTER    = make_xyz_unit( tripoint_below ),
    ABOVEEAST      = make_xyz_unit( tripoint_above + tripoint_east ),
    EAST           = make_xyz_unit( tripoint_east ),
    BELOWEAST      = make_xyz_unit( tripoint_below + tripoint_east ),

    ABOVESOUTHWEST = make_xyz_unit( tripoint_above + tripoint_south_west ),
    SOUTHWEST      = make_xyz_unit( tripoint_south_west ),
    BELOWSOUTHWEST = make_xyz_unit( tripoint_below + tripoint_south_west ),
    ABOVESOUTH     = make_xyz_unit( tripoint_above + tripoint_south ),
    SOUTH          = make_xyz_unit( tripoint_south ),
    BELOWSOUTH     = make_xyz_unit( tripoint_below + tripoint_south ),
    ABOVESOUTHEAST = make_xyz_unit( tripoint_above + tripoint_south_east ),
    SOUTHEAST      = make_xyz_unit( tripoint_south_east ),
    BELOWSOUTHEAST = make_xyz_unit( tripoint_below + tripoint_south_east ),
};

template< class T >
constexpr inline auto operator%( const direction &lhs, const T &rhs ) -> direction
{
    return static_cast<direction>( static_cast<T>( lhs ) % rhs );
}

template< class T >
constexpr inline auto operator+( const direction &lhs, const T &rhs ) -> T
{
    return static_cast<T>( lhs ) + rhs;
}

template< class T >
constexpr inline auto operator==( const direction &lhs, const T &rhs ) -> bool
{
    return static_cast<T>( lhs ) == rhs;
}

template< class T >
constexpr inline auto operator==( const T &lhs, const direction &rhs ) -> bool
{
    return operator==( rhs, lhs );
}

template< class T >
constexpr inline auto operator!=( const T &lhs, const direction &rhs ) -> bool
{
    return !operator==( rhs, lhs );
}

template< class T >
constexpr inline auto operator!=( const direction &lhs, const T &rhs ) -> bool
{
    return !operator==( lhs, rhs );
}

auto direction_from( const point &p ) noexcept -> direction;
auto direction_from( const tripoint &p ) noexcept -> direction;
auto direction_from( const point &p1, const point &p2 ) noexcept -> direction;
auto direction_from( const tripoint &p, const tripoint &q ) -> direction;

auto direction_XY( direction dir ) -> point;
auto direction_name( direction dir ) -> std::string;
auto direction_name_short( direction dir ) -> std::string;

/* Get suffix describing vector from p to q (e.g. 1NW, 2SE) or empty string if p == q */
auto direction_suffix( const tripoint &p, const tripoint &q ) -> std::string;

/**
 * The actual Bresenham algorithm in 2D and 3D, everything else should call these
 * and pass in an interact functor to iterate across a line between two points.
 */
void bresenham( const point &p1, const point &p2, int t,
                const std::function<bool( const point & )> &interact );
void bresenham( const tripoint &loc1, const tripoint &loc2, int t, int t2,
                const std::function<bool( const tripoint & )> &interact );

auto move_along_line( const tripoint &loc, const std::vector<tripoint> &line,
                          int distance ) -> tripoint;
// The "t" value decides WHICH Bresenham line is used.
auto line_to( const point &p1, const point &p2, int t = 0 ) -> std::vector<point>;
// t and t2 decide which Bresenham line is used.
auto line_to( const tripoint &loc1, const tripoint &loc2, int t = 0, int t2 = 0 ) -> std::vector<tripoint>;
// sqrt(dX^2 + dY^2)

inline auto trig_dist_squared( const tripoint &loc1, const tripoint &loc2 ) -> int
{
    return ( ( loc1.x - loc2.x ) * ( loc1.x - loc2.x ) ) +
           ( ( loc1.y - loc2.y ) * ( loc1.y - loc2.y ) ) +
           ( ( loc1.z - loc2.z ) * ( loc1.z - loc2.z ) );
}
inline auto trig_dist( const tripoint &loc1, const tripoint &loc2 ) -> float
{
    return std::sqrt( static_cast<double>( trig_dist_squared( loc1, loc2 ) ) );
}
inline auto trig_dist( const point &loc1, const point &loc2 ) -> float
{
    return trig_dist( tripoint( loc1, 0 ), tripoint( loc2, 0 ) );
}

// Chebyshev distance; maximum of dX and dY
inline auto square_dist( const tripoint &loc1, const tripoint &loc2 ) -> int
{
    const tripoint d = ( loc1 - loc2 ).abs();
    return std::max( { d.x, d.y, d.z } );
}
inline auto square_dist( const point &loc1, const point &loc2 ) -> int
{
    const point d = ( loc1 - loc2 ).abs();
    return std::max( d.x, d.y );
}

// Choose between the above two according to the "circular distances" option
inline auto rl_dist( const tripoint &loc1, const tripoint &loc2 ) -> int
{
    if( trigdist ) {
        return trig_dist( loc1, loc2 );
    }
    return square_dist( loc1, loc2 );
}
inline auto rl_dist( const point &a, const point &b ) -> int
{
    return rl_dist( tripoint( a, 0 ), tripoint( b, 0 ) );
}

/**
 * Helper type for the return value of dist_fast().
 *
 * This lets us delay the sqrt() call of trigdist until the actual length is needed.
 */
struct FastDistanceApproximation {
    private:
        int value;
    public:
        inline FastDistanceApproximation( int value ) : value( value ) { }
        template<typename T>
        inline auto operator<=( const T &rhs ) const -> bool {
            if( trigdist ) {
                return value <= rhs * rhs;
            }
            return value <= rhs;
        }
        template<typename T>
        inline auto operator>=( const T &rhs ) const -> bool {
            if( trigdist ) {
                return value >= rhs * rhs;
            }
            return value >= rhs;
        }
        inline operator int() const {
            if( trigdist ) {
                return std::sqrt( value );
            }
            return value;
        }
};

inline auto trig_dist_fast( const tripoint &loc1, const tripoint &loc2 ) -> FastDistanceApproximation
{
    return ( loc1.x - loc2.x ) * ( loc1.x - loc2.x ) +
           ( loc1.y - loc2.y ) * ( loc1.y - loc2.y ) +
           ( loc1.z - loc2.z ) * ( loc1.z - loc2.z );
}
inline auto square_dist_fast( const tripoint &loc1, const tripoint &loc2 ) -> FastDistanceApproximation
{
    const tripoint d = ( loc1 - loc2 ).abs();
    return std::max( { d.x, d.y, d.z } );
}
inline auto rl_dist_fast( const tripoint &loc1, const tripoint &loc2 ) -> FastDistanceApproximation
{
    if( trigdist ) {
        return trig_dist_fast( loc1, loc2 );
    }
    return square_dist_fast( loc1, loc2 );
}
inline auto rl_dist_fast( const point &a, const point &b ) -> FastDistanceApproximation
{
    return rl_dist_fast( tripoint( a, 0 ), tripoint( b, 0 ) );
}

auto rl_dist_exact( const tripoint &loc1, const tripoint &loc2 ) -> float;
// Sum of distance in both axes
auto manhattan_dist( const point &loc1, const point &loc2 ) -> int;

// Travel distance between 2 points on a square grid, assuming diagonal moves
// cost sqrt(2) and cardinal moves cost 1.
auto octile_dist( const point &loc1, const point &loc2, int multiplier = 1 ) -> int;
auto octile_dist_exact( const point &loc1, const point &loc2 ) -> float;

// get angle of direction represented by point
auto atan2( const point & ) -> units::angle;

// Get the magnitude of the slope ranging from 0.0 to 1.0
auto get_normalized_angle( const point &start, const point &end ) -> float;
auto continue_line( const std::vector<tripoint> &line, int distance ) -> std::vector<tripoint>;
auto squares_in_direction( const point &p1, const point &p2 ) -> std::vector<point>;
// Returns a vector of squares adjacent to @from that are closer to @to than @from is.
// Currently limited to the same z-level as @from.
auto squares_closer_to( const tripoint &from, const tripoint &to ) -> std::vector<tripoint>;
void calc_ray_end( units::angle, int range, const tripoint &p, tripoint &out );
/**
 * Calculates the horizontal angle between the lines from (0,0,0) to @p a and
 * the line from (0,0,0) to @p b.
 * Returned value is in degree and in the range 0....360.
 * Example: if @p a is (0,1) and @p b is (1,0), the result will 90 degree
 * The function currently ignores the z component.
 */
auto coord_to_angle( const tripoint &a, const tripoint &b ) -> units::angle;

#endif // CATA_SRC_LINE_H
