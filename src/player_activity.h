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
#include <queue>
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

struct simple_task {
    // Name of the target that's being processed
    const std::string target_name;
    /* Total number of moves required for this target/task to complete */
    const int moves_total = 0;
    /* The number of moves remaining for this target/task to complete */
    int moves_left = 0;

    simple_task( const std::string &name_, int moves_total_ )
        : target_name( name_ ), moves_total( moves_total_ ), moves_left( moves_total_ ) {
    }
    simple_task( const std::string &name_, int moves_total_, int moves_left_ )
        : target_name( name_ ), moves_total( moves_total_ ), moves_left( moves_left_ ) {
    }

    inline const bool complete() const {
        return moves_left <= 0;
    }

    //Json stuff

    void serialize( JsonOut &json ) const;

    simple_task() = default;
};

/*
 * Special class to track progress of current activity
 * Outside is a queue with slighly altered functionality
*/
class progress_counter
{
    private:
        /** Total number of moves required to complete the activity aka all the tasks */
        int moves_total = 0;
        /** The number of moves remaining in this activity before it is complete aka all the tasks */
        int moves_left = 0;
        //Index of current task - 1-based
        int idx = 1;
        //Counts total amount of tasks - done and in queue
        int total_tasks = 0;

        std::deque<simple_task> targets;

        // Only exists for dummy
        inline void pop_back() {
            if( targets.empty() ) {
                debugmsg( "task was popped out of empty progress queue" );
                return;
            }
            moves_left -= targets.back().moves_left;
            targets.pop_back();
            idx++;
        }

    public:
        inline void emplace( std::string name, int moves_total_ ) {
            moves_total += moves_total_;
            moves_left += moves_total_;
            targets.emplace_back( name, moves_total_ );
            total_tasks++;
        }
        inline void emplace( std::string name, int moves_total_, int moves_left_ ) {
            moves_total += moves_total_;
            moves_left += moves_left_;
            targets.emplace_back( name, moves_total_, moves_left_ );
            total_tasks++;
        }
        inline void pop() {
            if( targets.empty() ) {
                debugmsg( "task was popped out of empty progress queue" );
                return;
            }
            moves_left -= targets.front().moves_left;
            targets.pop_front();
            idx++;
        }
        inline const bool empty() const {
            return targets.empty();
        }
        inline const bool complete() const {
            return total_tasks > 0 && moves_left <= 0;
        }
        inline const bool invalid() const {
            return total_tasks == 0 && targets.empty();
        }
        inline const int get_index() const {
            return idx;
        }
        inline const int get_total_tasks() const {
            return total_tasks;
        }
        inline const int get_moves_total() const {
            return moves_total;
        }
        inline const int get_moves_left() const {
            return moves_left;
        }
        inline size_t size() const {
            return targets.size();
        }
        inline const simple_task front() const {
            return targets.front();
        }
        inline const simple_task back() const {
            return targets.back();
        }
        //Modifies move_left of the first task(and total progress)
        inline void mod_moves_left( int moves ) {
            moves_left += moves;
            targets.front().moves_left += moves;
        }
        /*
        * Creates a dummy task, ends it instantaneously
        * For very certain and rare occasions, use cautiously
        * Basically to properly process "unique" activities, like autodrive
        */
        inline void dummy() {
            emplace( "If you see this it's a bug", 1 );
            pop_back();
        }

        //Json stuff

        void serialize( JsonOut &json ) const;
        void deserialize( JsonIn &jsin );
};
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
        inline int totalMoves() const {
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
        bench_l *bench = nullptr;
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

        /*
        * Members to work with activity_actor.
        */

        //List of task with names of ACTUAL targets, like a wall u mine or a grave u dig
        //Also tracks number of moves left and total
        progress_counter progress;

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
