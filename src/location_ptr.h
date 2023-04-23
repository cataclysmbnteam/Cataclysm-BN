#pragma once
#ifndef CATA_SRC_LOCATION_PTR_H
#define CATA_SRC_LOCATION_PTR_H

#include "locations.h"

template<typename T, bool error_if_null = true>
class location_ptr
{
    private:
        friend detached_ptr<T>;
        T *ptr = nullptr;
        std::unique_ptr<location<T>> loc;

        void update_location() {
            if( ptr ) {
                ptr->set_location( &*loc );
            }
        }

        void unset_location() {
            if( ptr ) {
                ptr->remove_location();
            }
        }

    public:
        location_ptr( location<T> *loc ) : loc( loc ) {};
        location_ptr( location_ptr & ) = delete;
        location_ptr( location_ptr && ) = delete;
        location_ptr operator=( location_ptr<T, error_if_null> & ) = delete;

        location_ptr<T, error_if_null> &operator=( detached_ptr<T> &&source ) {
            if( ptr ) {
                ptr->remove_location();
                ptr->destroy();
            }
            ptr = source.ptr;
            update_location();
            source.ptr = nullptr;
            return  *this;
        }

        location_ptr<T, error_if_null> &operator=( location_ptr<T, true> &&source ) {
            if( ptr ) {
                ptr->remove_location();
                ptr->destroy();
            }
            ptr = source.ptr;
            update_location();
            source.ptr = nullptr;
            return *this;
        }

        location_ptr<T, error_if_null> &operator=( location_ptr<T, false> &&source ) {
            if( ptr ) {
                ptr->remove_location();
                ptr->destroy();
            }
            ptr = source.ptr;
            update_location();
            source.ptr = nullptr;
            return *this;
        }

        ~location_ptr() {
            unset_location();
        }

        void set_location( location<T> *l ) {
            if( loc ) {
                debugmsg( "Attempted to set the location of a location_ptr that already has one" );
            }
            loc = std::unique_ptr<location<T>>( l );
        }

        inline T *get() const {
            if( !*this ) {
                if( error_if_null ) {
                    debugmsg( "Attempted to resolve invalid location_ptr" );
                }
                return &null_item_reference();
            }
            return ptr;
        }

        explicit inline operator bool() const {
            return !!*this;
        }

        inline bool operator!() const {
            return !ptr;
        }

        inline T &operator*() const {
            return *get();
        }

        inline T *operator->() const {
            return get();
        }

        inline bool operator==( const T &against ) const {
            return ptr == &against;
        }

        inline bool operator==( const T *against ) const {
            return ptr == against;
        }

        template <typename U>
        inline bool operator!=( const U against ) const {
            return !( *this == against );
        }

};

#endif
