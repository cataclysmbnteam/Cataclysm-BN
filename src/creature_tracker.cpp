#include "creature_tracker.h"

#include <algorithm>
#include <cassert>
#include <ostream>
#include <string>
#include <utility>

#include "debug.h"
#include "mongroup.h"
#include "monster.h"
#include "mtype.h"
#include "point.h"
#include "string_formatter.h"
#include "type_id.h"

#define dbg(x) DebugLogFL((x),DC::Game)

Creature_tracker::Creature_tracker() = default;

Creature_tracker::~Creature_tracker() = default;

shared_ptr_fast<monster> Creature_tracker::find( const tripoint &pos ) const
{
    const auto iter = monsters_by_location.find( pos );
    if( iter != monsters_by_location.end() ) {
        const shared_ptr_fast<monster> &mon_ptr = iter->second;
        if( !mon_ptr->is_dead() ) {
            return mon_ptr;
        }
    }
    return nullptr;
}

static inline mfaction_id effective_faction( const monster &critter )
{
    static const mfaction_str_id playerfaction( "player" );
    return critter.friendly == 0 ? critter.faction : playerfaction;
}

inline size_t Creature_tracker::submap_offset( const point &pos )
{
    return pos.x / submap_size + submaps_in_grid * ( pos.y / submap_size );
}

std::vector<Creature_tracker::Faction_submap *>
Creature_tracker::find_in_area( mfaction_id faction, const tripoint &center, const int radius )
{
    std::vector<Faction_submap *> result;

    int range = ( ( radius - 1 ) / submap_size ) + 1;

    Faction_map &fmap = monster_faction_map_[faction];

    size_t sm = submap_offset( center.xy() );

    for( int dx = -range; dx <= range; dx++ ) {
        if( center.x + dx * submap_size < 0 ||
            center.x + dx * submap_size >= submaps_in_grid * submap_size ) {
            continue;
        }

        for( int dy = -range; dy <= range; dy++ ) {
            if( center.y + dy * submap_size < 0 ||
                center.y + dy * submap_size >= submaps_in_grid * submap_size ) {
                continue;
            }

            size_t submap = sm + dx + dy * submaps_in_grid;

            if( !fmap[submap].empty() ) {
                result.push_back( &( fmap[ submap ] ) );
            }
        }
    }
    return result;
}

std::vector<Creature_tracker::Faction_submap *>
Creature_tracker::find_at_range( mfaction_id faction, const tripoint &center, const int range )
{
    std::vector<Faction_submap *> result;

    Faction_map &fmap = monster_faction_map_[faction];

    size_t sm = submap_offset( center.xy() );
    for( int dx = -range; dx <= range; dx++ ) {

        if( center.x + dx * submap_size < 0 ||
            center.x + dx * submap_size >= submaps_in_grid * submap_size ) {
            continue;
        }

        if( center.y + range * submap_size < MAPSIZE_Y ) {
            size_t submap = sm + dx + range * submaps_in_grid;
            if( !fmap[submap].empty() ) {
                result.push_back( &( fmap[submap] ) );
            }
        }

        if( range == 0 ) {
            return result;
        }

        if( center.y - range * submap_size >= 0 ) {
            size_t submap = sm + dx - range * submaps_in_grid;
            if( !fmap[submap].empty() ) {
                result.push_back( &( fmap[submap] ) );
            }
        }
    }

    for( int dy = -range + 1; dy <= range - 1; dy++ ) {

        if( center.y + dy * submap_size < 0 ||
            center.y + dy * submap_size >=  submaps_in_grid * submap_size ) {
            continue;
        }

        if( center.x + range * submap_size <  submaps_in_grid * submap_size ) {
            size_t submap = sm + range + dy * submaps_in_grid;
            if( !fmap[submap].empty() ) {
                result.push_back( &( fmap[submap] ) );
            }
        }

        if( center.x - range * submap_size >= 0 ) {
            size_t submap = sm - range + dy * submaps_in_grid;
            if( !fmap[submap].empty() ) {
                result.push_back( &( fmap[submap] ) );
            }
        }
    }
    return result;
}

int Creature_tracker::temporary_id( const monster &critter ) const
{
    const auto iter = std::find_if( monsters_list.begin(), monsters_list.end(),
    [&]( const shared_ptr_fast<monster> &ptr ) {
        return ptr.get() == &critter;
    } );
    if( iter == monsters_list.end() ) {
        return -1;
    }
    return iter - monsters_list.begin();
}

shared_ptr_fast<monster> Creature_tracker::from_temporary_id( const int id )
{
    if( static_cast<size_t>( id ) < monsters_list.size() ) {
        return monsters_list[id];
    } else {
        return nullptr;
    }
}

bool Creature_tracker::add( shared_ptr_fast<monster> critter_ptr )
{
    assert( critter_ptr );
    monster &critter = *critter_ptr;

    if( critter.type->id.is_null() ) { // Don't want to spawn null monsters o.O
        return false;
    }

    if( critter.type->has_flag( MF_VERMIN ) ) {
        // Don't spawn vermin, they aren't implemented yet
        return false;
    }

    if( const shared_ptr_fast<monster> existing_mon_ptr = find( critter.pos() ) ) {
        // We can spawn stuff on hallucinations, but we need to kill them first
        if( existing_mon_ptr->is_hallucination() ) {
            existing_mon_ptr->die( nullptr );
            // But don't remove - that would change the monster order and could segfault
        } else if( critter.is_hallucination() ) {
            return false;
        } else {
            debugmsg( "there's already a monster at %d,%d,%d", critter.pos().x, critter.pos().y,
                      critter.pos().z );
            return false;
        }
    }

    if( MonsterGroupManager::monster_is_blacklisted( critter.type->id ) ) {
        return false;
    }

    monsters_list.emplace_back( critter_ptr );
    monsters_by_location[critter.pos()] = critter_ptr;
    add_to_faction_map( critter_ptr );
    return true;
}

void Creature_tracker::add_to_faction_map( shared_ptr_fast<monster> critter_ptr )
{
    assert( critter_ptr );
    monster &critter = *critter_ptr;

    tripoint pos = critter.pos();

    monster_faction_map_[ effective_faction( critter ) ][submap_offset( pos.xy() )].insert(
        critter_ptr );
}

size_t Creature_tracker::size() const
{
    return monsters_list.size();
}

bool Creature_tracker::update_pos( const monster &critter, const tripoint &new_pos )
{
    if( critter.is_dead() ) {
        // find ignores dead critters anyway, changing their position in the
        // monsters_by_location map is useless.
        remove_from_location_map( critter );
        return true;
    }

    if( const shared_ptr_fast<monster> new_critter_ptr = find( new_pos ) ) {
        auto &othermon = *new_critter_ptr;
        if( othermon.is_hallucination() ) {
            othermon.die( nullptr );
        } else {
            debugmsg( "update_zombie_pos: wanted to move %s to %d,%d,%d, but new location already has %s",
                      critter.disp_name(),
                      new_pos.x, new_pos.y, new_pos.z, othermon.disp_name() );
            return false;
        }
    }

    const auto iter = std::find_if( monsters_list.begin(), monsters_list.end(),
    [&]( const shared_ptr_fast<monster> &ptr ) {
        return ptr.get() == &critter;
    } );

    const tripoint &old_pos = critter.pos();
    if( iter != monsters_list.end() ) {
        monsters_by_location.erase( old_pos );
        monsters_by_location[new_pos] = *iter;

        int old_offset = submap_offset( old_pos.xy() );
        int new_offset = submap_offset( new_pos.xy() );
        if( old_offset != new_offset ) {
            monster_faction_map_[ effective_faction( critter ) ][old_offset].erase( *iter );
            monster_faction_map_[ effective_faction( critter ) ][new_offset].insert( *iter );
        }
        return true;
    } else {
        // We're changing the x/y/z coordinates of a zombie that hasn't been added
        // to the game yet. `add` will update monsters_by_location for us.
        debugmsg( "update_zombie_pos: no %s at %d,%d,%d (moving to %d,%d,%d)",
                  critter.disp_name(),
                  old_pos.x, old_pos.y, old_pos.z, new_pos.x, new_pos.y, new_pos.z );
        // Rebuild cache in case the monster actually IS in the game, just bugged
        rebuild_cache();
        return false;
    }
}

void Creature_tracker::remove_from_location_map( const monster &critter )
{
    const auto pos_iter = monsters_by_location.find( critter.pos() );
    if( pos_iter != monsters_by_location.end() && pos_iter->second.get() == &critter ) {
        monsters_by_location.erase( pos_iter );
        return;
    }

    // When it's not in the map at its current location, it might still be there under,
    // another location, so look for it.
    const auto iter = std::find_if( monsters_by_location.begin(), monsters_by_location.end(),
    [&]( const decltype( monsters_by_location )::value_type & v ) {
        return v.second.get() == &critter;
    } );
    if( iter != monsters_by_location.end() ) {
        monsters_by_location.erase( iter );
    }
}

void Creature_tracker::remove( const monster &critter, bool skip_cache )
{
    const auto iter = std::find_if( monsters_list.begin(), monsters_list.end(),
    [&]( const shared_ptr_fast<monster> &ptr ) {
        return ptr.get() == &critter;
    } );
    if( iter == monsters_list.end() ) {
        debugmsg( "Tried to remove invalid monster %s", critter.name() );
        return;
    }

    if( skip_cache ) {
        monsters_list.erase( iter );
        return;
    }

    const tripoint pos = critter.pos();
    for( auto &pair : monster_faction_map_ ) {
        const auto fac_iter = pair.second[submap_offset( pos.xy() )].find( *iter );
        if( fac_iter != pair.second[submap_offset( pos.xy() )].end() ) {
            // Need to do this manually because the shared pointer containing critter is kept valid
            // within removed_ and so the weak pointer in monster_faction_map_ is also valid.
            pair.second[submap_offset( pos.xy() )].erase( fac_iter );
            break;
        }
    }

    remove_from_location_map( critter );
    removed_.push_back( *iter );
    monsters_list.erase( iter );
}

void Creature_tracker::clear()
{
    monsters_list.clear();
    monsters_by_location.clear();
    monster_faction_map_.clear();
    removed_.clear();
}

void Creature_tracker::rebuild_cache()
{
    monsters_by_location.clear();
    monster_faction_map_.clear();
    for( const shared_ptr_fast<monster> &mon_ptr : monsters_list ) {
        monsters_by_location[mon_ptr->pos()] = mon_ptr;
        add_to_faction_map( mon_ptr );
    }
}

void Creature_tracker::swap_positions( monster &first, monster &second )
{
    if( first.pos() == second.pos() ) {
        return;
    }

    // Either of them may be invalid!
    const auto first_iter = monsters_by_location.find( first.pos() );
    const auto second_iter = monsters_by_location.find( second.pos() );
    // implied: first_iter != second_iter

    shared_ptr_fast<monster> first_ptr;
    if( first_iter != monsters_by_location.end() ) {
        first_ptr = first_iter->second;
        monsters_by_location.erase( first_iter );
    }

    shared_ptr_fast<monster> second_ptr;
    if( second_iter != monsters_by_location.end() ) {
        second_ptr = second_iter->second;
        monsters_by_location.erase( second_iter );
    }
    // implied: (first_ptr != second_ptr) or (first_ptr == nullptr && second_ptr == nullptr)

    tripoint temp = second.pos();
    second.spawn( first.pos() );
    first.spawn( temp );

    // If the pointers have been taken out of the list, put them back in.
    if( first_ptr ) {
        monsters_by_location[first.pos()] = first_ptr;
    }
    if( second_ptr ) {
        monsters_by_location[second.pos()] = second_ptr;
    }

    size_t first_offset = submap_offset( first.pos().xy() );
    size_t second_offset = submap_offset( second.pos().xy() );
    if( first_offset != second_offset ) {
        if( first_ptr ) {
            monster_faction_map_[effective_faction( first )][second_offset].erase( first_ptr );
            monster_faction_map_[effective_faction( first )][first_offset].insert( first_ptr );
        }
        if( second_ptr ) {
            monster_faction_map_[effective_faction( second )][first_offset].erase( second_ptr );
            monster_faction_map_[effective_faction( second )][second_offset].insert( second_ptr );
        }
    }
}

bool Creature_tracker::kill_marked_for_death()
{
    // Important: `Creature::die` must not be called after creature objects (NPCs, monsters) have
    // been removed, the dying creature could still have a pointer (the killer) to another creature.
    bool monster_is_dead = false;
    // Copy the list so we can iterate the copy safely *and* add new monsters from within monster::die
    // This happens for example with blob monsters (they split into two smaller monsters).
    const auto copy = monsters_list;
    for( const shared_ptr_fast<monster> &mon_ptr : copy ) {
        assert( mon_ptr );
        monster &critter = *mon_ptr;
        if( !critter.is_dead() ) {
            continue;
        }
        dbg( DL::Info ) << string_format( "cleanup_dead: critter %s hp:%d %s",
                                          critter.pos().to_string(), critter.get_hp(), critter.name() );

        critter.die( nullptr );
        monster_is_dead = true;
    }

    return monster_is_dead;
}

void Creature_tracker::remove_dead()
{
    // Can't use game::all_monsters() as it would not contain *dead* monsters.
    for( auto iter = monsters_list.begin(); iter != monsters_list.end(); ) {
        const monster &critter = **iter;
        if( critter.is_dead() ) {
            remove_from_location_map( critter );
            iter = monsters_list.erase( iter );
        } else {
            ++iter;
        }
    }

    removed_.clear();
}
