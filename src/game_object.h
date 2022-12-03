#pragma once
#ifndef CATA_SRC_GAME_OBJECT_H
#define CATA_SRC_GAME_OBJECT_H

#include <memory>

#include "debug.h"
#include "locations.h"

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
            cata_arena<T>::mark_for_destruction( static_cast<T *>( this ) );
        }

        void detach() {
            if( loc == nullptr ) {
                debugmsg( "Attempted to remove the location of an object that doesn't have one." );
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
                debugmsg( "Attempted to set the location of an object that already has one." );
                detach();
            }
            loc.reset( own );
        }

        void move_location( location<T> *own ) {
            if( is_null() ) {
                delete own;
                return;
            }
            detach();
            loc.reset( own );
        }

        void check_location( std::string file, int line ) const {
            if( is_null() ) {
                return;
            }
            if( !loc ) {
                debugmsg( "Object missing location during cleanup. Allocated at %s:%d", file, line );
                return;
            }
            if( !loc->check_for_corruption( static_cast<const item *>( this ) ) ) {
                debugmsg( "Location corruption detected. Allocated at %s:%d, object thought it was at %s", file,
                          line, loc->describe( nullptr, static_cast<const item *>( this ) ) );
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
            loc.reset( nullptr );
        }

        tripoint position( ) const {
            if( is_null() ) {
                debugmsg( "position called on a null object" );
                return tripoint_zero;
            }
            if( !loc ) {
                debugmsg( "position called on an object without a position" );
                return tripoint_zero;
            }
            return loc->position( static_cast<const T *>( this ) );
        };
};

#endif // CATA_SRC_GAME_OBJECT_H
