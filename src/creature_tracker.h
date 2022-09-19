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
#include "game_constants.h"

class JsonIn;
class JsonOut;
class monster;

class Creature_tracker
{
    public:

        class weak_ptr_comparator
        {
            public:
                bool operator()( const weak_ptr_fast<monster> &lhs,
                                 const weak_ptr_fast<monster> &rhs ) const {
                    return lhs.lock().get() < rhs.lock().get();
                }
        };
    private:

        static constexpr int submaps_in_grid = 9;
        static constexpr int submap_size = 16;

        static_assert( submaps_in_grid * submap_size >= MAPSIZE_X &&submaps_in_grid * submap_size >=
                       MAPSIZE_Y,
                       "The creature tracker's map must be at least as big as the main map" );

        void add_to_faction_map( shared_ptr_fast<monster> critter );

        using Faction_submap = std::set<weak_ptr_fast<monster>, weak_ptr_comparator>;
        using Faction_map = std::array<Faction_submap, submaps_in_grid *submaps_in_grid>;

        std::unordered_map<mfaction_id, Faction_map> monster_faction_map_;

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
        shared_ptr_fast<monster> find( const tripoint &pos ) const;

        /**
         * Returns a list of all monsters of a faction that are on submaps that could be within radius of the center.
         * This isn't accurate but it's fast, good for culling.
         */
        std::vector<Faction_submap *>
        find_in_area( mfaction_id faction, const tripoint &center, const int radius );

        /**
        * Returns a list of all monsters of a faction that are on submaps at a specific range around the center.
        * This allows for searches to spread outwards.
        */
        std::vector<Faction_submap *>
        find_at_range( mfaction_id faction, const tripoint &center, const int range );

        /**
         * Returns a temporary id of the given monster (which must exist in the tracker).
         * The id is valid until monsters are added or removed from the tracker.
         * The id remains valid through serializing and deserializing.
         * Use @ref from_temporary_id to get the monster pointer back. (The later may
         * return a nullptr if the given id is not valid.)
         */
        int temporary_id( const monster &critter ) const;
        shared_ptr_fast<monster> from_temporary_id( int id );
        /**
        * Adds the given monster to the tracker. @p critter must not be null.
         * If the operation succeeded, the monster pointer is now managed by this tracker.
         * @return Whether the operation was successful. It may fail if there is already
         * another monster at the location of the new monster.
         */
        bool add( shared_ptr_fast<monster> critter );
        size_t size() const;
        /** Updates the position of the given monster to the given point. Returns whether the operation
         *  was successful. */
        bool update_pos( const monster &critter, const tripoint &new_pos );
        /** Removes the given monster from the Creature tracker, adjusting other entries as needed. */
        void remove( const monster &critter, bool skip_cache = false );
        void clear();
        void rebuild_cache();
        /** Swaps the positions of two monsters */
        void swap_positions( monster &first, monster &second );
        /** Kills 0 hp monsters. Returns if it killed any. */
        bool kill_marked_for_death();
        /** Removes dead monsters from. Their pointers are invalidated. */
        void remove_dead();

        const std::vector<shared_ptr_fast<monster>> &get_monsters_list() const {
            return monsters_list;
        }

        void serialize( JsonOut &jsout ) const;
        void deserialize( JsonIn &jsin );

        const decltype( monster_faction_map_ ) &factions() const {
            return monster_faction_map_;
        }

    private:
        std::vector<shared_ptr_fast<monster>> monsters_list;
        std::unordered_map<tripoint, shared_ptr_fast<monster>> monsters_by_location;
        /** Remove the monsters entry in @ref monsters_by_location */
        void remove_from_location_map( const monster &critter );
        size_t submap_offset( const point &pos );
};

#endif // CATA_SRC_CREATURE_TRACKER_H
