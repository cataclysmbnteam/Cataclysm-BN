#pragma once
#ifndef CATA_SRC_ACTIVITY_TYPE_H
#define CATA_SRC_ACTIVITY_TYPE_H

#include <string>

#include "string_id.h"
#include "translations.h"

class JsonObject;
class activity_type;
class player;
class player_activity;

using activity_id = string_id<activity_type>;

/** @relates string_id */
template<>
auto string_id<activity_type>::obj() const -> const activity_type &;

enum class based_on_type : int {
    TIME = 0,
    SPEED,
    NEITHER
};

/** A class that stores constant information that doesn't differ between activities of the same type */
class activity_type
{
    private:
        activity_id id_;
        bool rooted_ = false;
        translation verb_ = to_translation( "THIS IS A BUG" );
        bool suspendable_ = true;
        based_on_type based_on_ = based_on_type::SPEED;
        bool no_resume_ = false;
        bool multi_activity_ = false;
        bool refuel_fires = false;
        bool auto_needs = false;

    public:
        auto id() const -> const activity_id & {
            return id_;
        }
        auto rooted() const -> bool {
            return rooted_;
        }
        auto suspendable() const -> bool {
            return suspendable_;
        }
        auto stop_phrase() const -> std::string;
        auto verb() const -> const translation & {
            return verb_;
        }
        auto based_on() const -> based_on_type {
            return based_on_;
        }
        auto no_resume() const -> bool {
            return no_resume_;
        }
        auto multi_activity() const -> bool {
            return multi_activity_;
        }
        /**
         * If true, player will refuel one adjacent fire if there is firewood spot adjacent.
         */
        auto will_refuel_fires() const -> bool {
            return refuel_fires;
        }
        /**
         * If true, player will automatically consume from relevant auto-eat/drink zones during activity
         */
        auto valid_auto_needs() const -> bool {
            return auto_needs;
        }
        void call_do_turn( player_activity *, player * ) const;
        /** Returns whether it had a finish function or not */
        auto call_finish( player_activity *, player * ) const -> bool;

        /** JSON stuff */
        static void load( const JsonObject &jo );
        static void check_consistency();
        static void reset();
};

#endif // CATA_SRC_ACTIVITY_TYPE_H
