#pragma once
#ifndef CATA_SRC_ARENA_H
#define CATA_SRC_ARENA_H

#include <unordered_map>
#include <set>
#include <execinfo.h>

#include "safe_reference.h"

template <typename T>
class cata_arena
{
    private:
        inline static std::set<T *> pending_deletion;

    public:
        using value_type = T;

        static void mark_for_destruction( T *alloc ) {
            pending_deletion.insert( alloc );
            safe_reference<T>::mark_destroyed( alloc );
            cache_reference<T>::mark_destroyed( alloc );
        }

        static bool cleanup() {
            if( pending_deletion.empty() ) {
                return false;
            }
            std::set<T *> dcopy = std::set<T *>( pending_deletion );
            pending_deletion.clear();
            for( T * const &p : dcopy ) {
                safe_reference<T>::mark_deallocated( p );
                delete p;
            }
            return true;
        }
};


void cleanup_arenas();

#endif
