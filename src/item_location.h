#pragma once
#ifndef CATA_SRC_ITEM_LOCATION_H
#define CATA_SRC_ITEM_LOCATION_H

#include <memory>
#include <string>

struct tripoint;
class item;
class Character;
class map_cursor;
class vehicle_cursor;
class JsonIn;
class JsonOut;

/**
 * A lightweight handle to an item independent of it's location
 * Unlike a raw pointer can be (de-)serialized to/from JSON
 * Provides a generic interface of querying, obtaining and removing an item
 * Is invalidated by many operations (including copying of the item)
 */
class item_location
{
    public:
        enum class type : int {
            invalid = 0,
            character = 1,
            map = 2,
            vehicle = 3,
            container = 4
        };

        item_location();

        static const item_location nowhere;

        item_location( Character &ch, item *which );
        item_location( const map_cursor &mc, item *which );
        item_location( const vehicle_cursor &vc, item *which );
        item_location( const item_location &container, item *which );

        void serialize( JsonOut &js ) const;
        void deserialize( JsonIn &js );

        auto operator==( const item_location &rhs ) const -> bool;
        auto operator!=( const item_location &rhs ) const -> bool;

        explicit operator bool() const;

        auto operator*() -> item &;
        auto operator*() const -> const item &;

        auto operator->() -> item *;
        auto operator->() const -> const item *;

        /** Returns the type of location where the item is found */
        auto where() const -> type;

        /** Returns the position where the item is found */
        auto position() const -> tripoint;

        /** Describes the item location
         *  @param ch if set description is relative to character location */
        auto describe( const Character *ch = nullptr ) const -> std::string;

        /** Move an item from the location to the character inventory
         *  @param ch Character who's inventory gets the item
         *  @param qty if specified limits maximum obtained charges
         *  @warning caller should restack inventory if item is to remain in it
         *  @warning all further operations using this class are invalid
         *  @warning it is unsafe to call this within unsequenced operations (see #15542)
         *  @return item_location for the item */
        auto obtain( Character &ch, int qty = -1 ) -> item_location;

        /** Calculate (but do not deduct) number of moves required to obtain an item
         *  @see item_location::obtain */
        auto obtain_cost( const Character &ch, int qty = -1 ) const -> int;

        /** Removes the selected item from the game
         *  @warning all further operations using this class are invalid */
        void remove_item();

        /** Gets the selected item or nullptr */
        auto get_item() -> item *;
        auto get_item() const -> const item *;

        void set_should_stack( bool should_stack ) const;

        /** returns the parent item, or an invalid location if it has no parent */
        auto parent_item() const -> item_location;

        // This is a dirty hack, don't use. TODO: Make not necessary, then delete
        void make_dirty();

    private:
        class impl;

        std::shared_ptr<impl> ptr;
};

#endif // CATA_SRC_ITEM_LOCATION_H
