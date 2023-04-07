#pragma once
#ifndef CATA_SRC_DETACHED_PTR_H
#define CATA_SRC_DETACHED_PTR_H

//#include "game_object.h"

template <typename T>
class game_object;

template <typename T>
class location_ptr;

template <typename T>
class location_vector;

template<typename T>
class detached_ptr
{
    private:
        friend T;
        friend game_object<T>;
        friend location_ptr<T>;
        friend location_vector<T>;
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
            ptr = source.ptr;
            source.ptr = nullptr;
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
        detached_ptr( detached_ptr & ) = delete;
        detached_ptr<T> &operator=( detached_ptr & ) = delete;

        detached_ptr( T *obj ) {
            assert( obj != nullptr );
            ptr = obj;
        }
};

template<typename T>
class location_container
{

};


#endif
