#pragma once
#ifndef CATA_SRC_GAME_OBJECT_H
#define CATA_SRC_GAME_OBJECT_H


#include "debug.h"
#include "detached_ptr.h"
#include "safe_reference.h"
#include "point.h"


class location_inventory;

template<typename T>
class location;

template<typename T>
class game_object
{
    protected:
        location<T> *loc = nullptr;

        game_object() {}

        game_object( const game_object & ) {}

        void destroy();

        void remove_location();
        void set_location( location<T> *own );

        friend detached_ptr<T>;
        friend location_ptr<T, true>;
        friend location_ptr<T, false>;
        friend location_inventory;
        friend location_vector<T>;

    public:

        virtual ~game_object() = default;

        //TODO Get rid of null items so this can be removed
        virtual bool is_null() const = 0;



        detached_ptr<T> detach();

        virtual bool attempt_detach( std::function < detached_ptr<T>( detached_ptr<T> && ) > cb );

        void check_location( std::string file, int line, void **backtrace, void **destroy_trace,
                             void **remove_trace ) const;

        bool is_loaded() const;

        tripoint position( ) const;
        /** Returns the name that will be used when referring to the object in error messages */
        virtual std::string debug_name() const = 0;
};

#endif // CATA_SRC_GAME_OBJECT_H
