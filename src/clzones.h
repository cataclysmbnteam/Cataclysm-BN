#pragma once
#ifndef CATA_SRC_CLZONES_H
#define CATA_SRC_CLZONES_H

#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "memory_fast.h"
#include "optional.h"
#include "point.h"
#include "string_id.h"
#include "type_id.h"

class JsonIn;
class JsonObject;
class JsonOut;
class faction;
class item;
class map;
struct construction;

using faction_id = string_id<faction>;
static const faction_id your_fac( "your_followers" );
const std::string type_fac_hash_str = "__FAC__";

class zone_type
{
    private:
        std::string name_;
        std::string desc_;
    public:

        zone_type_id id;
        bool was_loaded = false;

        zone_type() = default;
        explicit zone_type( const std::string &name, const std::string &desc ) : name_( name ),
            desc_( desc ) {}

        auto name() const -> std::string;
        auto desc() const -> std::string;

        static void reset_zones();
        static void load_zones( const JsonObject &jo, const std::string &src );
        void load( const JsonObject &jo, const std::string & );
        /**
         * All spells in the game.
         */
        static auto get_all() -> const std::vector<zone_type> &;
        auto is_valid() const -> bool;
};

class zone_options
{
    public:
        virtual ~zone_options() = default;

        /* create valid instance for zone type */
        static auto create( const zone_type_id &type ) -> shared_ptr_fast<zone_options>;

        /* checks if options is correct base / derived class for zone type */
        static auto is_valid( const zone_type_id &type, const zone_options &options ) -> bool;

        /* derived classes must always return true */
        virtual auto has_options() const -> bool {
            return false;
        }

        /* query only necessary options at zone creation, one by one
         * returns true if successful, returns false if fails or canceled */
        virtual auto query_at_creation() -> bool {
            return true;
        }

        /* query options, first uilist should allow to pick an option to edit (if more than one)
         * returns true if something is changed, otherwise returns false */
        virtual auto query() -> bool {
            return false;
        }

        /* suggest a name for the zone, depending on options */
        virtual auto get_zone_name_suggestion() const -> std::string {
            return "";
        }

        /* vector of pairs of each option's description and value */
        virtual auto get_descriptions() const -> std::vector<std::pair<std::string, std::string>> {
            return std::vector<std::pair<std::string, std::string>>();
        }

        virtual void serialize( JsonOut & ) const {}
        virtual void deserialize( const JsonObject & ) {}
};

// mark option interface
class mark_option
{
    public:
        virtual ~mark_option() = default;

        virtual auto get_mark() const -> std::string = 0;
};

class plot_options : public zone_options, public mark_option
{
    private:
        itype_id mark;
        itype_id seed;

        enum query_seed_result {
            canceled,
            successful,
            changed,
        };

        auto query_seed() -> query_seed_result;

    public:
        auto get_mark() const -> std::string override {
            return mark.str();
        }
        auto get_seed() const -> itype_id {
            return seed;
        }

        auto has_options() const -> bool override {
            return true;
        }

        auto query_at_creation() -> bool override;
        auto query() -> bool override;

        auto get_zone_name_suggestion() const -> std::string override;

        auto get_descriptions() const -> std::vector<std::pair<std::string, std::string>> override;

        void serialize( JsonOut &json ) const override;
        void deserialize( const JsonObject &jo_zone ) override;
};

class blueprint_options : public zone_options, public mark_option
{
    private:
        // furn/ter id as string.
        std::string mark;
        construction_group_str_id group = construction_group_str_id::NULL_ID();
        construction_id index;

        enum query_con_result {
            canceled,
            successful,
            changed,
        };

        auto query_con() -> query_con_result;

    public:
        auto get_mark() const -> std::string override {
            return mark;
        }
        auto get_index() const -> construction_id {
            return index;
        }

        auto has_options() const -> bool override {
            return true;
        }

        auto get_final_construction(
            const std::vector<construction_id> &list_constructions,
            const construction_id &id,
            std::set<construction_id> &skip_index ) -> construction_id;

        auto query_at_creation() -> bool override;
        auto query() -> bool override;

        auto get_zone_name_suggestion() const -> std::string override;

        auto get_descriptions() const -> std::vector<std::pair<std::string, std::string>> override;

        void serialize( JsonOut &json ) const override;
        void deserialize( const JsonObject &jo_zone ) override;
};

class loot_options : public zone_options, public mark_option
{
    private:
        // basic item filter.
        std::string mark;

        enum query_loot_result {
            canceled,
            successful,
            changed,
        };

        auto query_loot() -> query_loot_result;

    public:
        auto get_mark() const -> std::string override {
            return mark;
        }

        auto has_options() const -> bool override {
            return true;
        }

        auto query_at_creation() -> bool override;
        auto query() -> bool override;

        auto get_zone_name_suggestion() const -> std::string override;

        auto get_descriptions() const -> std::vector<std::pair<std::string, std::string>> override;

        void serialize( JsonOut &json ) const override;
        void deserialize( const JsonObject &jo_zone ) override;
};

/**
 * These are zones the player can designate.
 */
class zone_data
{
    private:
        std::string name;
        zone_type_id type;
        faction_id faction;
        bool invert;
        bool enabled;
        bool is_vehicle;
        tripoint start;
        tripoint end;
        shared_ptr_fast<zone_options> options;

    public:
        zone_data() {
            type = zone_type_id( "" );
            invert = false;
            enabled = false;
            is_vehicle = false;
            start = tripoint_zero;
            end = tripoint_zero;
            options = nullptr;
        }

        zone_data( const std::string &_name, const zone_type_id &_type, const faction_id &_faction,
                   bool _invert, const bool _enabled,
                   const tripoint &_start, const tripoint &_end,
                   shared_ptr_fast<zone_options> _options = nullptr ) {
            name = _name;
            type = _type;
            faction = _faction;
            invert = _invert;
            enabled = _enabled;
            is_vehicle = false;
            start = _start;
            end = _end;

            // ensure that suplied options is of correct class
            if( _options == nullptr || !zone_options::is_valid( type, *_options ) ) {
                options = zone_options::create( type );
            } else {
                options = _options;
            }
        }

        // returns true if name is changed
        auto set_name() -> bool;
        // returns true if type is changed
        auto set_type() -> bool;
        void set_position( const std::pair<tripoint, tripoint> &position, bool manual = true );
        void set_enabled( bool enabled_arg );
        void set_is_vehicle( bool is_vehicle_arg );

        static auto make_type_hash( const zone_type_id &_type, const faction_id &_fac ) -> std::string {
            return _type.c_str() + type_fac_hash_str + _fac.c_str();
        }
        static auto unhash_type( const std::string &hash_type ) -> zone_type_id {
            size_t end = hash_type.find( type_fac_hash_str );
            if( end != std::string::npos && end < hash_type.size() ) {
                return zone_type_id( hash_type.substr( 0, end ) );
            }
            return zone_type_id( "" );
        }
        auto get_name() const -> std::string {
            return name;
        }
        auto get_faction() const -> const faction_id & {
            return faction;
        }
        auto get_type_hash() const -> std::string {
            return make_type_hash( type, faction );
        }
        auto get_type() const -> const zone_type_id & {
            return type;
        }
        auto get_invert() const -> bool {
            return invert;
        }
        auto get_enabled() const -> bool {
            return enabled;
        }
        auto get_is_vehicle() const -> bool {
            return is_vehicle;
        }
        auto get_start_point() const -> tripoint {
            return start;
        }
        auto get_end_point() const -> tripoint {
            return end;
        }
        auto get_center_point() const -> tripoint;
        auto has_options() const -> bool {
            return options->has_options();
        }
        auto get_options() const -> const zone_options & {
            return *options;
        }
        auto get_options() -> zone_options & {
            return *options;
        }
        auto has_inside( const tripoint &p ) const -> bool {
            return p.x >= start.x && p.x <= end.x &&
                   p.y >= start.y && p.y <= end.y &&
                   p.z >= start.z && p.z <= end.z;
        }
        void serialize( JsonOut &json ) const;
        void deserialize( JsonIn &jsin );
};

class zone_manager
{
    public:
        using ref_zone_data = std::reference_wrapper<zone_data>;
        using ref_const_zone_data = std::reference_wrapper<const zone_data>;

    private:
        static const int MAX_DISTANCE = 10;
        std::vector<zone_data> zones;
        //Containers for Revert functionality for Vehicle Zones
        //Pointer to added zone to be removed
        std::vector<zone_data *> added_vzones;
        //Copy of original data, pointer to the zone
        std::vector<std::pair<zone_data, zone_data *>> changed_vzones;
        //copy of original data to be re-added
        std::vector<zone_data> removed_vzones;

        std::map<zone_type_id, zone_type> types;
        std::unordered_map<std::string, std::unordered_set<tripoint>> area_cache;
        std::unordered_map<std::string, std::unordered_set<tripoint>> vzone_cache;
        auto get_point_set( const zone_type_id &type,
                const faction_id &fac = your_fac ) const -> std::unordered_set<tripoint>;
        auto get_vzone_set( const zone_type_id &type,
                const faction_id &fac = your_fac ) const -> std::unordered_set<tripoint>;

        //Cache number of items already checked on each source tile when sorting
        std::unordered_map<tripoint, int> num_processed;

        zone_manager();
        ~zone_manager() = default;
        zone_manager( zone_manager && ) = default;
        zone_manager( const zone_manager & ) = default;
        auto operator=( zone_manager && ) -> zone_manager & = default;
        auto operator=( const zone_manager & ) -> zone_manager & = default;

    public:
        static auto get_manager() -> zone_manager &;
        static void reset_manager();

        void add( const std::string &name, const zone_type_id &type, const faction_id &faction,
                  bool invert, bool enabled,
                  const tripoint &start, const tripoint &end,
                  shared_ptr_fast<zone_options> options = nullptr );
        auto get_zone_at( const tripoint &where, const zone_type_id &type ) const -> const zone_data *;
        void create_vehicle_loot_zone( class vehicle &vehicle, const point &mount_point,
                                       zone_data &new_zone );

        auto remove( zone_data &zone ) -> bool;

        auto size() const -> unsigned int {
            return zones.size();
        }
        auto get_types() const -> const std::map<zone_type_id, zone_type> & {
            return types;
        }
        auto get_name_from_type( const zone_type_id &type ) const -> std::string;
        auto has_type( const zone_type_id &type ) const -> bool;
        auto has_defined( const zone_type_id &type, const faction_id &fac = your_fac ) const -> bool;
        void cache_data();
        void cache_vzones();
        auto has( const zone_type_id &type, const tripoint &where,
                  const faction_id &fac = your_fac ) const -> bool;
        auto has_near( const zone_type_id &type, const tripoint &where, int range = MAX_DISTANCE,
                       const faction_id &fac = your_fac ) const -> bool;
        auto has_loot_dest_near( const tripoint &where ) const -> bool;
        auto custom_loot_has( const tripoint &where, const item *it ) const -> bool;
        auto get_near( const zone_type_id &type, const tripoint &where,
                                               int range = MAX_DISTANCE, const item *it = nullptr, const faction_id &fac = your_fac ) const -> std::unordered_set<tripoint>;
        auto get_nearest( const zone_type_id &type, const tripoint &where,
                                              int range = MAX_DISTANCE, const faction_id &fac = your_fac ) const -> cata::optional<tripoint>;
        auto get_near_zone_type_for_item( const item &it, const tripoint &where,
                int range = MAX_DISTANCE ) const -> zone_type_id;
        auto get_zones( const zone_type_id &type, const tripoint &where,
                                          const faction_id &fac = your_fac ) const -> std::vector<zone_data>;
        auto get_zone_at( const tripoint &where ) const -> const zone_data *;
        auto get_bottom_zone( const tripoint &where,
                                          const faction_id &fac = your_fac ) const -> const zone_data *;
        auto query_name( const std::string &default_name = "" ) const -> cata::optional<std::string>;
        auto query_type() const -> cata::optional<zone_type_id>;
        void swap( zone_data &a, zone_data &b );
        void rotate_zones( map &target_map, int turns );
        // list of tripoints of zones that are loot zones only
        auto get_point_set_loot( const tripoint &where, int radius,
                const faction_id &fac = your_fac ) const -> std::unordered_set<tripoint>;
        auto get_point_set_loot( const tripoint &where, int radius,
                bool npc_search, const faction_id &fac = your_fac ) const -> std::unordered_set<tripoint>;

        // 'direct' access to zone_manager::zones, giving direct access was nono
        auto get_zones( const faction_id &fac = your_fac ) -> std::vector<ref_zone_data>;
        auto get_zones( const faction_id &fac = your_fac ) const -> std::vector<ref_const_zone_data>;

        auto save_zones() -> bool;
        void load_zones();
        void zone_edited( zone_data &zone );
        void revert_vzones();
        void serialize( JsonOut &json ) const;
        void deserialize( JsonIn &jsin );
};

#endif // CATA_SRC_CLZONES_H
