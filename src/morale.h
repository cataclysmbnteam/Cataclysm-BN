#pragma once
#ifndef CATA_SRC_MORALE_H
#define CATA_SRC_MORALE_H

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "bodypart.h"
#include "calendar.h"
#include "morale_types.h"
#include "type_id.h"

class JsonIn;
class JsonObject;
class JsonOut;
class item;
class effect;
struct itype;
struct morale_mult;
template<typename T> struct enum_traits;

enum class morale_subtype_t : char {
    single = 0,
    by_item,
    by_effect,
    last
};

template<>
struct enum_traits<morale_subtype_t> {
    static constexpr morale_subtype_t last = morale_subtype_t::last;
};

class player_morale
{
    public:
        player_morale();

        player_morale( player_morale && ) = default;
        player_morale( const player_morale & ) = default;
        auto operator =( player_morale && ) -> player_morale & = default;
        auto operator =( const player_morale & ) -> player_morale & = default;

        /** Adds morale to existing or creates one */
        void add( morale_type type, int bonus, int max_bonus = 0,
                  const time_duration &duration = 6_minutes, const time_duration &decay_start = 3_minutes,
                  bool capped = false );
        void add( morale_type type, int bonus, int max_bonus,
                  const time_duration &duration, const time_duration &decay_start,
                  bool capped,
                  const itype &item_type );
        void add( morale_type type, int bonus, int max_bonus,
                  const time_duration &duration, const time_duration &decay_start,
                  bool capped,
                  const efftype_id &effect_type );
        /** Sets the new level for the permanent morale, or creates one */
        void set_permanent( const morale_type &type, int bonus );
        /** Returns true if any morale point with specified morale exists */
        auto has( const morale_type &type ) const -> bool;
        /** Returns bonus from specified morale */
        auto get( const morale_type &type ) const -> int;
        /** Removes specified morale */
        void remove( const morale_type &type );
        /** Clears up all morale points */
        void clear();
        /** Returns overall morale level */
        auto get_level() const -> int;
        /** Ticks down morale counters and removes them */
        void decay( const time_duration &ticks = 1_turns );
        /** Displays morale screen */
        void display( int focus_eq, int pain_penalty, int fatigue_cap );
        /** Returns false whether morale is inconsistent with the argument.
         *  Only permanent morale is checked */
        auto consistent_with( const player_morale &morale ) const -> bool;

        /**calculates the percentage contribution for each morale point*/
        void calculate_percentage();

        auto get_total_positive_value() const -> int;
        auto get_total_negative_value() const -> int;

        void on_mutation_gain( const trait_id &mid );
        void on_mutation_loss( const trait_id &mid );
        void on_stat_change( const std::string &stat, int value );
        void on_item_wear( const item &it );
        void on_item_takeoff( const item &it );
        void on_worn_item_transform( const item &old_it, const item &new_it );
        void on_worn_item_washed( const item &it );
        void on_effect_int_change( const efftype_id &eid, int intensity,
                                   const bodypart_str_id &bp = bodypart_str_id::NULL_ID() );

        void store( JsonOut &jsout ) const;
        void load( const JsonObject &jsin );

    private:

        // TODO: It would be cleaner if it was saved/loaded similar to @ref poly_serialized
        class morale_subtype
        {
            public:
                morale_subtype();
                morale_subtype( const itype &item_type )
                    : subtype_type( morale_subtype_t::by_item )
                    , item_type( &item_type ) {};
                morale_subtype( const efftype_id &eff_type )
                    : subtype_type( morale_subtype_t::by_effect )
                    , eff_type( eff_type ) {};

                // TODO: (optional) use optional
                auto has_description() const -> bool;
                auto describe() const -> std::string;

                auto matches( const morale_subtype &other ) const -> bool {
                    return *this == other;
                }
                auto operator==( const morale_subtype &other ) const -> bool {
                    if( subtype_type != other.subtype_type ) {
                        return false;
                    }
                    switch( subtype_type ) {
                        case morale_subtype_t::single:
                            return true;
                        case morale_subtype_t::by_item:
                            return item_type == other.item_type;
                        case morale_subtype_t::by_effect:
                            return eff_type == other.eff_type;
                        default:
                            // Error!
                            return false;
                    }
                }

                void deserialize( JsonIn &jsin );
                void serialize( JsonOut &json ) const;

            private:
                morale_subtype_t subtype_type = morale_subtype_t::single;
                const itype *item_type = nullptr;
                efftype_id eff_type;
        };

        class morale_point
        {
            public:
                morale_point(
                    const morale_type &type = MORALE_NULL,
                    morale_subtype subtype = morale_subtype(),
                    int bonus = 0,
                    int max_bonus = 0,
                    time_duration duration = 6_minutes,
                    time_duration decay_start = 3_minutes,
                    bool capped = false ) :

                    type( type ),
                    subtype( subtype ),
                    bonus( normalize_bonus( bonus, max_bonus, capped ) ),
                    duration( std::max( duration, 0_turns ) ),
                    decay_start( std::max( decay_start, 0_turns ) ),
                    age( 0_turns ) {}

                void deserialize( JsonIn &jsin );
                void serialize( JsonOut &json ) const;

                auto get_name() const -> std::string;
                auto get_net_bonus() const -> int;
                auto get_net_bonus( const morale_mult &mult ) const -> int;
                auto is_expired() const -> bool;
                auto is_permanent() const -> bool;
                auto type_matches( const morale_type &_type ) const -> bool;
                auto matches( const morale_type &_type ) const -> bool;
                auto matches( const morale_type &_type, const morale_subtype &_subtype ) const -> bool;
                auto matches( const morale_point &mp ) const -> bool;

                void add( int new_bonus, int new_max_bonus, time_duration new_duration,
                          time_duration new_decay_start, bool new_cap );
                void decay( const time_duration &ticks = 1_turns );
                /*
                 *contribution should be bettween [0,100] (inclusive)
                 */
                void set_percent_contribution( double contribution );
                auto get_percent_contribution() const -> double;
            private:
                morale_type type;
                morale_subtype subtype;

                int bonus = 0;
                time_duration duration = 0_turns;   // Zero duration == infinity
                time_duration decay_start = 0_turns;
                time_duration age = 0_turns;
                /**
                 *this point's percent contribution to the total positive or total negative morale effect
                 */
                double percent_contribution = 0;

                /**
                 * Returns either new_time or remaining time (which one is greater).
                 * Only returns new time if same_sign is true
                 */
                auto pick_time( const time_duration &current_time, const time_duration &new_time,
                                         bool same_sign ) const -> time_duration;
                /**
                 * Returns normalized bonus if either max_bonus != 0 or capped == true
                 */
                auto normalize_bonus( int bonus, int max_bonus, bool capped ) const -> int;
        };
    private:
        void add( morale_type type, const morale_subtype &subtype, int bonus, int max_bonus,
                  const time_duration &duration, const time_duration &decay_start,
                  bool capped );
        auto has( const morale_type &type, const morale_subtype &subtype ) const -> int;
        void remove( const morale_type &type, const morale_subtype &subtype );
        void set_permanent_typed( const morale_type &type, int bonus, const morale_subtype &subtype );

        auto get_temper_mult() const -> morale_mult;

        void set_prozac( bool new_took_prozac );
        void set_prozac_bad( bool new_took_prozac_bad );
        void set_stylish( bool new_stylish );
        void set_worn( const item &it, bool worn );
        void set_mutation( const trait_id &mid, bool active );
        auto has_mutation( const trait_id &mid ) -> bool;

        void remove_if( const std::function<bool( const morale_point & )> &func );
        void remove_expired();
        void invalidate();

        void update_stylish_bonus();
        void update_squeamish_penalty();
        void update_masochist_bonus();
        void update_bodytemp_penalty( const time_duration &ticks );
        void update_constrained_penalty();

    private:
        std::vector<morale_point> points;

        struct body_part_data {
            unsigned int covered;
            unsigned int fancy;
            unsigned int filthy;
            int hot;
            int cold;

            body_part_data() :
                covered( 0 ),
                fancy( 0 ),
                filthy( 0 ),
                hot( 0 ),
                cold( 0 ) {}
        };
        std::map<bodypart_id, body_part_data> body_parts;
        body_part_data no_body_part;

        using mutation_handler = std::function<void ( player_morale & )>;
        struct mutation_data {
            public:
                mutation_data() = default;
                mutation_data( mutation_handler on_gain_and_loss ) :
                    on_gain( on_gain_and_loss ),
                    on_loss( on_gain_and_loss ),
                    active( false ) {}
                mutation_data( mutation_handler on_gain, mutation_handler on_loss ) :
                    on_gain( on_gain ),
                    on_loss( on_loss ),
                    active( false ) {}
                void set_active( player_morale &sender, bool new_active );
                auto get_active() const -> bool;
                void clear();
            private:
                mutation_handler on_gain;
                mutation_handler on_loss;
                bool active;
        };
        std::map<trait_id, mutation_data> mutations;

        std::map<itype_id, int> super_fancy_items;

        // Mutability is required for lazy initialization
        mutable int level;
        mutable bool level_is_valid;

        bool took_prozac;
        bool took_prozac_bad;
        bool stylish;
        int perceived_pain;
};

#endif // CATA_SRC_MORALE_H
