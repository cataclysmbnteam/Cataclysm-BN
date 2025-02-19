#pragma once
#ifndef CATA_SRC_ACTIVITY_TYPE_H
#define CATA_SRC_ACTIVITY_TYPE_H

#include <string>
#include <optional>

#include "catalua_type_operators.h"
#include "string_id.h"
#include "translations.h"
#include "character_stat.h"
#include "type_id.h"

class JsonObject;
class activity_type;
class player;
class player_activity;

using activity_id = string_id<activity_type>;

/** @relates string_id */
template<>
const activity_type &string_id<activity_type>::obj() const;

/** A class that stores constant information that doesn't differ between activities of the same type */
class activity_type
{
    private:
        activity_id id_;
        bool rooted_ = false;
        translation verb_ = to_translation( "THIS IS A BUG" );
        bool suspendable_ = true;
        bool no_resume_ = false;
        bool multi_activity_ = false;
        bool refuel_fires = false;
        bool auto_needs = false;
        bool special_ = false;
        bool complex_moves_ = false;
        bool assistable_ = false;
        bool bench_affected_ = false;
        bool light_affected_ = false;
        bool skill_affected_ = false;
        bool speed_affected_ = false;
        bool stats_affected_ = false;
        bool tools_affected_ = false;
        bool morale_affected_ = false;
        bool morale_blocked_ = false;
        bool verbose_tooltip_ = true;

    public:
        std::unordered_map<character_stat, int> stats = {};
        std::unordered_map<skill_id, int> skills = {};
        std::unordered_map<quality_id, int> qualities = {};

        const activity_id &id() const {
            return id_;
        }
        bool rooted() const {
            return rooted_;
        }
        bool suspendable() const {
            return suspendable_;
        }
        std::string stop_phrase() const;
        const translation &verb() const {
            return verb_;
        }
        bool no_resume() const {
            return no_resume_;
        }
        bool multi_activity() const {
            return multi_activity_;
        }

        /*
         * "Special" activities do not use basic logic
         * instead those rely on their own unique spagett
        */
        bool special() const {
            return special_;
        }
        bool complex_moves() const {
            return complex_moves_;
        }
        bool assistable() const {
            return assistable_;
        }
        bool bench_affected() const {
            return bench_affected_;
        }
        bool light_affected() const {
            return light_affected_;
        }
        bool skill_affected() const {
            return skill_affected_;
        }
        bool stats_affected() const {
            return stats_affected_;
        }
        bool speed_affected() const {
            return speed_affected_;
        }
        bool tools_affected() const {
            return tools_affected_;
        }
        bool morale_affected() const {
            return morale_affected_;
        }
        bool morale_blocked() const {
            return morale_blocked_;
        }
        bool verbose_tooltip() const {
            return verbose_tooltip_;
        }
        /**
         * If true, player will refuel one adjacent fire if there is firewood spot adjacent.
         */
        bool will_refuel_fires() const {
            return refuel_fires;
        }
        /**
         * If true, player will automatically consume from relevant auto-eat/drink zones during activity
         */
        bool valid_auto_needs() const {
            return auto_needs;
        }
        void call_do_turn( player_activity *, player * ) const;
        /** Returns whether it had a finish function or not */
        bool call_finish( player_activity *, player * ) const;

        /** JSON stuff */
        static void load( const JsonObject &jo );
        static void check_consistency();
        static void reset();

        LUA_TYPE_OPS( activity_type, id_ );
};

#endif // CATA_SRC_ACTIVITY_TYPE_H
