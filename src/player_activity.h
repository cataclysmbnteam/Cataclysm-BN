#pragma once
#ifndef CATA_SRC_PLAYER_ACTIVITY_H
#define CATA_SRC_PLAYER_ACTIVITY_H

#include <climits>
#include <cstddef>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include "activity_actor.h"
#include "clone_ptr.h"
#include "enums.h"
#include "memory_fast.h"
#include "point.h"
#include "type_id.h"
#include "safe_reference.h"
#include "crafting.h"

class activity_actor;
class Character;
class JsonIn;
class JsonOut;
class avatar;
class monster;
class player;
class translation;
class activity_ptr;

/*
 * Struct to track activity by factors
*/
struct activity_speed {
    public:
        float assist = 1.0f;
        float bench = 1.0f;
        float player_speed = 1.0f;
        float stats = 1.0f;
        float skills = 1.0f;
        float tools = 1.0f;
        float morale = 1.0f;
        float light = 1.0f;

        //Returns total product of all factors
        inline float total() const {
            return 1.0f * assist * bench * player_speed * stats * skills * tools * morale * light ;
        }

        //Returns total amonut of moves based on factors
        inline int total_moves() const {
            return std::roundf( total() * 100.0f );
        }

        activity_speed() = default;
};

class player_activity
{
    private:
        activity_id type;
        bool bench_affected;
        bool speed_affected;
        bool skill_affected;
        bool tools_affected;
        bool morale_affected;
        std::unique_ptr<activity_actor> actor;

        std::set<distraction_type> ignored_distractions;

        friend activity_ptr;
        /** This keeps track of if the activity is currently running so we can avoid deleting it until it's done. */
        bool active = false;

        /** Unlocks the activity, or deletes it if it's already gone. */
        void resolve_active();

    public:
        /** Total number of moves required to complete the activity */
        int moves_total = 0;
        /** The number of moves remaining in this activity before it is complete. */
        int moves_left = 0;
        /** Controls whether this activity can be cancelled with 'pause' action */
        bool interruptable_with_kb = true;

        activity_speed speed = activity_speed();
        std::optional<bench_loc> bench;
        std::vector<safe_reference<item>> tools;

        // The members in the following block are deprecated, prefer creating a new
        // activity_actor.
        int index = 0;
        /**
         *   An activity specific value.
         *   DO NOT USE FOR ITEM INDEX
        */
        int position = 0;
        std::string name;
        std::vector<safe_reference<item>> targets;
        std::vector<int> values;
        std::vector<std::string> str_values;
        std::vector<tripoint> coords;
        std::unordered_set<tripoint> coord_set;
        std::vector<weak_ptr_fast<monster>> monsters;
        tripoint placement;

        bool no_drink_nearby_for_auto_consume = false;
        bool no_food_nearby_for_auto_consume = false;
        /** If true, the activity will be auto-resumed next time the player attempts
         *  an identical activity. This value is set dynamically.
         */
        bool auto_resume = false;


        player_activity();
        // This constructor does not work with activites using the new activity_actor system
        // TODO: delete this constructor once migration to the activity_actor system is complete
        player_activity( activity_id, int turns = 0, int Index = -1, int pos = INT_MIN,
                         const std::string &name_in = "" );
        ~player_activity();
        /**
         * Create a new activity with the given actor
         */
        // player_activity( const activity_actor &actor );
        player_activity( std::unique_ptr<activity_actor> &&actor );

        player_activity( player_activity && ) = default;
        player_activity &operator=( player_activity && ) = default;

        explicit operator bool() const {
            return !type.is_null();
        }
        bool is_null() const {
            return type.is_null();
        }

        int get_moves_left() const {
            if( actor ) {
                return actor->progress.get_moves_left();
            }
            return moves_left;
        }

        bool complete() const {
            if( actor ) {
                return actor->progress.complete();
            }
            return moves_left <= 0;
        }
        /*
        * Members to work with activity_actor.
        */

        /**
         * If this returns true, the action can be continued without
         * starting from scratch again (see player::backlog). This is only
         * possible if the player start the very same activity (with the same
         * parameters) again.
         */
        bool is_suspendable() const {
            return type->suspendable();
        }


        bool is_multi_type() const {
            return type->multi_activity();
        }
        bool is_assistable() const {
            return type->light_affected();
        }
        bool is_bench_affected() const {
            return type->bench_affected();
        }
        bool is_light_affected() const {
            return type->light_affected();
        }
        bool is_skill_affected() const {
            return type->skill_affected();
        }
        bool is_stats_affected() const {
            return type->stats_affected();
        }
        bool is_speed_affected() const {
            return type->speed_affected();
        }
        bool is_tools_affected() const {
            return type->tools_affected();
        }
        bool is_morale_affected() const {
            return type->morale_affected();
        }
        bool is_morale_blocked() const {
            return type->morale_blocked();
        }
        bool is_verbose_tooltip() const {
            return type->verbose_tooltip();
        }
        /** This replaces the former usage `act.type = ACT_NULL` */
        void set_to_null();

        const activity_id &id() const {
            return type;
        }
        bool rooted() const {
            return type != activity_id::NULL_ID() && type->rooted();
        }

        // Question to ask when the activity is to be stopped,
        // e.g. "Stop doing something?", already translated.
        std::string get_stop_phrase() const {
            return type->stop_phrase();
        }

        const translation &get_verb() const {
            return type->verb();
        }

        int get_value( size_t index, int def = 0 ) const;
        std::string get_str_value( size_t index, const std::string &def = "" ) const;

        /*
         * Bunch of functioins to calculate speed factors based on certain conditions
        */

        void calc_moves( const Character &who );
        float calc_bench_factor() const;
        float calc_light_factor( const Character &who ) const;
        float calc_skill_factor( const Character &who ) const;
        float calc_stats_factor( const Character &who ) const;
        float calc_tools_factor() const;
        float calc_morale_factor( int morale ) const;
        void find_best_bench( const tripoint &pos );

        /**
         * Helper that returns an activity specific progress message.
         */
        std::optional<std::string> get_progress_message( const avatar &u ) const;

        void serialize( JsonOut &json ) const;
        void deserialize( JsonIn &jsin );

        /**
         * Preform necessary initialization to start or resume the activity. Must be
         * called whenever a Character starts a new activity.
         * When resuming an activity, do not call activity_actor::start
         */
        void start_or_resume( Character &who, bool resuming );

        /**
         * Performs the activity for a single turn. If the activity is complete
         * at the end of the turn, do_turn also executes whatever actions, if
         * any, are needed to conclude the activity.
         */
        void do_turn( player &p );

        /**
         * Performs activity-specific cleanup when Character::cancel_activity() is called
         */
        void canceled( Character &who );

        /**
         * Returns true if activities are similar enough that this activity
         * can be resumed instead of starting the other activity.
         */
        bool can_resume_with( const player_activity &other, const Character &who ) const;

        bool is_distraction_ignored( distraction_type type ) const;
        void ignore_distraction( distraction_type type );
        void allow_distractions();
        void inherit_distractions( const player_activity & );
};

#endif // CATA_SRC_PLAYER_ACTIVITY_H
