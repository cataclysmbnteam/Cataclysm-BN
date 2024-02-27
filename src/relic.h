#pragma once
#ifndef CATA_SRC_RELIC_H
#define CATA_SRC_RELIC_H

#include <string>
#include <vector>

#include "json_source_location.h"
#include "magic.h"
#include "magic_enchantment.h"
#include "translations.h"
#include "weighted_list.h"

class Creature;
class JsonIn;
class JsonObject;
class JsonOut;
class relic;
class relic_procgen_data;
struct tripoint;

enum class relic_recharge_type {
    /** Recharges slowly with time */
    time,
    /** Recharges in sunlight */
    solar,
    /** Creates pain to recharge */
    pain,
    /** Drains HP to recharge */
    hp,
    /** Creates fatigue to recharge */
    fatigue,
    /** Consumes field to recharge */
    field,
    /** Consumes trap to recharge */
    trap,

    num
};

enum class relic_recharge_req {
    /** No additional requirements */
    none,
    /** Must be worn/wielded */
    equipped,
    /**
     * Must be worn on closest layer on at least 1 body part if armor, or
     * must be wielding with nothing worn on left and/or right hand.
     */
    close_to_skin,
    /** Will recharge only when character is asleep */
    sleep,
    /** Must be irradiated/in irradiated tile */
    rad,
    /** Must be wet or in rain */
    wet,
    /** Must be a Z-level above the surface */
    sky,

    num
};

template<>
struct enum_traits<relic_recharge_type> {
    static constexpr relic_recharge_type last = relic_recharge_type::num;
};

template<>
struct enum_traits<relic_recharge_req> {
    static constexpr relic_recharge_req last = relic_recharge_req::num;
};

class relic_recharge
{
        json_source_location src_loc;

    public:
        /** Relic recharge type */
        relic_recharge_type type = relic_recharge_type::time;
        /** Relic recharge requirements */
        relic_recharge_req req = relic_recharge_req::none;
        /** If relic consumes fields to recharge, this specifies field type */
        field_type_str_id field_type;
        /** If relic consumes traps to recharge, this specifies trap type */
        trap_str_id trap_type;
        /** For time-based recharge, specifies recharge interval */
        time_duration interval = 1_seconds;
        /** For intensity-based recharge types, specifies min intensity */
        int intensity_min = 0;
        /** For intensity-based recharge types, specifies max intensity */
        int intensity_max = 0;
        /** Specifies amount of charges gained per charge operation.  Can be 0 or even negative. */
        int rate = 0;
        /** Recharge activation message. */
        std::optional<std::string> message;

        bool operator==( const relic_recharge &rhs ) const;

        void load( const JsonObject &jo );
        void serialize( JsonOut &json ) const;
        void deserialize( JsonIn &jsin );
        void check() const;
};

using relic_procgen_id = string_id<relic_procgen_data>;

class relic_procgen_data
{
    public:

        /*
         * various procgen values for passive enchantment values
         * this is a template for the ability to write a little bit
         * less code and easier maintainability for additional values
         */
        template<typename T = int>
        struct enchantment_value_passive {
            enchant_vals::mod type;
            // THIS CANNOT BE 0
            int power_per_increment = 1;
            // whatever increment is used for the point values
            // THIS CANNOT BE 0
            T increment = 1;
            T min_value = 0;
            T max_value = 0;

            int calc_power( T level ) const {
                return std::round( level * static_cast<float>( power_per_increment ) /
                                   static_cast<float>( increment ) );
            }

            bool was_loaded = false;

            void load( const JsonObject &jo );
            void deserialize( JsonIn &jsin );
        };

        struct enchantment_active {
            spell_id activated_spell;
            // power cost of spell at level 0
            int base_power = 0;
            // power cost increment per spell level increment
            int power_per_increment = 1;
            // number of spell levels that give the power per increment at
            int increment = 1;
            // min level of the spell allowed
            int min_level = 0;
            // max level of the spell allowed
            int max_level = 0;

            int calc_power( int level ) const {
                return base_power + std::round( level *
                                                static_cast<float>( power_per_increment ) / static_cast<float>( increment ) );
            }

            bool was_loaded = false;

            void load( const JsonObject &jo );
            void deserialize( JsonIn &jsin );
        };

        struct generation_rules {
            // the desired power level for the generated artifact
            int power_level = 0;
            // the most negative (total) attributes a generated artifact can have
            int max_negative_power = 0;
            // the maximum number of attributes a generated artifact can have
            int max_attributes = INT_MAX;
        };

        enum type {
            passive_enchantment_add,
            passive_enchantment_mult,
            hit_you,
            hit_me,
            active_enchantment,
            last
        };
    private:

        weighted_int_list<enchantment_value_passive<int>> passive_add_procgen_values;
        weighted_int_list<enchantment_value_passive<float>> passive_mult_procgen_values;
        weighted_int_list<enchantment_active> passive_hit_you;
        weighted_int_list<enchantment_active> passive_hit_me;
        weighted_int_list<enchantment_active> active_procgen_values;
        weighted_int_list<type> type_weights;
        weighted_int_list<itype_id> item_weights;

    public:
        relic_procgen_id id;

        int power_level( const enchantment &ench ) const;
        // power level of the active spell
        int power_level( const fake_spell &sp ) const;

        detached_ptr<item> create_item( const relic_procgen_data::generation_rules &rules ) const;
        relic generate( const generation_rules &rules, const itype_id &it_id ) const;

        bool was_loaded;

        static void load_relic_procgen_data( const JsonObject &jo, const std::string &src );
        void load( const JsonObject &jo, const std::string & = "" );
        void deserialize( JsonIn &jsin );
};

class relic
{
    private:
        std::vector<fake_spell> active_effects;
        std::vector<enchantment> passive_effects;
        std::vector<relic_recharge> recharge_scheme;

        // the item's name will be replaced with this if the string is not empty
        translation item_name_override;

        int charges_per_activation;
        // activating an artifact overrides all spell casting costs
        int moves;
    public:
        bool operator==( const relic &rhs ) const;

        std::string name() const;
        // returns number of charges that should be consumed
        int activate( Creature &caster, const tripoint &target ) const;

        void load( const JsonObject &jo );

        void serialize( JsonOut &jsout ) const;
        void deserialize( JsonIn &jsin );

        void add_passive_effect( const enchantment &ench );
        void add_active_effect( const fake_spell &sp );
        void add_recharge_scheme( const relic_recharge &r );

        inline const std::vector<enchantment> &get_enchantments() const {
            return passive_effects;
        }
        inline const std::vector<relic_recharge> &get_recharge_scheme() const {
            return recharge_scheme;
        }

        // what is the power level of this artifact, given a specific ruleset
        int power_level( const relic_procgen_id &ruleset ) const;

        void check() const;
};

namespace relic_funcs
{

bool check_recharge_reqs( const item &itm, const relic_recharge &rech, const Character &carrier );
bool process_recharge_entry( item &itm, const relic_recharge &rech, Character &carrier );

void process_recharge( item &itm, Character &carrier );

} // namespace relic_funcs

template <typename E> struct enum_traits;

template<>
struct enum_traits<relic_procgen_data::type> {
    static constexpr relic_procgen_data::type last = relic_procgen_data::type::last;
};

#endif // CATA_SRC_RELIC_H
