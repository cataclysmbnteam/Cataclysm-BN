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

        //TODO!: enable this stuff in debug mode only

        struct alloc_entry {
            const char *file;
            int line;
            void **backtrace;
            void **destroy_trace;
            void **remove_trace;
        };
#if !defined(RELEASE)
        inline static std::unordered_map<T *, alloc_entry> full_list;
#endif
    public:
        using value_type = T;

        static void mark_for_destruction( T *alloc ) {
            pending_deletion.insert( alloc );
            safe_reference<T>::mark_destroyed( alloc );
        }

        static bool cleanup() {
            if( pending_deletion.empty() ) {
                return false;
            }
            std::set<T *> dcopy = std::set<T *>( pending_deletion );
            pending_deletion.clear();
            for( T * const &p : dcopy ) {
#if !defined(RELEASE)
                auto it = full_list.find( p );
                free( it->second.backtrace );
                free( it->second.destroy_trace );
                free( it->second.remove_trace );
                full_list.erase( it );
#endif
                safe_reference<T>::mark_deallocated( p );
                delete p;
            }
            return true;
        }
#if !defined(RELEASE)
        static void add_debug_entry( T *obj, const char *file, int line, void **backtrace = nullptr ) {
            full_list.insert( { obj, {file, line, backtrace, nullptr, nullptr}} );
        }

        static void add_removed_trace( T *obj, void **backtrace = nullptr ) {
            auto it = full_list.find( obj );
            if( it == full_list.end() ) {
                return;
            }
            it->second.remove_trace = backtrace;
        }

        static void add_destroy_trace( T *obj, void **backtrace = nullptr ) {
            auto it = full_list.find( obj );
            if( it == full_list.end() ) {
                return;
            }
            it->second.destroy_trace = backtrace;
        }

        static void check_for_leaks() {
            for( auto &it : full_list ) {
                it.first->check_location( it.second.file, it.second.line, it.second.backtrace,
                                          it.second.destroy_trace, it.second.remove_trace );
            }
        }
#endif
};


void cleanup_arenas();

#endif
