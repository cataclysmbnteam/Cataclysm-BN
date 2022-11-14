#pragma once
#ifndef CATA_SRC_OM_DIRECTION_H
#define CATA_SRC_OM_DIRECTION_H

#include <climits>
#include <array>
#include <string>

struct point;
struct tripoint;

/** Direction on the overmap. */
namespace om_direction
{
/** Basic enum for directions. */
enum class type : int {
    invalid = -1,
    none,
    north = none,
    east,
    south,
    west,
};

/** For the purposes of iteration. */
const std::array<type, 4> all = {{ type::north, type::east, type::south, type::west }};
const size_t size = all.size();

const std::array<std::string, 4> all_suffixes = {{ "_north", "_east", "_south", "_west" }};
const std::string invalid_dir_suffix;
const std::array<int, 4> all_cw_rotations = {{ 0, 1, 2, 3 }};
const std::array<int, 4> all_ccw_rotations = { { 0, 3, 2, 1 } };
const int invalid_dir_rotations = 0;

/** Returns directional suffix associated with the value, e.g. _north or _west. */
constexpr const std::string &get_suffix( type dir )
{
    if( dir == type::invalid ) {
        return invalid_dir_suffix;
    } else {
        return all_suffixes[static_cast<size_t>( dir )];
    }
}

/** Returns number of clockwise rotations needed to reach this direction from 'north'. */
constexpr int get_num_cw_rotations( type dir )
{
    if( dir == type::invalid ) {
        return invalid_dir_rotations;
    } else {
        return all_cw_rotations[static_cast<size_t>( dir )];
    }
}

/** Returns number of counterclockwise rotations needed to reach this direction from 'north'. */
constexpr int get_num_ccw_rotations( type dir )
{
    if( dir == type::invalid ) {
        return invalid_dir_rotations;
    } else {
        return all_ccw_rotations[static_cast<size_t>( dir )];
    }
}

/** Number of bits needed to store directions. */
const size_t bits = static_cast<size_t>( -1 ) >> ( CHAR_BIT *sizeof( size_t ) - size );

/** Identifier for serialization purposes. */
const std::string &id( type dir );

/** Get Human readable name of a direction */
std::string name( type dir );

/** Various rotations. */
point rotate( const point &p, type dir );
tripoint rotate( const tripoint &p, type dir );
uint32_t rotate_symbol( uint32_t sym, type dir );

/** Returns point(0, 0) displaced in specified direction by a specified distance
 * @param dir Direction of displacement
 * @param dist Distance of displacement
 */
point displace( type dir, int dist = 1 );

/** Returns a sum of two numbers
 *  @param dir1 first number
 *  @param dir2 second number */
type add( type dir1, type dir2 );

/** Turn by 90 degrees to the left, to the right, or randomly (either left or right). */
type turn_left( type dir );
type turn_right( type dir );
type turn_random( type dir );

/** Returns an opposite direction. */
type opposite( type dir );

/** Returns a random direction. */
type random();

/** Whether these directions are parallel. */
bool are_parallel( type dir1, type dir2 );

} // namespace om_direction

#endif // CATA_SRC_OM_DIRECTION_H
