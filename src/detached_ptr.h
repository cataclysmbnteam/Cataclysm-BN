#pragma once
#ifndef CATA_SRC_DETACHED_PTR_H
#define CATA_SRC_DETACHED_PTR_H

#include "debug.h"

template <typename T>
class game_object;

template <typename T, bool error_if_null>
class location_ptr;

template <typename T>
class location_vector;

class location_inventory;

template<typename T>
class detached_ptr
{
    private:
        friend T;
        friend game_object<T>;
        friend location_ptr<T, true>;
        friend location_ptr<T, false>;
        friend location_vector<T>;
        friend location_inventory;
        T *ptr = nullptr;
    public:
        detached_ptr() {
            ptr = nullptr;
        }

        detached_ptr( detached_ptr &&source ) {
            ptr = source.ptr;
            source.ptr = nullptr;
        }

        detached_ptr<T> &operator=( detached_ptr &&source ) {
            if( &source == this ) {
                return *this;
            }
            if( ptr ) {
                ptr->destroy();
            }
            ptr = source.ptr;
            source.ptr = nullptr;
            return *this;
        }

        explicit detached_ptr( location_ptr<T, true> &&loc ) {
            loc.unset_location();
            ptr = loc.ptr;
            loc.ptr = nullptr;
        }

        explicit detached_ptr( location_ptr<T, false> &&loc ) {
            loc.unset_location();
            ptr = loc.ptr;
            loc.ptr = nullptr;
        }

        ~detached_ptr() {
            if( ptr ) {
                ptr->destroy();
            }
        }

        inline T *get() const {
            if( !*this ) {
                debugmsg( "Attempted to resolve invalid detached_ptr" );
                return nullptr;//TODO!: error pointer
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
    private:
        detached_ptr( const detached_ptr & ) = delete;
        detached_ptr<T> &operator=( const detached_ptr & ) = delete;

        explicit detached_ptr( T *obj ) {
            assert( obj != nullptr );
            ptr = obj;
        }

        T *release() {
            T *ret = ptr;
            ptr = nullptr;
            return ret;
        }
};

template<typename T>
class location_container
{

};


#endif
