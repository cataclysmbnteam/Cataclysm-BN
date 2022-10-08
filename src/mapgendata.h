#pragma once
#ifndef CATA_SRC_MAPGENDATA_H
#define CATA_SRC_MAPGENDATA_H

#include "calendar.h"
#include "coordinates.h"
#include "type_id.h"
#include "weighted_list.h"

struct point;
struct tripoint;
class mission;
struct regional_settings;
class map;
namespace om_direction
{
enum class type : int;
} // namespace om_direction

/**
 * Contains various information regarding the individual mapgen instance
 * (generating a specific part of the map), used by the various mapgen
 * functions to do their thing.
 *
 * Contains for example:
 * - the @ref map to generate the map onto,
 * - the overmap terrain of the area to generate and its surroundings,
 * - regional settings to use.
 *
 * An instance of this class is passed through most of the mapgen code.
 * If any of these functions need more information, add them here.
 */
// TODO: documentation
// TODO: encapsulate data member
class mapgendata
{
    private:
        oter_id terrain_type_;
        float density_;
        time_point when_;
        ::mission *mission_;
        int zlevel_;

    public:
        oter_id t_nesw[8];

        int n_fac = 0;  // dir == 0
        int e_fac = 0;  // dir == 1
        int s_fac = 0;  // dir == 2
        int w_fac = 0;  // dir == 3
        int ne_fac = 0; // dir == 4
        int se_fac = 0; // dir == 5
        int sw_fac = 0; // dir == 6
        int nw_fac = 0; // dir == 7

        oter_id t_above;
        oter_id t_below;

        const regional_settings &region;

        map &m;

        weighted_int_list<ter_id> default_groundcover;

        mapgendata( oter_id t_north, oter_id t_east, oter_id t_south, oter_id t_west,
                    oter_id northeast, oter_id southeast, oter_id southwest, oter_id northwest,
                    oter_id up, oter_id down, int z, const regional_settings &rsettings, map &mp,
                    const oter_id &terrain_type, float density, const time_point &when, ::mission *miss );

        mapgendata( const tripoint_abs_omt &over, map &m, float density, const time_point &when,
                    ::mission *miss );

        /**
         * Creates a copy of this mapgen data, but stores a different @ref terrain_type.
         * Useful when you want to create a base map (e.g. forest/field/river), that gets
         * refined later:
         * @code
         * void generate_foo( mapgendata &dat ) {
         *     mapgendata base_dat( dat, oter_id( "forest" ) );
         *     generate( base_dat );
         *     ... // refine map some more.
         * }
         * @endcode
         */
        mapgendata( const mapgendata &other, const oter_id &other_id );

        auto terrain_type() const -> const oter_id & {
            return terrain_type_;
        }
        auto monster_density() const -> float {
            return density_;
        }
        auto when() const -> const time_point & {
            return when_;
        }
        auto mission() const -> ::mission * {
            return mission_;
        }
        auto zlevel() const -> int {
            // TODO: should be able to determine this from the map itself
            return zlevel_;
        }

        void set_dir( int dir_in, int val );
        void fill( int val );
        auto dir( int dir_in ) -> int &;
        auto north() const -> const oter_id & {
            return t_nesw[0];
        }
        auto east()  const -> const oter_id & {
            return t_nesw[1];
        }
        auto south() const -> const oter_id & {
            return t_nesw[2];
        }
        auto west()  const -> const oter_id & {
            return t_nesw[3];
        }
        auto neast() const -> const oter_id & {
            return t_nesw[4];
        }
        auto seast() const -> const oter_id & {
            return t_nesw[5];
        }
        auto swest() const -> const oter_id & {
            return t_nesw[6];
        }
        auto nwest() const -> const oter_id & {
            return t_nesw[7];
        }
        auto above() const -> const oter_id & {
            return t_above;
        }
        auto below() const -> const oter_id & {
            return t_below;
        }
        auto neighbor_at( om_direction::type dir ) const -> const oter_id &;
        void fill_groundcover();
        void square_groundcover( const point &p1, const point &p2 );
        auto groundcover() -> ter_id;
        auto is_groundcover( const ter_id &iid ) const -> bool;
};

#endif // CATA_SRC_MAPGENDATA_H
