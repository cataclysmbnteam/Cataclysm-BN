#pragma once
#ifndef CATA_SRC_SUBMAP_H
#define CATA_SRC_SUBMAP_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <iterator>
#include <map>

#include "active_item_cache.h"
#include "active_tile_data.h"
#include "calendar.h"
#include "colony.h"
#include "computer.h"
#include "construction_partial.h"
#include "field.h"
#include "game_constants.h"
#include "item.h"
#include "type_id.h"
#include "point.h"
#include "poly_serialized.h"

class JsonIn;
class JsonOut;
class basecamp;
class map;
struct trap;
struct ter_t;
struct furn_t;
class vehicle;

struct spawn_point {
    point pos;
    int count;
    mtype_id type;
    int faction_id;
    int mission_id;
    bool friendly;
    std::string name;
    spawn_point( const mtype_id &T = mtype_id::NULL_ID(), int C = 0, point P = point_zero,
                 int FAC = -1, int MIS = -1, bool F = false,
                 const std::string &N = "NONE" ) :
        pos( P ), count( C ), type( T ), faction_id( FAC ),
        mission_id( MIS ), friendly( F ), name( N ) {}
};

template<int sx, int sy>
struct maptile_soa {
    ter_id             ter[sx][sy];  // Terrain on each square
    furn_id            frn[sx][sy];  // Furniture on each square
    std::uint8_t       lum[sx][sy];  // Number of items emitting light on each square
    cata::colony<item> itm[sx][sy];  // Items on each square
    field              fld[sx][sy];  // Field on each square
    trap_id            trp[sx][sy];  // Trap on each square
    int                rad[sx][sy];  // Irradiation of each square

    void swap_soa_tile( const point &p1, const point &p2 );
    void swap_soa_tile( const point &p, maptile_soa<1, 1> &other );
};

class submap : maptile_soa<SEEX, SEEY>
{
    public:
        submap();
        submap( submap && );
        ~submap();

        auto operator=( submap && ) -> submap &;

        auto get_trap( const point &p ) const -> trap_id {
            return trp[p.x][p.y];
        }

        void set_trap( const point &p, trap_id trap ) {
            is_uniform = false;
            trp[p.x][p.y] = trap;
        }

        void set_all_traps( const trap_id &trap ) {
            std::uninitialized_fill_n( &trp[0][0], elements, trap );
        }

        auto get_furn( const point &p ) const -> furn_id {
            return frn[p.x][p.y];
        }

        void set_furn( const point &p, furn_id furn ) {
            is_uniform = false;
            frn[p.x][p.y] = furn;
        }

        void set_all_furn( const furn_id &furn ) {
            std::uninitialized_fill_n( &frn[0][0], elements, furn );
        }

        auto get_ter( const point &p ) const -> ter_id {
            return ter[p.x][p.y];
        }

        void set_ter( const point &p, ter_id terr ) {
            is_uniform = false;
            ter[p.x][p.y] = terr;
        }

        void set_all_ter( const ter_id &terr ) {
            std::uninitialized_fill_n( &ter[0][0], elements, terr );
        }

        auto get_radiation( const point &p ) const -> int {
            return rad[p.x][p.y];
        }

        void set_radiation( const point &p, const int radiation ) {
            is_uniform = false;
            rad[p.x][p.y] = radiation;
        }

        auto get_lum( const point &p ) const -> uint8_t {
            return lum[p.x][p.y];
        }

        void set_lum( const point &p, uint8_t luminance ) {
            is_uniform = false;
            lum[p.x][p.y] = luminance;
        }

        void update_lum_add( const point &p, const item &i ) {
            is_uniform = false;
            if( i.is_emissive() && lum[p.x][p.y] < 255 ) {
                lum[p.x][p.y]++;
            }
        }

        void update_lum_rem( const point &p, const item &i ) {
            is_uniform = false;
            if( !i.is_emissive() ) {
                return;
            } else if( lum[p.x][p.y] && lum[p.x][p.y] < 255 ) {
                lum[p.x][p.y]--;
                return;
            }

            // Have to scan through all items to be sure removing i will actually lower
            // the count below 255.
            int count = 0;
            for( const auto &it : itm[p.x][p.y] ) {
                if( it.is_emissive() ) {
                    count++;
                }
            }

            if( count <= 256 ) {
                lum[p.x][p.y] = static_cast<uint8_t>( count - 1 );
            }
        }

        // TODO: Replace this as it essentially makes itm public
        auto get_items( const point &p ) -> cata::colony<item> & {
            return itm[p.x][p.y];
        }

        auto get_items( const point &p ) const -> const cata::colony<item> & {
            return itm[p.x][p.y];
        }

        // TODO: Replace this as it essentially makes fld public
        auto get_field( const point &p ) -> field & {
            return fld[p.x][p.y];
        }

        auto get_field( const point &p ) const -> const field & {
            return fld[p.x][p.y];
        }

        struct cosmetic_t {
            point pos;
            std::string type;
            std::string str;
        };

        void insert_cosmetic( const point &p, const std::string &type, const std::string &str ) {
            cosmetic_t ins;

            ins.pos = p;
            ins.type = type;
            ins.str = str;

            cosmetics.push_back( ins );
        }

        auto get_temperature() const -> int {
            return temperature;
        }

        void set_temperature( int new_temperature ) {
            temperature = new_temperature;
        }

        auto has_graffiti( const point &p ) const -> bool;
        auto get_graffiti( const point &p ) const -> const std::string &;
        void set_graffiti( const point &p, const std::string &new_graffiti );
        void delete_graffiti( const point &p );

        // Signage is a pretend union between furniture on a square and stored
        // writing on the square. When both are present, we have signage.
        // Its effect is meant to be cosmetic and atmospheric only.
        auto has_signage( const point &p ) const -> bool;
        // Dependent on furniture + cosmetics.
        auto get_signage( const point &p ) const -> std::string;
        // Can be used anytime (prevents code from needing to place sign first.)
        void set_signage( const point &p, const std::string &s );
        // Can be used anytime (prevents code from needing to place sign first.)
        void delete_signage( const point &p );

        auto has_computer( const point &p ) const -> bool;
        auto get_computer( const point &p ) const -> const computer *;
        auto get_computer( const point &p ) -> computer *;
        void set_computer( const point &p, const computer &c );
        void delete_computer( const point &p );

        auto contains_vehicle( vehicle * ) -> bool;

        void rotate( int turns );

        void store( JsonOut &jsout ) const;
        void load( JsonIn &jsin, const std::string &member_name, int version );

        // If is_uniform is true, this submap is a solid block of terrain
        // Uniform submaps aren't saved/loaded, because regenerating them is faster
        bool is_uniform;

        std::vector<cosmetic_t> cosmetics; // Textual "visuals" for squares

        active_item_cache active_items;

        int field_count = 0;
        time_point last_touched = calendar::turn_zero;
        std::vector<spawn_point> spawns;
        /**
         * Vehicles on this submap (their (0,0) point is on this submap).
         * This vehicle objects are deleted by this submap when it gets
         * deleted.
         */
        std::vector<std::unique_ptr<vehicle>> vehicles;
        std::map<tripoint, partial_con> partial_constructions;
        std::unique_ptr<basecamp> camp;  // only allowing one basecamp per submap
        std::map<point_sm_ms, cata::poly_serialized<active_tile_data>> active_furniture;

    private:
        std::map<point, computer> computers;
        std::unique_ptr<computer> legacy_computer;
        int temperature = 0;

        void update_legacy_computer();

        static constexpr size_t elements = SEEX * SEEY;
};

/**
 * A wrapper for a submap point. Allows getting multiple map features
 * (terrain, furniture etc.) without directly accessing submaps or
 * doing multiple bounds checks and submap gets.
 */
struct maptile {
    private:
        friend map; // To allow "sliding" the tile in x/y without bounds checks
        friend submap;
        submap *const sm;
        point pos_;

        inline auto pos() const -> const point & {
            return pos_;
        }

        maptile( submap *sub, const point &p ) :
            sm( sub ), pos_( p ) { }
    public:
        auto get_trap() const -> trap_id {
            return sm->get_trap( pos() );
        }

        auto get_furn() const -> furn_id {
            return sm->get_furn( pos() );
        }

        auto get_ter() const -> ter_id {
            return sm->get_ter( pos() );
        }

        auto get_trap_t() const -> const trap & {
            return sm->get_trap( pos() ).obj();
        }

        auto get_furn_t() const -> const furn_t & {
            return sm->get_furn( pos() ).obj();
        }
        auto get_ter_t() const -> const ter_t & {
            return sm->get_ter( pos() ).obj();
        }

        auto get_field() const -> const field & {
            return sm->get_field( pos() );
        }

        auto find_field( const field_type_id &field_to_find ) -> field_entry * {
            return sm->get_field( pos() ).find_field( field_to_find );
        }

        auto get_radiation() const -> int {
            return sm->get_radiation( pos() );
        }

        auto has_graffiti() const -> bool {
            return sm->has_graffiti( pos() );
        }

        auto get_graffiti() const -> const std::string & {
            return sm->get_graffiti( pos() );
        }

        auto has_signage() const -> bool {
            return sm->has_signage( pos() );
        }

        auto get_signage() const -> std::string {
            return sm->get_signage( pos() );
        }

        // For map::draw_maptile
        auto get_item_count() const -> size_t {
            return sm->get_items( pos() ).size();
        }

        // Assumes there is at least one item
        auto get_uppermost_item() const -> const item & {
            return *std::prev( sm->get_items( pos() ).cend() );
        }
};

#endif // CATA_SRC_SUBMAP_H
