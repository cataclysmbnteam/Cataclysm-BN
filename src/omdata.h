#pragma once
#ifndef CATA_SRC_OMDATA_H
#define CATA_SRC_OMDATA_H

#include <climits>
#include <cstddef>
#include <cstdint>
#include <bitset>
#include <list>
#include <set>
#include <vector>
#include <array>
#include <string>

#include "cuboid_rectangle.h"
#include "catacharset.h"
#include "color.h"
#include "numeric_interval.h"
#include "coordinates.h"
#include "int_id.h"
#include "om_direction.h"
#include "point.h"
#include "string_id.h"
#include "translations.h"
#include "type_id.h"
#include "optional.h"

struct city;
class overmap_land_use_code;
struct MonsterGroup;

using overmap_land_use_code_id = string_id<overmap_land_use_code>;
class JsonObject;

static const overmap_land_use_code_id land_use_code_forest( "forest" );
static const overmap_land_use_code_id land_use_code_wetland( "wetland" );
static const overmap_land_use_code_id land_use_code_wetland_forest( "wetland_forest" );
static const overmap_land_use_code_id land_use_code_wetland_saltwater( "wetland_saltwater" );

class overmap_land_use_code
{
    public:
        overmap_land_use_code_id id = overmap_land_use_code_id::NULL_ID();

        int land_use_code = 0;
        std::string name;
        std::string detailed_definition;
        uint32_t symbol = 0;
        nc_color color = c_black;

        auto get_symbol() const -> std::string;

        // Used by generic_factory
        bool was_loaded = false;
        void load( const JsonObject &jo, const std::string &src );
        void finalize();
        void check() const;
};

struct overmap_spawns {
        overmap_spawns() : group( mongroup_id::NULL_ID() ) {}

        string_id<MonsterGroup> group;
        numeric_interval<int> population;

        auto operator==( const overmap_spawns &rhs ) const -> bool {
            return group == rhs.group && population == rhs.population;
        }

    protected:
        void deserialize( const JsonObject &jo );
};

struct overmap_static_spawns : public overmap_spawns {
    int chance = 0;

    auto operator==( const overmap_static_spawns &rhs ) const -> bool {
        return overmap_spawns::operator==( rhs ) && chance == rhs.chance;
    }

    void deserialize( const JsonObject &jo );
};

//terrain flags enum! this is for tracking the indices of each flag.
enum oter_flags {
    known_down = 0,
    known_up,
    no_rotate,    // this tile doesn't have four rotated versions (north, east, south, west)
    river_tile,
    has_sidewalk,
    line_drawing, // does this tile have 8 versions, including straights, bends, tees, and a fourway?
    subway_connection,
    lake,
    lake_shore,
    generic_loot,
    risk_high,
    risk_low,
    source_ammo,
    source_animals,
    source_books,
    source_chemistry,
    source_clothing,
    source_construction,
    source_cooking,
    source_drink,
    source_electronics,
    source_fabrication,
    source_farming,
    source_food,
    source_forage,
    source_fuel,
    source_gun,
    source_luxury,
    source_medicine,
    source_people,
    source_safety,
    source_tailoring,
    source_vehicles,
    source_weapon,
    num_oter_flags
};

struct oter_type_t {
    public:
        static const oter_type_t null_type;

    public:
        oter_type_str_id id;
        std::string name;               // Untranslated name
        uint32_t symbol = 0;
        nc_color color = c_black;
        overmap_land_use_code_id land_use_code = overmap_land_use_code_id::NULL_ID();
        std::vector<std::string> looks_like;
        unsigned char see_cost = 0;     // Affects how far the player can see in the overmap
        unsigned char travel_cost = 5;  // Affects the pathfinding and travel times
        std::string extras = "none";
        int mondensity = 0;
        // Spawns are added to the submaps *once* upon mapgen of the submaps
        overmap_static_spawns static_spawns;
        bool was_loaded = false;

        auto get_symbol() const -> std::string;

        oter_type_t() = default;

        auto get_first() const -> oter_id;
        auto get_rotated( om_direction::type dir ) const -> oter_id;
        auto get_linear( size_t n ) const -> oter_id;

        auto has_flag( oter_flags flag ) const -> bool {
            return flags[flag];
        }

        void set_flag( oter_flags flag, bool value = true ) {
            flags[flag] = value;
        }

        void load( const JsonObject &jo, const std::string &src );
        void check() const;
        void finalize();

        auto is_rotatable() const -> bool {
            return !has_flag( no_rotate ) && !has_flag( line_drawing );
        }

        auto is_linear() const -> bool {
            return has_flag( line_drawing );
        }

        auto has_connections() const -> bool {
            return !connect_group.empty();
        }

        auto connects_to( const oter_type_id &other ) const -> bool {
            return has_connections() && connect_group == other->connect_group;
        }

    private:
        std::bitset<num_oter_flags> flags;
        std::vector<oter_id> directional_peers;
        std::string connect_group; // Group for connection when rendering overmap tiles

        void register_terrain( const oter_t &peer, size_t n, size_t max_n );
};

struct oter_t {
    private:
        const oter_type_t *type;

    public:
        oter_str_id id;         // definitive identifier.

        oter_t();
        oter_t( const oter_type_t &type );
        oter_t( const oter_type_t &type, om_direction::type dir );
        oter_t( const oter_type_t &type, size_t line );

        auto get_type_id() const -> const oter_type_str_id & {
            return type->id;
        }

        auto get_mapgen_id() const -> std::string;
        auto get_rotated( om_direction::type dir ) const -> oter_id;

        auto get_name() const -> std::string {
            return _( type->name );
        }

        auto get_symbol( const bool from_land_use_code = false ) const -> std::string {
            return utf32_to_utf8( from_land_use_code ? symbol_alt : symbol );
        }

        auto get_uint32_symbol() const -> uint32_t {
            return symbol;
        }

        auto get_color( const bool from_land_use_code = false ) const -> nc_color {
            return from_land_use_code ? type->land_use_code->color : type->color;
        }

        auto get_dir() const -> om_direction::type {
            return dir;
        }

        auto get_line() const -> size_t {
            return line;
        }
        void get_rotation_and_subtile( int &rotation, int &subtile ) const;

        auto get_see_cost() const -> unsigned char {
            return type->see_cost;
        }
        auto get_travel_cost() const -> unsigned char {
            return type->travel_cost;
        }

        auto get_extras() const -> const std::string & {
            return type->extras;
        }

        auto get_mondensity() const -> int {
            return type->mondensity;
        }

        auto get_static_spawns() const -> const overmap_static_spawns & {
            return type->static_spawns;
        }

        auto get_land_use_code() const -> overmap_land_use_code_id {
            return type->land_use_code;
        }

        auto type_is( const oter_type_id &type_id ) const -> bool;
        auto type_is( const oter_type_t &type ) const -> bool;

        auto has_connection( om_direction::type dir ) const -> bool;

        auto has_flag( oter_flags flag ) const -> bool {
            return type->has_flag( flag );
        }

        auto is_hardcoded() const -> bool;

        auto is_rotatable() const -> bool {
            return type->is_rotatable();
        }

        auto is_linear() const -> bool {
            return type->is_linear();
        }

        auto is_river() const -> bool {
            return type->has_flag( river_tile );
        }

        auto is_wooded() const -> bool {
            return type->land_use_code == land_use_code_forest ||
                   type->land_use_code == land_use_code_wetland ||
                   type->land_use_code == land_use_code_wetland_forest ||
                   type->land_use_code == land_use_code_wetland_saltwater;
        }

        auto is_lake() const -> bool {
            return type->has_flag( lake );
        }

        auto is_lake_shore() const -> bool {
            return type->has_flag( lake_shore );
        }

    private:
        om_direction::type dir = om_direction::type::none;
        uint32_t symbol;
        uint32_t symbol_alt;
        size_t line = 0;         // Index of line. Only valid in case of line drawing.
};

// TODO: Deprecate these operators
auto operator==( const oter_id &lhs, const char *rhs ) -> bool;
auto operator!=( const oter_id &lhs, const char *rhs ) -> bool;

namespace overmap_terrains
{

void load( const JsonObject &jo, const std::string &src );
void check_consistency();
void finalize();
void reset();

auto get_all() -> const std::vector<oter_t> &;

} // namespace overmap_terrains

namespace overmap_land_use_codes
{

void load( const JsonObject &jo, const std::string &src );
void finalize();
void check_consistency();
void reset();

auto get_all() -> const std::vector<overmap_land_use_code> &;

} // namespace overmap_land_use_codes

#endif // CATA_SRC_OMDATA_H
