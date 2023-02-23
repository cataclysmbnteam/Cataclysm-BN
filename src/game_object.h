#pragma once
#ifndef CATA_SRC_GAME_OBJECT_H
#define CATA_SRC_GAME_OBJECT_H

#include <memory>

#include "debug.h"
#include "locations.h"
#include "safe_reference.h"

template<typename T>
class game_object
{
    protected:
        std::unique_ptr<location<T>> loc = nullptr;

        game_object() {}

        game_object( const game_object & ) {}

    public:

        virtual ~game_object() = default;

        //TODO Get rid of null items so this can be removed
        virtual bool is_null() const = 0;

        void destroy() {
            if( loc != nullptr ) {
                loc->detach_for_destroy( static_cast<T *>( this ) );
            }
#if !defined(RELEASE)
            void **buf = static_cast<void **>( malloc( sizeof( void * ) * 20 ) );
            backtrace( buf, 20 );
            cata_arena<T>::add_destroy_trace( static_cast<T *>( this ), buf );
#endif
            cata_arena<T>::mark_for_destruction( static_cast<T *>( this ) );
        }

        void detach() {
            if( loc == nullptr ) {
                debugmsg( "Attempted to remove the location of [%s] that doesn't have one.", debug_name() );
                return;
            }
            loc->detach( static_cast<T *>( this ) );
            loc.reset( nullptr );
        }

        void set_location( location<T> *own ) {
            if( is_null() ) {
                delete own;
                return;
            }
            if( loc != nullptr ) {
                debugmsg( "Attempted to set the location of [%s] that already has one.", debug_name() );
                detach();
            }
            loc.reset( own );
        }

        void check_location( std::string file, int line, void **backtrace, void **destroy_trace,
                             void **remove_trace ) const {
            if( is_null() ) {
                return;
            }
            if( !loc ) {
                if( backtrace ) {
                    char **funcs = backtrace_symbols( backtrace, 20 );
                    for( int i = 0; i < 20 && funcs[i]; i++ ) {
                        DebugLog( DL::Error, DC::Main ) << funcs[i];
                    }
                    free( funcs );
                }
                if( remove_trace ) {
                    char **funcs = backtrace_symbols( remove_trace, 20 );
                    for( int i = 0; i < 20 && funcs[i]; i++ ) {
                        DebugLog( DL::Error, DC::Main ) << funcs[i];
                    }
                    free( funcs );
                }
                if( destroy_trace ) {
                    char **funcs = backtrace_symbols( destroy_trace, 20 );
                    for( int i = 0; i < 20 && funcs[i]; i++ ) {
                        DebugLog( DL::Error, DC::Main ) << funcs[i];
                    }
                    free( funcs );
                }
                debugmsg( "[%s] missing location during cleanup. Allocated at %s:%d", debug_name(), file, line );
                return;
            }
            if( !loc->check_for_corruption( static_cast<const item *>( this ) ) ) {
                if( backtrace ) {
                    char **funcs = backtrace_symbols( backtrace, 20 );
                    for( int i = 0; i < 20 && funcs[i]; i++ ) {
                        DebugLog( DL::Error, DC::Main ) << funcs[i];
                    }
                    free( funcs );
                }
                if( remove_trace ) {
                    char **funcs = backtrace_symbols( remove_trace, 20 );
                    for( int i = 0; i < 20 && funcs[i]; i++ ) {
                        DebugLog( DL::Error, DC::Main ) << funcs[i];
                    }
                    free( funcs );
                }
                if( destroy_trace ) {
                    char **funcs = backtrace_symbols( destroy_trace, 20 );
                    for( int i = 0; i < 20 && funcs[i]; i++ ) {
                        DebugLog( DL::Error, DC::Main ) << funcs[i];
                    }
                    free( funcs );
                }
                debugmsg( "Location corruption detected. Allocated at %s:%d, [%s] thought it was at %s", file,
                          line, debug_name(), loc->describe( nullptr, static_cast<const item *>( this ) ) );
            }
        }

        bool is_loaded() const {
            if( is_null() ) {
                return false;
            }
            if( !loc ) {
                return false;
            }
            return loc->is_loaded( static_cast<const T *>( this ) );
        }

        void remove_location() {
            if( is_null() ) {
                return;
            }
#if !defined(RELEASE)
            void **buf = static_cast<void **>( malloc( sizeof( void * ) * 20 ) );
            backtrace( buf, 20 );
            cata_arena<T>::add_removed_trace( static_cast<T *>( this ), buf );
#endif
            loc.reset( nullptr );
        }

        tripoint position( ) const {
            if( is_null() ) {
                debugmsg( "position called on a null object" );
                return tripoint_zero;
            }
            if( !loc ) {
                debugmsg( "position called on [%s] without a position", debug_name() );
                return tripoint_zero;
            }
            return loc->position( static_cast<const T *>( this ) );
        };

        /** Moves the game object's internal location to the absolute coords given.
         *  This should only be called on game objects that are already in a tile.
         *  It does not check the validity of the move or update anything other than the internal location.
         *  It's an optimization to prevent excessive reallocation, it's not for normal use.
         **/
        void move_to( const tripoint &p ) {
            go_tile_location *l = dynamic_cast<go_tile_location *>( &*loc );
            if( !l ) {
                debugmsg( "Tried to move_to on [%s] not in a tile", debug_name() );
                return;
            }
            l->move_to( p );
        }

        /** Moves the game object's internal location by the offset.
         *  See moves_to for further details.
         **/
        void move_by( const tripoint &offset ) {
            go_tile_location *l = dynamic_cast<go_tile_location *>( &*loc );
            if( !l ) {
                debugmsg( "Tried to move_by on [%s] not in a tile", debug_name() );
                return;
            }
            l->move_by( offset );
        }

        /** Returns the name that will be used when referring to the object in error messages */
        virtual std::string debug_name() const = 0;
};

#endif // CATA_SRC_GAME_OBJECT_H
