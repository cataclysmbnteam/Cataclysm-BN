#pragma once
#ifndef CATA_SRC_TILERAY_H
#define CATA_SRC_TILERAY_H

#include "point.h"
#include "units_angle.h"

// Class for calculating tile coordinates
// of a point that moves along the ray with given
// direction (dir) or delta tile coordinates (dx, dy).
// Advance method will move to the next tile
// along ray
// Direction is angle in degrees from positive x-axis to positive y-axis:
//
//       | 270         orthogonal dir left (-)
// 180   |     0         ^
//   ----+----> X    -------> X (forward dir, 0 degrees)
//       |               v
//       V 90          orthogonal dir right (+)
//       Y
//
// note to future developers: tilerays can't be cached at the tileray level,
// because tileray values depend on leftover, and thus tileray.advance(1)
// changes depending on previous calls to advance.

class tileray
{
    private:
        point delta;            // ray delta
        int leftover = 0;       // counter to shift coordinates
        point abs_d;            // absolute value of delta
        units::angle direction = 0_degrees; // ray direction
        point last_d;           // delta of last advance
        int steps = 0;          // how many steps we advanced so far
        bool infinite = false;  // ray is infinite (end will always return true)
    public:
        tileray();
        tileray( const point &ad );
        tileray( units::angle adir );

        void init( const point &ad );   // init ray with ad
        void init( units::angle adir ); // init ray with direction

        auto dx() const -> int;       // return dx of last advance (-1 to 1)
        auto dy() const -> int;       // return dy of last advance (-1 to 1)
        auto dir() const -> units::angle;      // return direction of ray
        auto quadrant() const -> int;
        auto dir4() const -> int;     // return 4-sided direction (0 = east, 1 = south, 2 = west, 3 = north)
        auto dir8() const -> int;     // return 8-sided direction (0 = east, 1 = southeast, 2 = south ...)
        // convert certain symbols from north-facing variant into current dir facing
        auto dir_symbol( int sym ) const -> int;

        /** convert to a string representation of the azimuth from north, in integer degrees */
        auto to_string_azimuth_from_north() const -> std::string;

        // return dx for point at "od" distance in orthogonal direction
        auto ortho_dx( int od ) const -> int;
        // return dy for point at "od" distance in orthogonal direction
        auto ortho_dy( int od ) const -> int;
        auto mostly_vertical() const -> bool;  // return if ray is mostly vertical

        void advance( int num = 1 ); // move to the next tile (calculate last dx, dy)
        void clear_advance(); // clear steps, leftover, last_dx, and last_dy
        auto get_steps() const -> int;  // how many steps we advanced
};

#endif // CATA_SRC_TILERAY_H
