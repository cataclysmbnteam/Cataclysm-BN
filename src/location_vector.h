#pragma once
#ifndef CATA_SRC_LOCATION_VECTOR_H
#define CATA_SRC_LOCATION_VECTOR_H

#include <vector>

#include "locations.h"
#include "detached_ptr.h"

//TODO!: probably move these to cpp
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

        typename std::vector<T *>::iterator erase( typename std::vector<T *>::iterator it,
                detached_ptr<T> *out = nullptr );

        typename std::vector<T *>::iterator insert( typename  std::vector<T *>::iterator it,
                detached_ptr<T> &&obj );

        typename std::vector<T *>::const_iterator begin() const {
            return contents.begin();
        }

        typename std::vector<T *>::const_iterator end() const {
            return contents.end();
        }

        typename std::vector<T *>::iterator begin() {
            return contents.begin();
        }

        typename std::vector<T *>::iterator end() {
            return contents.end();
        }

        typename std::vector<T *>::const_reverse_iterator rbegin() const {
            return contents.rbegin();
        }

        typename std::vector<T *>::const_reverse_iterator rend() const {
            return contents.rend();
        }

        typename std::vector<T *>::reverse_iterator rbegin() {
            return contents.rbegin();
        }

        typename std::vector<T *>::reverse_iterator rend() {
            return contents.rend();
        }

};


#endif
