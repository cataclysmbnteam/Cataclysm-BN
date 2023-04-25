#pragma once
#ifndef CATA_SRC_GAME_OBJECT_H
#define CATA_SRC_GAME_OBJECT_H

#include <memory>

#include "debug.h"
#include "locations.h"
#include "detached_ptr.h"
#include "safe_reference.h"

#define GO_BACKTRACE (40)

class location_inventory;

template<typename T>
class game_object
{
    protected:
        location<T> *loc = nullptr;

        game_object() {}

        game_object( const game_object & ) {}

        void destroy() {
            if( is_null() ) {
                return;
            }
            if( loc != nullptr ) {
                debugmsg( "Attempted to destroy an item with a location." );
            }
#if !defined(RELEASE)
            void **buf = static_cast<void **>( malloc( sizeof( void * ) * GO_BACKTRACE ) );
            backtrace( buf, GO_BACKTRACE );
            cata_arena<T>::add_destroy_trace( static_cast<T *>( this ), buf );
#endif
            cata_arena<T>::mark_for_destruction( static_cast<T *>( this ) );
        }

        void remove_location() {
            if( is_null() ) {
                return;
            }
#if !defined(RELEASE)
            void **buf = static_cast<void **>( malloc( sizeof( void * ) * GO_BACKTRACE ) );
            backtrace( buf, GO_BACKTRACE );
            cata_arena<T>::add_removed_trace( static_cast<T *>( this ), buf );
#endif
            loc = nullptr;
        }

        void set_location( location<T> *own ) {
            if( is_null() ) {
                return;
            }
            if( loc != nullptr ) {
                debugmsg( "Attempted to set the location of [%s] that already has one.", debug_name() );
                detach();
            }
            loc = own;
        }

        friend detached_ptr<T>;
        friend location_ptr<T, true>;
        friend location_ptr<T, false>;
        friend location_inventory;
        friend location_vector<T>;

    public:

        virtual ~game_object() = default;

        //TODO Get rid of null items so this can be removed
        virtual bool is_null() const = 0;



        detached_ptr<T> detach() {
            if( is_null() ) {
                return detached_ptr<T>();
            }
            if( loc == nullptr ) {
                debugmsg( "Attempted to detach [%s] not in a location.", debug_name() );
                return detached_ptr<T>();
            }
            detached_ptr<T> res = std::move( loc->detach( static_cast<T *>( this ) ) );
#if !defined(RELEASE)
            void **buf = static_cast<void **>( malloc( sizeof( void * ) * GO_BACKTRACE ) );
            backtrace( buf, GO_BACKTRACE );
            cata_arena<T>::add_removed_trace( static_cast<T *>( this ), buf );
#endif
            loc = nullptr;
            return std::move( res );
        }

        virtual bool attempt_detach( std::function < detached_ptr<T>( detached_ptr<T> && ) > cb ) {
            if( loc == nullptr ) {
                debugmsg( "Attempted to detach (with attempt_detach) [%s] not in a location.", debug_name() );
                return false;
            }
            location<T> *saved_loc = loc;
            remove_location();
            detached_ptr<T> original( static_cast<T *>( this ) );
            detached_ptr<T> n = cb( std::move( original ) );
            if( n ) {
                if( &*n == this ) {
                    loc = saved_loc;
                    return false;
                } else {
                    debugmsg( "Returning a different item in attempt detach is not currently supported" );
                    return false;
                }
            }
            return true;
        }

        void check_location( std::string file, int line, void **backtrace, void **destroy_trace,
                             void **remove_trace ) const {
            if( is_null() ) {
                return;
            }
            if( !loc ) {
                if( backtrace ) {
                    char **funcs = backtrace_symbols( backtrace, GO_BACKTRACE );
                    for( int i = 0; i < GO_BACKTRACE && backtrace[i]; i++ ) {
                        DebugLog( DL::Error, DC::Main ) << funcs[i];
                    }
                    free( funcs );
                }
                if( remove_trace ) {
                    char **funcs = backtrace_symbols( remove_trace, GO_BACKTRACE );
                    for( int i = 0; i < GO_BACKTRACE && remove_trace[i]; i++ ) {
                        DebugLog( DL::Error, DC::Main ) << funcs[i];
                    }
                    free( funcs );
                }
                if( destroy_trace ) {
                    char **funcs = backtrace_symbols( destroy_trace, GO_BACKTRACE );
                    for( int i = 0; i < GO_BACKTRACE && destroy_trace[i]; i++ ) {
                        DebugLog( DL::Error, DC::Main ) << funcs[i];
                    }
                    free( funcs );
                }
                debugmsg( "[%s] missing location during cleanup. Allocated at %s:%d", debug_name(), file, line );
                return;
            }
            if( !loc->check_for_corruption( static_cast<const item *>( this ) ) ) {
                if( backtrace ) {
                    char **funcs = backtrace_symbols( backtrace, GO_BACKTRACE );
                    for( int i = 0; i < GO_BACKTRACE && backtrace[i]; i++ ) {
                        DebugLog( DL::Error, DC::Main ) << funcs[i];
                    }
                    free( funcs );
                }
                if( remove_trace ) {
                    char **funcs = backtrace_symbols( remove_trace, GO_BACKTRACE );
                    for( int i = 0; i < GO_BACKTRACE && remove_trace[i]; i++ ) {
                        DebugLog( DL::Error, DC::Main ) << funcs[i];
                    }
                    free( funcs );
                }
                if( destroy_trace ) {
                    char **funcs = backtrace_symbols( destroy_trace, GO_BACKTRACE );
                    for( int i = 0; i < GO_BACKTRACE && destroy_trace[i]; i++ ) {
                        DebugLog( DL::Error, DC::Main ) << funcs[i];
                    }
                    free( funcs );
                }
                debugmsg( "Location corruption detected. Allocated at %s:%d, [%s] thought it was at %s", file,
                          line, debug_name(), loc->describe( nullptr, static_cast<const T *>( this ) ) );
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

        /** Returns the name that will be used when referring to the object in error messages */
        virtual std::string debug_name() const = 0;
};

#endif // CATA_SRC_GAME_OBJECT_H
