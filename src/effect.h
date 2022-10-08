#pragma once
#ifndef CATA_SRC_EFFECT_H
#define CATA_SRC_EFFECT_H

#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "bodypart.h"
#include "calendar.h"
#include "hash_utils.h"
#include "translations.h"
#include "type_id.h"

class player;

enum game_message_type : int;
class JsonIn;
class JsonObject;
class JsonOut;

/** Handles the large variety of weed messages. */
void weed_msg( player &p );

enum effect_rating {
    e_good,     // The effect is good for the one who has it.
    e_neutral,  // There is no effect or the effect is very nominal. This is the default.
    e_bad,      // The effect is bad for the one who has it.
    e_mixed     // The effect has good and bad parts to the one who has it.
};

class effect_type
{
        friend void load_effect_type( const JsonObject &jo );
        friend class effect;
    public:
        effect_type() = default;

        efftype_id id;

        /** Returns if an effect is good or bad for message display. */
        auto get_rating() const -> effect_rating;

        /** Returns true if there is a listed name in the JSON entry for each intensity from
         *  1 to max_intensity. */
        auto use_name_ints() const -> bool;
        /** Returns true if there is a listed description in the JSON entry for each intensity
         *  from 1 to max_intensity with the matching reduced value. */
        auto use_desc_ints( bool reduced ) const -> bool;

        /** Returns the appropriate game_message_type when a new effect is obtained. This is equal to
         *  an effect's "rating" value. */
        auto gain_game_message_type() const -> game_message_type;
        /** Returns the appropriate game_message_type when an effect is lost. This is opposite to
         *  an effect's "rating" value. */
        auto lose_game_message_type() const -> game_message_type;

        /** Returns the message displayed when a new effect is obtained. */
        auto get_apply_message() const -> std::string;
        /** Returns the memorial log added when a new effect is obtained. */
        auto get_apply_memorial_log() const -> std::string;
        /** Returns the message displayed when an effect is removed. */
        auto get_remove_message() const -> std::string;
        /** Returns the memorial log added when an effect is removed. */
        auto get_remove_memorial_log() const -> std::string;
        /** Returns the effect's description displayed when character conducts blood analysis. */
        auto get_blood_analysis_description() const -> std::string;

        /** Returns true if an effect will only target main body parts (i.e., those with HP). */
        auto get_main_parts() const -> bool;
        /** Returns the maximum duration of an effect. */
        auto get_max_duration() const -> time_duration;
        /** Returns the number of turns it takes for the intensity to fall by 1 or 0 if intensity isn't based on duration. */
        auto get_int_dur_factor() const -> time_duration;

        /** Returns the id of morale type this effect produces. */
        auto get_morale_type() const -> morale_type;

        auto is_show_in_info() const -> bool;

        /** Returns true if an effect is permanent, i.e. it's duration does not decrease over time. */
        auto is_permanent() const -> bool;

        /** Loading helper functions */
        auto load_mod_data( const JsonObject &jo, const std::string &member ) -> bool;
        auto load_miss_msgs( const JsonObject &jo, const std::string &member ) -> bool;
        auto load_decay_msgs( const JsonObject &jo, const std::string &member ) -> bool;

        /** Registers the effect in the global map */
        static void register_ma_buff_effect( const effect_type &eff );

        static void check_consistency();

    private:
        bool permanent = false;

    protected:
        int max_intensity = 0;
        int max_effective_intensity = 0;
        time_duration max_duration = 0_turns;

        int dur_add_perc = 0;
        int int_add_val = 0;

        int int_decay_step = 0;
        int int_decay_tick = 0 ;
        time_duration int_dur_factor = 0_turns;

        std::set<std::string> flags;

        bool main_parts_only = false;

        // Determines if effect should be shown in description.
        bool show_in_info = false;

        std::vector<trait_id> resist_traits;
        std::vector<efftype_id> resist_effects;
        std::vector<efftype_id> removes_effects;
        std::vector<efftype_id> blocks_effects;

        std::vector<std::pair<std::string, int>> miss_msgs;

        bool pain_sizing = false;
        bool hurt_sizing = false;
        bool harmful_cough = false;
        // TODO: Once addictions are JSON-ized it should be trivial to convert this to a
        // "generic" addiction reduces value
        bool pkill_addict_reduces = false;
        // This flag is hard-coded for specific IDs now
        // It needs to be set for monster::move_effects
        bool impairs_movement = false;

        std::vector<translation> name;
        std::string speed_mod_name;
        std::vector<std::string> desc;
        std::vector<std::string> reduced_desc;
        bool part_descs = false;

        std::vector<std::pair<std::string, game_message_type>> decay_msgs;

        effect_rating rating = effect_rating::e_neutral;

        std::string apply_message;
        std::string apply_memorial_log;
        std::string remove_message;
        std::string remove_memorial_log;

        std::string blood_analysis_description;

        morale_type morale;

        /** Key tuple order is:("base_mods"/"scaling_mods", reduced: bool, type of mod: "STR", desired argument: "tick") */
        std::unordered_map <
        std::tuple<std::string, bool, std::string, std::string>, double, cata::tuple_hash > mod_data;
};

class effect
{
    public:
        effect() : eff_type( nullptr ), duration( 0_turns ), bp( num_bp ),
            intensity( 1 ), start_time( calendar::turn_zero ),
            removed( true ) {
        }
        effect( const effect_type *peff_type, const time_duration &dur,
                const bodypart_str_id &part, int nintensity, const time_point &nstart_time ) :
            eff_type( peff_type ), duration( dur ), bp( part->token ),
            intensity( nintensity ), start_time( nstart_time ),
            removed( false ) {
        }
        effect( const effect & ) = default;
        auto operator=( const effect & ) -> effect & = default;

        /** Returns true if the effect is the result of `effect()`, ie. an effect that doesn't exist. */
        auto is_null() const -> bool;
        operator bool() const {
            return !is_null() && !is_removed();
        }

        /** Dummy used for "reference to effect()" */
        static effect null_effect;

        /** Returns the name displayed in the player status window. */
        auto disp_name() const -> std::string;
        /** Returns the description displayed in the player status window. */
        auto disp_desc( bool reduced = false ) const -> std::string;
        /** Returns the short description as set in json. */
        auto disp_short_desc( bool reduced = false ) const -> std::string;
        /** Returns true if a description will be formatted as "Your" + body_part + description. */
        auto use_part_descs() const -> bool;

        /** Returns the effect's matching effect_type. */
        auto get_effect_type() const -> const effect_type *;

        /** Decays effect durations, returning true if duration <= 0.
         *  This is called in the middle of a loop through all effects, which is
         *  why we aren't allowed to remove the effects here. */
        auto decay( const time_point &time, bool player ) -> bool;

        /** Returns the remaining duration of an effect. */
        auto get_duration() const -> time_duration;
        /** Returns the maximum duration of an effect. */
        auto get_max_duration() const -> time_duration;
        /** Sets the duration, capping at max_duration if it exists. */
        void set_duration( const time_duration &dur, bool alert = false );
        /** Mods the duration, capping at max_duration if it exists. */
        void mod_duration( const time_duration &dur, bool alert = false );
        /** Multiplies the duration, capping at max_duration if it exists. */
        void mult_duration( double dur, bool alert = false );

        /** Returns the turn the effect was applied. */
        auto get_start_time() const -> time_point;

        /** Returns the targeted body_part of the effect. This is NULL_ID for untargeted effects. */
        auto get_bp() const -> const bodypart_str_id &;

        /** Returns true if an effect is permanent, i.e. it's duration does not decrease over time. */
        auto is_permanent() const -> bool;

        /** Returns the intensity of an effect. */
        auto get_intensity() const -> int;
        /** Returns the maximum intensity of an effect. */
        auto get_max_intensity() const -> int;

        /**
         * Sets intensity of effect capped by range [1..max_intensity]
         * @param val Value to set intensity to
         * @param alert whether decay messages should be displayed
         * @return new intensity of the effect after val subjected to above cap
         */
        auto set_intensity( int val, bool alert = false ) -> int;

        /**
         * Modify intensity of effect capped by range [1..max_intensity]
         * @param mod Amount to increase current intensity by
         * @param alert whether decay messages should be displayed
         * @return new intensity of the effect after modification and capping
         */
        auto mod_intensity( int mod, bool alert = false ) -> int;

        /**
         * Returns if the effect is disabled and set up for removal.
         */
        auto is_removed() const -> bool {
            return removed;
        }
        void set_removed() {
            removed = true;
        }

        /** Returns the string id of the resist trait to be used in has_trait("id"). */
        auto get_resist_traits() const -> const std::vector<trait_id> &;
        /** Returns the string id of the resist effect to be used in has_effect("id"). */
        auto get_resist_effects() const -> const std::vector<efftype_id> &;
        /** Returns the string ids of the effects removed by this effect to be used in remove_effect("id"). */
        auto get_removes_effects() const -> const std::vector<efftype_id> &;
        /** Returns the string ids of the effects blocked by this effect to be used in add_effect("id"). */
        auto get_blocks_effects() const -> std::vector<efftype_id>;

        /** Returns the matching modifier type from an effect, used for getting actual effect effects. */
        auto get_mod( std::string arg, bool reduced = false ) const -> int;
        /** Returns the average return of get_mod for a modifier type. Used in effect description displays. */
        auto get_avg_mod( std::string arg, bool reduced = false ) const -> int;
        /** Returns the amount of a modifier type applied when a new effect is first added. */
        auto get_amount( std::string arg, bool reduced = false ) const -> int;
        /** Returns the minimum value of a modifier type that get_mod() and get_amount() will push the player to. */
        auto get_min_val( std::string arg, bool reduced = false ) const -> int;
        /** Returns the maximum value of a modifier type that get_mod() and get_amount() will push the player to. */
        auto get_max_val( std::string arg, bool reduced = false ) const -> int;
        /** Returns true if the given modifier type's trigger chance is affected by size mutations. */
        auto get_sizing( const std::string &arg ) const -> bool;
        /** Returns the approximate percentage chance of a modifier type activating on any given tick, used for descriptions. */
        auto get_percentage( std::string arg, int val, bool reduced = false ) const -> double;
        /** Checks to see if a given modifier type can activate, and performs any rolls required to do so. mod is a direct
         *  multiplier on the overall chance of a modifier type activating. */
        auto activated( const time_point &when, std::string arg, int val,
                        bool reduced = false, double mod = 1 ) const -> bool;

        /** Check if the effect has the specified flag */
        auto has_flag( const std::string &flag ) const -> bool;

        /** Returns the modifier caused by addictions. Currently only handles painkiller addictions. */
        auto get_addict_mod( const std::string &arg, int addict_level ) const -> double;
        /** Returns true if the coughs caused by an effect can harm the player directly. */
        auto get_harmful_cough() const -> bool;
        /** Returns the percentage value by further applications of existing effects' duration is multiplied by. */
        auto get_dur_add_perc() const -> int;
        /** Returns the number of turns it takes for the intensity to fall by 1 or 0 if intensity isn't based on duration. */
        auto get_int_dur_factor() const -> time_duration;
        /** Returns the amount an already existing effect intensity is modified by further applications of the same effect. */
        auto get_int_add_val() const -> int;

        /** Returns a vector of the miss message messages and chances for use in add_miss_reason() while the effect is in effect. */
        auto get_miss_msgs() const -> std::vector<std::pair<std::string, int>>;

        /** Returns the value used for display on the speed modifier window in the player status menu. */
        auto get_speed_name() const -> std::string;

        /** Returns if the effect is supposed to be handled in Creature::movement */
        auto impairs_movement() const -> bool;

        /** Returns the effect's matching effect_type id. */
        auto get_id() const -> const efftype_id & {
            return eff_type->id;
        }

        void serialize( JsonOut &json ) const;
        void deserialize( JsonIn &jsin );

    protected:
        const effect_type *eff_type;
        time_duration duration;
        body_part bp;
        int intensity;
        time_point start_time;
        bool removed;

        // TODO: REMOVE!
        bool permanent = false;
    public:
        /**
         * Legacy compatibility TODO: Remove
         * No set un-permanent, because no use case
         */
        void set_permanent();

};

void load_effect_type( const JsonObject &jo );
void reset_effect_types();

auto find_all_effect_types() -> std::vector<efftype_id>;

auto texitify_base_healing_power( int power ) -> std::string;
auto texitify_healing_power( int power ) -> std::string;

// Inheritance here allows forward declaration of the map in class Creature.
class effects_map : public
    std::unordered_map<efftype_id, std::unordered_map<bodypart_str_id, effect>>
{
};

#endif // CATA_SRC_EFFECT_H
