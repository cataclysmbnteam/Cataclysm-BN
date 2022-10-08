#pragma once
#ifndef CATA_SRC_VISITABLE_H
#define CATA_SRC_VISITABLE_H

#include <climits>
#include <functional>
#include <list>
#include <string>
#include <vector>

#include "filter_utils.h"
#include "type_id.h"

class item;

enum class VisitResponse {
    ABORT, // Stop processing after this node
    NEXT,  // Descend vertically to any child nodes and then horizontally to next sibling
    SKIP   // Skip any child nodes and move directly to the next sibling
};

template <typename T>
class visitable
{
    public:
        /**
         * Traverses this object and any child items contained using a visitor pattern
         *
         * @param func visitor function called for each node which controls whether traversal continues.
         * The first argument is the node and the second is the parent node (if any)
         *
         * The visitor function should return VisitResponse::Next to recursively process child items,
         * VisitResponse::Skip to ignore children of the current node or VisitResponse::Abort to skip all remaining nodes
         *
         * @return This method itself only ever returns VisitResponse::Next or VisitResponse::Abort.
         */
        auto visit_items( const std::function<VisitResponse( item *, item * )> &func ) -> VisitResponse;
        auto visit_items( const std::function<VisitResponse( const item *, const item * )> &func )
        const -> VisitResponse;

        /** Lightweight version which provides only the current node */
        auto visit_items( const std::function<VisitResponse( item * )> &func ) -> VisitResponse;
        auto visit_items( const std::function<VisitResponse( const item * )> &func ) const -> VisitResponse;

        /**
         * Determine the immediate parent container (if any) for an item.
         * @param it item to search for which must be contained (at any depth) by this object
         * @return parent container or nullptr if the item is not within a container
         */
        auto find_parent( const item &it ) -> item *;
        auto find_parent( const item &it ) const -> const item *;

        /**
         * Returns vector of parent containers (if any) starting with the innermost
         * @param it item to search for which must be contained (at any depth) by this object
         */
        auto parents( const item &it ) -> std::vector<item *>;
        auto parents( const item &it ) const -> std::vector<const item *>;

        /** Returns true if this visitable instance contains the item */
        auto has_item( const item &it ) const -> bool;

        /** Returns true if any item (including those within a container) matches the filter */
        auto has_item_with( const std::function<bool( const item & )> &filter ) const -> bool;

        /** Returns true if instance has amount (or more) items of at least quality level */
        auto has_quality( const quality_id &qual, int level = 1, int qty = 1 ) const -> bool;

        /** Return maximum tool quality level provided by instance or INT_MIN if not found */
        auto max_quality( const quality_id &qual ) const -> int;

        /**
         * Count maximum available charges from this instance and any contained items
         * @param what ID of item to count charges of
         * @param limit stop searching after this many charges have been found
         * @param filter only count charges of items that match the filter
         * @param visitor is called when UPS charge is used (parameter is the charge itself)
         */
        auto charges_of( const itype_id &what, int limit = INT_MAX,
                        const std::function<bool( const item & )> &filter = return_true<item>,
                        std::function<void( int )> visitor = nullptr ) const -> int;

        /**
         * Count items matching id including both this instance and any contained items
         * @param what ID of items to count. "any" will count all items (usually used with a filter)
         * @param pseudo whether pseudo-items (from map/vehicle tiles, bionics etc) are considered
         * @param limit stop searching after this many matches
         * @param filter only count items that match the filter
         * @note items must be empty to be considered a match
         */
        auto amount_of( const itype_id &what, bool pseudo = true,
                       int limit = INT_MAX,
                       const std::function<bool( const item & )> &filter = return_true<item> ) const -> int;

        /** Check instance provides at least qty of an item (@see amount_of) */
        auto has_amount( const itype_id &what, int qty, bool pseudo = true,
                         const std::function<bool( const item & )> &filter = return_true<item> ) const -> bool;

        /** Returns all items (including those within a container) matching the filter */
        auto items_with( const std::function<bool( const item & )> &filter ) -> std::vector<item *>;
        auto items_with( const std::function<bool( const item & )> &filter ) const -> std::vector<const item *>;

        /**
         * Removes items contained by this instance which match the filter
         * @note if this instance itself is an item it will not be considered by the filter
         * @param filter a UnaryPredicate which should return true if the item is to be removed
         * @param count maximum number of items to if unspecified unlimited. A count of zero is a no-op
         * @return any items removed (items counted by charges are not guaranteed to be stacked)
         */
        auto remove_items_with( const std::function<bool( const item & )> &filter,
                                           int count = INT_MAX ) -> std::list<item>;

        /** Removes and returns the item which must be contained by this instance */
        auto remove_item( item &it ) -> item;
};

#endif // CATA_SRC_VISITABLE_H
