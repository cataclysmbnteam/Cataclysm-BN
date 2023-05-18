#include "active_item_cache.h"

#include <algorithm>
#include <utility>

#include "item.h"
#include "safe_reference.h"

void active_item_cache::remove( const item *it )
{
    std::vector<cache_reference<item>> &items = active_items[it->processing_speed()];
    items.erase( std::remove( items.begin(), items.end(), it ), items.end() );
    if( it->can_revive() ) {
        std::vector<cache_reference<item>> &corpse = special_items[ special_item_type::corpse ];
        corpse.erase( std::remove( corpse.begin(), corpse.end(), it ), corpse.end() );
    }
    if( it->get_use( "explosion" ) ) {
        std::vector<cache_reference<item>> &explosive = special_items[ special_item_type::explosive ];
        explosive.erase( std::remove( explosive.begin(), explosive.end(), it ), explosive.end() );
    }
}

void active_item_cache::add( item &it )
{
    // If the item is alread in the cache for some reason, don't add a second reference
    std::vector<cache_reference<item>> &target_list = active_items[it.processing_speed()];
    if( std::find( target_list.begin(), target_list.end(), it ) != target_list.end() ) {
        return;
    }
    if( it.can_revive() ) {
        special_items[ special_item_type::corpse ].push_back( it );
    }
    if( it.get_use( "explosion" ) ) {
        special_items[ special_item_type::explosive ].push_back( it );
    }
    target_list.push_back( it );
}

bool active_item_cache::empty() const
{
    for( std::pair<int, std::vector<cache_reference<item>>> active_queue : active_items ) {
        if( !active_queue.second.empty() ) {
            return false;
        }
    }
    return true;
}

std::vector<item *> active_item_cache::get()
{
    std::vector<item *> all_cached_items;
    for( std::pair<const int, std::vector<cache_reference<item>>> &kv : active_items ) {
        for( std::vector<cache_reference<item>>::iterator it = kv.second.begin(); it != kv.second.end(); ) {
            if( *it ) {
                all_cached_items.push_back( & **it );
                ++it;
            } else {
                it = kv.second.erase( it );
            }
        }
    }
    return all_cached_items;
}

std::vector<item *> active_item_cache::get_for_processing()
{
    std::vector<item *> items_to_process;
    for( std::pair < const int, std::vector<cache_reference<item>>> &kv : active_items ) {
        // Rely on iteration logic to make sure the number is sane.
        int num_to_process = kv.second.size() / kv.first;
        std::vector<cache_reference<item>>::iterator it = kv.second.begin();

        //TODO!: This is a bit of a hack, at the very least check the maths
        //Skip the entries that were processed last turn.
        it += num_to_process * ( to_turn<int>( calendar::turn ) % kv.first );

        for( ; it != kv.second.end() && num_to_process >= 0; ) {
            if( *it ) {
                items_to_process.push_back( & **it );
                --num_to_process;
                ++it;
            } else {
                // The item has been destroyed, so remove the reference from the cache
                it = kv.second.erase( it );
            }
        }
        // Rotate the returned items to the end of their list so that the items that weren't
        // returned this time will be first in line on the next call
        // kv.second.splice( kv.second.end(), kv.second, kv.second.begin(), it );
    }
    return items_to_process;
}

std::vector<item *> active_item_cache::get_special( special_item_type type )
{
    std::vector<item *> matching_items;
    for( const cache_reference<item> &it : special_items[type] ) {
        if( it ) {
            matching_items.push_back( &*it );
        }
    }
    return matching_items;
}
