#include "game_object.h"

#include <memory>
#include <execinfo.h>

#include "cata_arena.h"
#include "item.h"
#include "locations.h"

#define GO_BACKTRACE (40)

template<typename T>
void game_object<T>::destroy()
{
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

template<typename T>
void game_object<T>::remove_location()
{
    if( is_null() ) {
        return;
    }/*
#if !defined(RELEASE)
    void **buf = static_cast<void **>( malloc( sizeof( void * ) * GO_BACKTRACE ) );
    backtrace( buf, GO_BACKTRACE );
    cata_arena<T>::add_removed_trace( static_cast<T *>( this ), buf );
#endif
*/
    loc = nullptr;
}

template<typename T>
void game_object<T>::set_location( location<T> *own )
{
    if( is_null() ) {
        return;
    }
    if( loc != nullptr ) {
        debugmsg( "Attempted to set the location of [%s] that already has one.", debug_name() );
        detach().release();
    }
    loc = own;
}

template<typename T>
detached_ptr<T> game_object<T>::detach()
{
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
    remove_location();
    return std::move( res );
}

template<typename T>
bool game_object<T>::attempt_detach( std::function < detached_ptr<T>
                                     ( detached_ptr<T> && ) > cb )
{
    location<T> *saved_loc = loc;
    detached_ptr<T> n = cb( detach() );
    if( n ) {
        saved_loc->attach( std::move( n ) );
        return false;
    } else {
        return true;
    }
}

template<typename T>
void game_object<T>::check_location( std::string file, int line, void **backtrace,
                                     void **destroy_trace,
                                     void **remove_trace ) const
{
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

template<typename T>
bool game_object<T>::is_loaded() const
{
    if( is_null() ) {
        return false;
    }
    if( !loc ) {
        return false;
    }
    return loc->is_loaded( static_cast<const T *>( this ) );
}

template<typename T>
tripoint game_object<T>::position( ) const
{
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


template
class game_object<item>;
