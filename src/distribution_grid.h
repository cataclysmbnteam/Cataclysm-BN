#pragma once
#ifndef CATA_SRC_DISTRIBUTION_GRID_H
#define CATA_SRC_DISTRIBUTION_GRID_H

#include <cstdint>
#include <vector>
#include <map>

#include "calendar.h"
#include "coordinates.h"
#include "cuboid_rectangle.h"
#include "memory_fast.h"
#include "point.h"

class map;
class mapbuffer;

struct tile_location {
    point_sm_ms on_submap;
    tripoint_abs_ms absolute;

    tile_location( point_sm_ms on_submap, tripoint_abs_ms absolute )
        : on_submap( on_submap )
        , absolute( absolute )
    {}
};

/**
 * A cache that organizes producers, storage and consumers
 * of some resource, like electricity.
 * WARNING: Shouldn't be stored, as out of date grids are not updated.
 */
class distribution_grid
{
    private:
        friend class distribution_grid_tracker;

        /**
         * Map of submap coords to points on this submap
         * that contain an active tile.
         */
        std::map<tripoint_abs_sm, std::vector<tile_location>> contents;
        std::vector<tripoint_abs_ms> flat_contents;
        std::vector<tripoint_abs_sm> submap_coords;

        mapbuffer &mb;

    public:
        distribution_grid( const std::vector<tripoint_abs_sm> &global_submap_coords, mapbuffer &buffer );
        bool empty() const;
        explicit operator bool() const;
        void update( time_point to );
        int mod_resource( int amt, bool recurse = true );
        int get_resource( bool recurse = true ) const;
        const std::vector<tripoint_abs_ms> &get_contents() const {
            return flat_contents;
        }
};

/**
 * Contains and manages all the active distribution grids.
 */
class distribution_grid_tracker
{
    private:
        /**
         * Mapping of sm position to grid it belongs to.
         */
        std::map<tripoint_abs_sm, shared_ptr_fast<distribution_grid>> parent_distribution_grids;

        /**
         * @param omt_pos Absolute submap position of one of the tiles of the grid.
         */
        distribution_grid &make_distribution_grid_at( const tripoint_abs_sm &sm_pos );

        /**
         * In submap coords, to mirror @ref map
         */
        half_open_rectangle<point_abs_sm> bounds;

        mapbuffer &mb;

    public:
        distribution_grid_tracker();
        distribution_grid_tracker( mapbuffer &buffer );
        distribution_grid_tracker( distribution_grid_tracker && ) = default;
        /**
         * Gets grid at given global map square coordinate. @ref map::getabs
         */
        /**@{*/
        distribution_grid &grid_at( const tripoint_abs_ms &p );
        const distribution_grid &grid_at( const tripoint_abs_ms &p ) const;
        /*@}*/

        /**
         * Identify grid at given overmap tile (for debug purposes).
         * @returns 0 if there's no grid.
         */
        std::uintptr_t debug_grid_id( const tripoint_abs_omt &omp ) const;

        void update( time_point to );
        /**
         * Loads grids in an area given by submap coords.
         */
        void load( half_open_rectangle<point_abs_sm> area );
        /**
         * Loads grids in the same area as a given map.
         */
        void load( const map &m );

        /**
         * Updates grid at given global map square coordinate.
         */
        void on_changed( const tripoint_abs_ms &p );
        void on_saved();
        void on_options_changed();
};

namespace distribution_graph
{

/**
* Traverses the graph of connected vehicles and grids.
*/
template <typename VehFunc, typename GridFunc, typename StartPoint>
int traverse( StartPoint *start, int amount,
              VehFunc veh_action, GridFunc grid_action );

} // namespace distribution_graph

/**
 * Returns distribution grid tracker that is a part of the global game *g. @ref game
 * TODO: This wouldn't be required in an ideal world
 */
distribution_grid_tracker &get_distribution_grid_tracker();

#endif // CATA_SRC_DISTRIBUTION_GRID_H
