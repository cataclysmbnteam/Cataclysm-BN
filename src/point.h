#pragma once
#ifndef CATA_SRC_POINT_H
#define CATA_SRC_POINT_H

// The CATA_NO_STL macro is used by the cata clang-tidy plugin tests so they
// can include this header when compiling with -nostdinc++
#ifndef CATA_NO_STL

#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <ostream>
#include <string>
#include <vector>

#else

#define assert(...)

namespace std
{
class string;
class ostream;
}

#endif // CATA_NO_STL

class JsonIn;
class JsonOut;

// NOLINTNEXTLINE(cata-xy)
struct point {
    static constexpr int dimension = 2;

    int x = 0;
    int y = 0;
    constexpr point() = default;
    constexpr point( int X, int Y ) : x( X ), y( Y ) {}

    constexpr auto operator+( const point &rhs ) const -> point {
        return point( x + rhs.x, y + rhs.y );
    }
    auto operator+=( const point &rhs ) -> point & {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    constexpr auto operator-() const -> point {
        return point( -x, -y );
    }
    constexpr auto operator-( const point &rhs ) const -> point {
        return point( x - rhs.x, y - rhs.y );
    }
    auto operator-=( const point &rhs ) -> point & {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }
    constexpr auto operator*( const int rhs ) const -> point {
        return point( x * rhs, y * rhs );
    }
    friend constexpr auto operator*( int lhs, const point &rhs ) -> point {
        return rhs * lhs;
    }
    auto operator*=( const int rhs ) -> point & {
        x *= rhs;
        y *= rhs;
        return *this;
    }
    constexpr auto operator/( const int rhs ) const -> point {
        return point( x / rhs, y / rhs );
    }

#ifndef CATA_NO_STL
    inline auto abs() const -> point {
        return point( std::abs( x ), std::abs( y ) );
    }
#endif

    /**
     * Rotate point clockwise @param turns times, 90 degrees per turn,
     * around the center of a rectangle with the dimensions specified
     * by @param dim
     * By default rotates around the origin (0, 0).
     * NOLINTNEXTLINE(cata-use-named-point-constants) */
    auto rotate( int turns, const point &dim = { 1, 1 } ) const -> point {
        assert( turns >= 0 );
        assert( turns <= 4 );

        switch( turns ) {
            case 1:
                return { dim.y - y - 1, x };
            case 2:
                return { dim.x - x - 1, dim.y - y - 1 };
            case 3:
                return { y, dim.x - x - 1 };
        }

        return *this;
    }

    auto to_string() const -> std::string;

    void serialize( JsonOut &jsout ) const;
    void deserialize( JsonIn &jsin );

    friend inline constexpr auto operator<( const point &a, const point &b ) -> bool {
        return a.x < b.x || ( a.x == b.x && a.y < b.y );
    }
    friend inline constexpr auto operator==( const point &a, const point &b ) -> bool {
        return a.x == b.x && a.y == b.y;
    }
    friend inline constexpr auto operator!=( const point &a, const point &b ) -> bool {
        return !( a == b );
    }

    friend auto operator<<( std::ostream &, const point & ) -> std::ostream &;
};

inline auto divide_round_to_minus_infinity( int n, int d ) -> int
{
    // The NOLINT comments here are to suppress a clang-tidy warning that seems
    // to be a clang-tidy bug.  I'd like to get rid of them if the bug is ever
    // fixed.  The warning comes via a project_remain call in
    // mission_companion.cpp.
    if( n >= 0 ) {
        return n / d; // NOLINT(clang-analyzer-core.DivideZero)
    }
    return ( n - d + 1 ) / d; // NOLINT(clang-analyzer-core.DivideZero)
}

inline auto multiply_xy( const point &p, int f ) -> point
{
    return point( p.x * f, p.y * f );
}

inline auto divide_xy_round_to_minus_infinity( const point &p, int d ) -> point
{
    return point( divide_round_to_minus_infinity( p.x, d ),
                  divide_round_to_minus_infinity( p.y, d ) );
}

// NOLINTNEXTLINE(cata-xy)
struct tripoint {
    static constexpr int dimension = 3;

    int x = 0;
    int y = 0;
    int z = 0;
    constexpr tripoint() = default;
    constexpr tripoint( int X, int Y, int Z ) : x( X ), y( Y ), z( Z ) {}
    constexpr tripoint( const point &p, int Z ) : x( p.x ), y( p.y ), z( Z ) {}

    constexpr auto operator+( const tripoint &rhs ) const -> tripoint {
        return tripoint( x + rhs.x, y + rhs.y, z + rhs.z );
    }
    constexpr auto operator-( const tripoint &rhs ) const -> tripoint {
        return tripoint( x - rhs.x, y - rhs.y, z - rhs.z );
    }
    auto operator+=( const tripoint &rhs ) -> tripoint & {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
    constexpr auto operator-() const -> tripoint {
        return tripoint( -x, -y, -z );
    }
    constexpr auto operator*( const int rhs ) const -> tripoint {
        return tripoint( x * rhs, y * rhs, z * rhs );
    }
    friend constexpr auto operator*( int lhs, const tripoint &rhs ) -> tripoint {
        return rhs * lhs;
    }
    auto operator*=( const int rhs ) -> tripoint & {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }
    constexpr auto operator/( const int rhs ) const -> tripoint {
        return tripoint( x / rhs, y / rhs, z / rhs );
    }
    /*** some point operators and functions ***/
    constexpr auto operator+( const point &rhs ) const -> tripoint {
        return tripoint( x + rhs.x, y + rhs.y, z );
    }
    friend constexpr auto operator+( const point &lhs, const tripoint &rhs ) -> tripoint {
        return rhs + lhs;
    }
    constexpr auto operator-( const point &rhs ) const -> tripoint {
        return tripoint( x - rhs.x, y - rhs.y, z );
    }
    auto operator+=( const point &rhs ) -> tripoint & {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    auto operator-=( const point &rhs ) -> tripoint & {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }
    auto operator-=( const tripoint &rhs ) -> tripoint & {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

#ifndef CATA_NO_STL
    inline auto abs() const -> tripoint {
        return tripoint( std::abs( x ), std::abs( y ), std::abs( z ) );
    }
#endif

    constexpr auto xy() const -> point {
        return point( x, y );
    }

    auto to_string() const -> std::string;

    /**
     * Rotate x and y components clockwise @param turns times,
     * 90 degrees per turn, around the center of a rectangle with
     * the dimensions specified by @param dim.
     * By default rotates around the origin (0, 0).
     * NOLINTNEXTLINE(cata-use-named-point-constants) */
    inline auto rotate_2d( int turns, const point &dim = { 1, 1 } ) const -> tripoint {
        return tripoint( xy().rotate( turns, dim ), z );
    }

    void serialize( JsonOut &jsout ) const;
    void deserialize( JsonIn &jsin );

    friend auto operator<<( std::ostream &, const tripoint & ) -> std::ostream &;

    friend inline constexpr auto operator==( const tripoint &a, const tripoint &b ) -> bool {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }
    friend inline constexpr auto operator!=( const tripoint &a, const tripoint &b ) -> bool {
        return !( a == b );
    }
    friend inline auto operator<( const tripoint &a, const tripoint &b ) -> bool {
        if( a.x != b.x ) {
            return a.x < b.x;
        }
        if( a.y != b.y ) {
            return a.y < b.y;
        }
        if( a.z != b.z ) {
            return a.z < b.z;
        }
        return false;
    }
};

inline auto multiply_xy( const tripoint &p, int f ) -> tripoint
{
    return tripoint( p.x * f, p.y * f, p.z );
}

inline auto divide_xy_round_to_minus_infinity( const tripoint &p, int d ) -> tripoint
{
    return tripoint( divide_round_to_minus_infinity( p.x, d ),
                     divide_round_to_minus_infinity( p.y, d ),
                     p.z );
}

static constexpr tripoint tripoint_zero{};
static constexpr point point_zero{};

static constexpr point point_north{ 0, -1 };
static constexpr point point_north_east{ 1, -1 };
static constexpr point point_east{ 1, 0 };
static constexpr point point_south_east{ 1, 1 };
static constexpr point point_south{ 0, 1 };
static constexpr point point_south_west{ -1, 1 };
static constexpr point point_west{ -1, 0 };
static constexpr point point_north_west{ -1, -1 };

static constexpr tripoint tripoint_north{ point_north, 0 };
static constexpr tripoint tripoint_north_east{ point_north_east, 0 };
static constexpr tripoint tripoint_east{ point_east, 0 };
static constexpr tripoint tripoint_south_east{ point_south_east, 0 };
static constexpr tripoint tripoint_south{ point_south, 0 };
static constexpr tripoint tripoint_south_west{ point_south_west, 0 };
static constexpr tripoint tripoint_west{ point_west, 0 };
static constexpr tripoint tripoint_north_west{ point_north_west, 0 };

static constexpr tripoint tripoint_above{ 0, 0, 1 };
static constexpr tripoint tripoint_below{ 0, 0, -1 };

struct sphere {
    int radius = 0;
    tripoint center = tripoint_zero;

    sphere() = default;
    explicit sphere( const tripoint &center ) : radius( 1 ), center( center ) {}
    explicit sphere( const tripoint &center, int radius ) : radius( radius ), center( center ) {}
};

#ifndef CATA_NO_STL

/**
 * Following functions return points in a spiral pattern starting at center_x/center_y until it hits the radius. Clockwise fashion.
 * Credit to Tom J Nowell; http://stackoverflow.com/a/1555236/1269969
 */
auto closest_points_first( const tripoint &center, int max_dist ) -> std::vector<tripoint>;
auto closest_points_first( const tripoint &center, int min_dist, int max_dist ) -> std::vector<tripoint>;

auto closest_points_first( const point &center, int max_dist ) -> std::vector<point>;
auto closest_points_first( const point &center, int min_dist, int max_dist ) -> std::vector<point>;

static constexpr tripoint tripoint_min { INT_MIN, INT_MIN, INT_MIN };
static constexpr tripoint tripoint_max{ INT_MAX, INT_MAX, INT_MAX };

static constexpr point point_min{ tripoint_min.xy() };
static constexpr point point_max{ tripoint_max.xy() };

// Make point hashable so it can be used as an unordered_set or unordered_map key,
// or a component of one.
namespace std
{
template <>
struct hash<point> {
    auto operator()( const point &k ) const noexcept -> std::size_t {
        constexpr uint64_t a = 2862933555777941757;
        size_t result = k.y;
        result *= a;
        result += k.x;
        return result;
    }
};
} // namespace std

// Make tripoint hashable so it can be used as an unordered_set or unordered_map key,
// or a component of one.
namespace std
{
template <>
struct hash<tripoint> {
    auto operator()( const tripoint &k ) const noexcept -> std::size_t {
        constexpr uint64_t a = 2862933555777941757;
        size_t result = k.z;
        result *= a;
        result += k.y;
        result *= a;
        result += k.x;
        return result;
    }
};
} // namespace std

static constexpr std::array<point, 4> four_adjacent_offsets{{
        point_north, point_east, point_south, point_west
    }};

static constexpr std::array<point, 8> eight_adjacent_offsets{{
        point_north, point_north_east, point_east, point_south_east,
        point_south, point_south_west, point_west, point_north_west
    }};

static constexpr std::array<point, 4> neighborhood{ {
        point_south, point_east, point_west, point_north
    }};

static constexpr std::array<point, 4> offsets = {{
        point_south, point_east, point_west, point_north
    }
};

static constexpr std::array<point, 4> four_cardinal_directions{{
        point_west, point_east, point_north, point_south
    }};

static constexpr std::array<point, 5> five_cardinal_directions{{
        point_west, point_east, point_north, point_south, point_zero
    }};

static constexpr std::array<tripoint, 6> six_cardinal_directions{{
        tripoint_west, tripoint_east, tripoint_north, tripoint_south,
        tripoint_above, tripoint_below
    }};

static const std::array<tripoint, 8> eight_horizontal_neighbors = { {
        { tripoint_north_west },
        { tripoint_north },
        { tripoint_north_east },
        { tripoint_west },
        { tripoint_east },
        { tripoint_south_west },
        { tripoint_south },
        { tripoint_south_east },
    }
};

#endif // CATA_NO_STL

#endif // CATA_SRC_POINT_H
