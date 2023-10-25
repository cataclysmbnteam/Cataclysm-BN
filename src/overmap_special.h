#pragma once
#ifndef CATA_SRC_OVERMAP_SPECIAL_H
#define CATA_SRC_OVERMAP_SPECIAL_H

#include <climits>
#include <cstddef>
#include <cstdint>
#include <bitset>
#include <list>
#include <set>
#include <vector>
#include <array>
#include <string>

#include "omdata.h"
#include "point.h"
#include "type_id.h"

struct city;

class JsonObject;
class overmap_special_batch;
class overmap_special;

// LINE_**** corresponds to the ACS_**** macros in ncurses, and are patterned
// the same way; LINE_NESW, where X indicates a line and O indicates no line
// (thus, LINE_OXXX looks like 'T'). LINE_ is defined in output.h.  The ACS_
// macros can't be used here, since ncurses hasn't been initialized yet.

// Overmap specials--these are "special encounters," dungeons, nests, etc.
// This specifies how often and where they may be placed.

struct overmap_special_spawns : public overmap_spawns {
    numeric_interval<int> radius;

    bool operator==( const overmap_special_spawns &rhs ) const {
        return overmap_spawns::operator==( rhs ) && radius == rhs.radius;
    }

    void deserialize( const JsonObject &jo );
};

struct overmap_special_terrain {
    overmap_special_terrain() = default;
    tripoint p;
    oter_str_id terrain;
    std::set<overmap_location_id> locations;

    void deserialize( const JsonObject &jo );

    /**
     * Returns whether this terrain of the special can be placed on the specified terrain.
     * It's true if oter meets any of locations.
     */
    bool can_be_placed_on( const oter_id &oter ) const;
};

struct overmap_special_connection {
    tripoint p;
    std::optional<tripoint> from;
    om_direction::type initial_dir = om_direction::type::invalid;
    overmap_connection_id connection;
    bool existing = false;

    void deserialize( const JsonObject &jo );
};

class overmap_special
{
    public:
        /** Returns terrain at the given point. */
        const overmap_special_terrain &get_terrain_at( const tripoint &p ) const;
        /** @returns true if this special requires a city */
        bool requires_city() const;
        /** @returns whether the special at specified tripoint can belong to the specified city. */
        bool can_belong_to_city( const tripoint_om_omt &p, const city &cit ) const;

        overmap_special_id id;
        std::list<overmap_special_terrain> terrains;
        std::vector<overmap_special_connection> connections;

        numeric_interval<int> city_size{ 0, INT_MAX };
        numeric_interval<int> city_distance{ 0, INT_MAX };
        numeric_interval<int> occurrences;

        bool rotatable = true;
        overmap_special_spawns spawns;
        std::set<std::string> flags;

        // Used by generic_factory
        bool was_loaded = false;
        void load( const JsonObject &jo, const std::string &src );
        void finalize();
        void check() const;
    private:
        // These locations are the default values if ones are not specified for the individual OMTs.
        std::set<overmap_location_id> default_locations;
};

namespace overmap_specials
{

void load( const JsonObject &jo, const std::string &src );
void finalize();
void check_consistency();
void reset();

const std::vector<overmap_special> &get_all();

overmap_special_batch get_default_batch( const point_abs_om &origin );
/**
 * Generates a simple special from a building id.
 */
overmap_special_id create_building_from( const oter_type_str_id &base );

} // namespace overmap_specials

namespace city_buildings
{

void load( const JsonObject &jo, const std::string &src );

} // namespace city_buildings

// Wrapper around an overmap special to track progress of placing specials.
struct overmap_special_placement {
    int instances_placed;
    const overmap_special *special_details;
};

// A batch of overmap specials to place.
class overmap_special_batch
{
    public:
        overmap_special_batch( const point_abs_om &origin ) : origin_overmap( origin ) {}
        overmap_special_batch( const point_abs_om &origin,
                               const std::vector<const overmap_special *> &specials ) :
            origin_overmap( origin ) {
            std::transform( specials.begin(), specials.end(),
            std::back_inserter( placements ), []( const overmap_special * elem ) {
                return overmap_special_placement{ 0, elem };
            } );
        }

        // Wrapper methods that make overmap_special_batch act like
        // the underlying vector of overmap placements.
        std::vector<overmap_special_placement>::iterator begin() {
            return placements.begin();
        }
        std::vector<overmap_special_placement>::iterator end() {
            return placements.end();
        }
        std::vector<overmap_special_placement>::iterator erase(
            std::vector<overmap_special_placement>::iterator pos ) {
            return placements.erase( pos );
        }
        bool empty() {
            return placements.empty();
        }

        point_abs_om get_origin() const {
            return origin_overmap;
        }

    private:
        std::vector<overmap_special_placement> placements;
        point_abs_om origin_overmap;
};

#endif // CATA_SRC_OVERMAP_SPECIAL_H
