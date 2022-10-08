#pragma once
#ifndef CATA_SRC_CREATURE_TRACKER_H
#define CATA_SRC_CREATURE_TRACKER_H

#include <cstddef>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "memory_fast.h"
#include "point.h"
#include "type_id.h"

class JsonIn;
class JsonOut;
class monster;

class Creature_tracker
{
    private:

        void add_to_faction_map( shared_ptr_fast<monster> critter );

        class weak_ptr_comparator
        {
            public:
                auto operator()( const weak_ptr_fast<monster> &lhs,
                                 const weak_ptr_fast<monster> &rhs ) const -> bool {
                    return lhs.lock().get() < rhs.lock().get();
                }
        };

        std::unordered_map<mfaction_id, std::set<weak_ptr_fast<monster>, weak_ptr_comparator>>
                monster_faction_map_;

        /**
         * Creatures that get removed via @ref remove are stored here until the end of the turn.
         * This keeps the objects valid and they can still be accessed instead of causing UB.
         */
        std::vector<shared_ptr_fast<monster>> removed_;

    public:
        Creature_tracker();
        ~Creature_tracker();
        /**
         * Returns the monster at the given location.
         * If there is no monster, it returns a `nullptr`.
         * Dead monsters are ignored and not returned.
         */
        auto find( const tripoint &pos ) const -> shared_ptr_fast<monster>;
        /**
         * Returns a temporary id of the given monster (which must exist in the tracker).
         * The id is valid until monsters are added or removed from the tracker.
         * The id remains valid through serializing and deserializing.
         * Use @ref from_temporary_id to get the monster pointer back. (The later may
         * return a nullptr if the given id is not valid.)
         */
        auto temporary_id( const monster &critter ) const -> int;
        auto from_temporary_id( int id ) -> shared_ptr_fast<monster>;
        /**
        * Adds the given monster to the tracker. @p critter must not be null.
         * If the operation succeeded, the monster pointer is now managed by this tracker.
         * @return Whether the operation was successful. It may fail if there is already
         * another monster at the location of the new monster.
         */
        auto add( shared_ptr_fast<monster> critter ) -> bool;
        auto size() const -> size_t;
        /** Updates the position of the given monster to the given point. Returns whether the operation
         *  was successful. */
        auto update_pos( const monster &critter, const tripoint &new_pos ) -> bool;
        /** Removes the given monster from the Creature tracker, adjusting other entries as needed. */
        void remove( const monster &critter );
        void clear();
        void rebuild_cache();
        /** Swaps the positions of two monsters */
        void swap_positions( monster &first, monster &second );
        /** Kills 0 hp monsters. Returns if it killed any. */
        auto kill_marked_for_death() -> bool;
        /** Removes dead monsters from. Their pointers are invalidated. */
        void remove_dead();

        auto get_monsters_list() const -> const std::vector<shared_ptr_fast<monster>> & {
            return monsters_list;
        }

        void serialize( JsonOut &jsout ) const;
        void deserialize( JsonIn &jsin );

        auto factions() const -> const decltype( monster_faction_map_ ) & {
            return monster_faction_map_;
        }

    private:
        std::vector<shared_ptr_fast<monster>> monsters_list;
        std::unordered_map<tripoint, shared_ptr_fast<monster>> monsters_by_location;
        /** Remove the monsters entry in @ref monsters_by_location */
        void remove_from_location_map( const monster &critter );
};

#endif // CATA_SRC_CREATURE_TRACKER_H
