#pragma once
#ifndef CATA_SRC_ITEM_STACK_H
#define CATA_SRC_ITEM_STACK_H

#include <cstddef>

#include "colony.h"
#include "item.h" // IWYU pragma: keep
#include "units.h"

// A wrapper class to bundle up the references needed for a caller to safely manipulate
// items and obtain information about items at a particular map x/y location.
// Note this does not expose the container itself,
// which means you cannot call e.g. cata::colony::erase() directly.

// Pure virtual base class for a collection of items with origin information.
// Only a subset of the functionality is callable without casting to the specific
// subclass, e.g. not begin()/end() or range loops.
class item_stack
{
    protected:
        cata::colony<item> *items;

    public:
        using iterator = cata::colony<item>::iterator;
        using const_iterator = cata::colony<item>::const_iterator;
        using reverse_iterator = cata::colony<item>::reverse_iterator;
        using const_reverse_iterator = cata::colony<item>::const_reverse_iterator;

        item_stack( cata::colony<item> *items ) : items( items ) { }
        virtual ~item_stack() = default;

        auto size() const -> size_t;
        auto empty() const -> bool;
        virtual void insert( const item &newitem ) = 0;
        virtual auto erase( const_iterator it ) -> iterator = 0;
        virtual void clear();
        // Will cause a debugmsg if there is not exactly one item at the location
        auto only_item() -> item &;

        // While iterators to colonies are stable, indexes are not.
        // These functions should only be used for serialization/deserialization
        auto get_iterator_from_pointer( item *it ) -> iterator;
        auto get_iterator_from_index( size_t idx ) -> iterator;
        auto get_index_from_iterator( const const_iterator &it ) -> size_t;

        auto begin() -> iterator;
        auto end() -> iterator;
        auto begin() const -> const_iterator;
        auto end() const -> const_iterator;
        auto rbegin() -> reverse_iterator;
        auto rend() -> reverse_iterator;
        auto rbegin() const -> const_reverse_iterator;
        auto rend() const -> const_reverse_iterator;

        /** Maximum number of items allowed here */
        virtual auto count_limit() const -> int = 0;
        /** Maximum volume allowed here */
        virtual auto max_volume() const -> units::volume = 0;
        /** Total volume of the items here */
        auto stored_volume() const -> units::volume;
        auto free_volume() const -> units::volume;
        /**
         * Returns how many of the specified item (or how many charges if it's counted by charges)
         * could be added without violating either the volume or itemcount limits.
         *
         * @returns Value of zero or greater for all items. For items counted by charges, it is always at
         * most it.charges.
         */
        auto amount_can_fit( const item &it ) const -> int;
        /** Return the item (or nullptr) that stacks with the argument */
        auto stacks_with( const item &it ) -> item *;
        auto stacks_with( const item &it ) const -> const item *;
};

#endif // CATA_SRC_ITEM_STACK_H
