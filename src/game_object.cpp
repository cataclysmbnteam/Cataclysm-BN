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
        debugmsg( "Tried to detach [%s] not in a location.", debug_name() );
        return detached_ptr<T>();
    }
    if( saved_loc != nullptr ) {
        debugmsg( "Tried to detach [%s] that's already in an attempted detach.", debug_name() );
        return detached_ptr<T>();
    }
    detached_ptr<T> res = loc->detach( static_cast<T *>( this ) );
#if !defined(RELEASE)
    void **buf = static_cast<void **>( malloc( sizeof( void * ) * GO_BACKTRACE ) );
    backtrace( buf, GO_BACKTRACE );
    cata_arena<T>::add_removed_trace( static_cast<T *>( this ), buf );
#endif
    remove_location();
    return  res;
}

template<typename T>
bool game_object<T>::attempt_detach( std::function < detached_ptr<T>
                                     ( detached_ptr<T> && ) > cb )
{
    /* Before changing this function be sure to consider the following edge case:
     * get_avatar().get_weapon().attempt_detach([](detached_ptr<item> &&e){
     *     get_avatar().set_weapon(std::move(e));
     *     get_avatar().get_weapon().attempt_detach([](detached_ptr<item> &&e){
     *         get_avatar().set_weapon(std::move(e));
     *         return detached_ptr<item>();
     *     }
     *     return detached_ptr<item>();
     * }
     *
     * Of course this is contrived but attempting to detach to somewhere the object already is or recursively detaching must work.
     * Most of the complexity in this function comes from handling these sorts of cases.
     */

    if( !loc ) {
        debugmsg( "Called attempt_detach on an object without a location" );
        return false;
    }

    assert( !saved_loc );

    //We keep our own copy of the location and also place one on the object.
    //If the object is added to location structures during the cb this property will be used and perhaps reset.
    location<T> *old_loc = loc;
    saved_loc = loc;
    remove_location();

    //Then we create a detached pointer of ourselves. Note that we haven't removed ourselves yet.
    T *self = static_cast<T *>( this );
    detached_ptr<T> orig( self );

    //Then run the callback.
    detached_ptr<T> n = cb( std::move( orig ) );

    //There are a bunch of awkwards cases here
    if( !n ) {
        //First the simplest one, we got a null detached_ptr back, we need to remove the object.

        //It might be that the saved_loc has been reset by a location structure (i.e. the object was placed somewhere else). It will have been removed at that point and we don't need to do anything.
        if( saved_loc ) {
            //Otherwise we need to remove it from its location without destroying it. It may have another valid detached_ptr somewhere at this point or orig may still be valid.
            saved_loc->detach( self ).release();
        }
        return true;
    } else {
        //We got back a valid detached_ptr

        if( &*n == self ) {
            if( saved_loc ) {
                //It's the same as the original and hasn't been moved, i.e. we just reset our location we didn't actually move
                self->set_location( saved_loc );
                saved_loc = nullptr;
                n.release();
                return false;
            } else {
                //It's the same as the original, but it's been moved at some point, we need to reattach it.
                n->set_location( old_loc );
                old_loc->attach( std::move( n ) );
                return false;
            }
        } else {
            //We've been given something different.
            if( saved_loc ) {
                //If it hasn't been placed elsewhere then remove the old one now.
                saved_loc->detach( self ).release();
                saved_loc = nullptr;
            }
            n->set_location( old_loc );
            old_loc->attach( std::move( n ) );
            return false;
        }
    }
}

template<typename T>
void game_object<T>::resolve_saved_loc()
{
    if( !saved_loc ) {
        return;
    }
    T *self = static_cast<T *>( this );
    saved_loc->detach( self ).release();
    saved_loc = nullptr;
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
