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

        location_vector( location<T> *loc ) : loc( loc ) {};
        location_vector( location<T> *loc, std::vector<detached_ptr<T>> &from ) : loc( loc ) {
            for( detached_ptr<T> &obj : from ) {
                obj->set_location( &*loc );
                contents.push_back( &*obj );
                obj.ptr = nullptr;
            }
            from.clear();
        };

        void push_back( detached_ptr<T> &&obj ) {
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
