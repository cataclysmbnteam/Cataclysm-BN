#pragma once
#ifndef CATA_SRC_LOCATION_VECTOR_H
#define CATA_SRC_LOCATION_VECTOR_H

#include <vector>
#include <memory>

#include "detached_ptr.h"

template<typename T>
class location;

struct tripoint;

template<typename T>
class location_vector
{
    private:
        std::unique_ptr<location<T>> loc;
        std::vector<T *> contents;
    public:

        location_vector( location<T> *loc );
        location_vector( location<T> *loc, std::vector<detached_ptr<T>> &from );
        location_vector( location_vector && ) = default;
        location_vector &operator=( location_vector && ) = default;

        ~location_vector();

        location<T> *get_location() const;

        void push_back( detached_ptr<T> &&obj );
        size_t size() const;
        bool empty() const;
        T *back() const;
        T *front() const;
        const std::vector<T *> &as_vector() const;

        typename std::vector<T *>::iterator erase( typename std::vector<T *>::const_iterator it,
                detached_ptr<T> *out = nullptr );

        typename std::vector<T *>::iterator insert( typename std::vector<T *>::iterator it,
                detached_ptr<T> &&obj );

        typename std::vector<T *>::iterator insert( typename std::vector<T *>::iterator it,
                typename std::vector<detached_ptr<T>>::iterator start,
                typename std::vector<detached_ptr<T>>::iterator end );
        typename std::vector<T *>::const_iterator begin() const;
        typename std::vector<T *>::const_iterator end() const;
        typename std::vector<T *>::iterator begin();
        typename std::vector<T *>::iterator end();
        typename std::vector<T *>::const_reverse_iterator rbegin() const;
        typename std::vector<T *>::const_reverse_iterator rend() const;
        typename std::vector<T *>::const_reverse_iterator crbegin() const;
        typename std::vector<T *>::const_reverse_iterator crend() const;
        typename std::vector<T *>::reverse_iterator rbegin();
        typename std::vector<T *>::reverse_iterator rend();
        typename std::vector<T *>::const_iterator cbegin() const;
        typename std::vector<T *>::const_iterator cend() const;
        typename std::vector<detached_ptr<T>> clear();

        void remove_with( std::function < detached_ptr<T>( detached_ptr<T> && ) > cb );

        void move_by( tripoint offset );

        /** this is needed until vehicles are GOs */
        void set_loc_hack( location<T> *loc );
};


#endif
