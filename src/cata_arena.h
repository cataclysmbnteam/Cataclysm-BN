#pragma once
#ifndef CATA_SRC_ARENA_H
#define CATA_SRC_ARENA_H

#include <vector>

template <typename T>
class cata_arena
{
    private:
        inline static std::vector<T *> pending_deletion;

        //TODO!: enable this stuff in debug mode only

        struct alloc_entry {
            T *obj;
            const char *file;
            int line;
        };

        inline static std::vector<alloc_entry> full_list;

    public:
        using value_type = T;

        T *allocate( size_t n ) {
            T *obj = static_cast<T *>( malloc( sizeof( T ) * n ) );
            return obj;
        }

        static void mark_for_destruction( T *alloc ) {
            pending_deletion.push_back( alloc );
        }

        static void check_for_leaks() {
            for( alloc_entry &entry : full_list ) {
                entry.obj->check_location( entry.file, entry.line );
            }
        }

        static void cleanup() {
            for( T *&p : pending_deletion ) {
                std::remove_if( full_list.begin(), full_list.end(), [p]( alloc_entry & e ) {
                    return p == e.obj;
                } );
                delete p;
            }
            pending_deletion.clear();
            check_for_leaks();
        }

        static void add_debug_entry( T *obj, const char *file, int line ) {
            full_list.push_back( { obj, file, line} );
        }

};


void cleanup_arenas();

#endif
