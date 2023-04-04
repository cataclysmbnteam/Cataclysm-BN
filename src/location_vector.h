#pragma once
#ifndef CATA_SRC_LOCATION_VECTOR_H
#define CATA_SRC_LOCATION_VECTOR_H

#include <vector>

#include "locations.h"
#include "detached_ptr.h"

template<typename T>
class location_vector
{
    private:
        std::unique_ptr<location<T>> loc;
        std::vector<T *> contents;
    public:

        location_vector( std::unique_ptr<location<T>> &&loc ) {
            this->loc = std::move( loc );
        }

        void set_location( std::unique_ptr<location<T>> &&loc ) {
            if( this->loc ) {
                debugmsg( "Tried to set the location of a vector that already has one." );
            } else {
                this->loc = std::move( loc );
            }
        }

        void push_back( detached_ptr<T> &&obj ) {
            assert( loc );
            T *raw = obj.ptr;
            obj.ptr = nullptr;
            raw->set_location( &*loc );
            contents.push_back( raw );
        }

        size_t size() const {
            return contents.size();
        }
        bool empty() const {
            return contents.empty();
        }

        const std::vector<T *> as_vector() const {
            return contents;
        }

};


#endif
