#pragma once
#ifndef CATA_SRC_PICKUP_TOKEN_H
#define CATA_SRC_PICKUP_TOKEN_H

#include <list>
#include <vector>
#include "item_location.h"
#include "item_stack.h"
#include "optional.h"

class JsonIn;
class JsonOut;

using drop_location = std::pair<item_location, int>;
using drop_locations = std::list<drop_location>;

namespace pickup
{

/** Activity-associated item */
struct act_item {
    /// inventory item
    item_location loc;
    /// How many items need to be processed
    int count;
    /// Amount of moves that processing will consume
    int consumed_moves;

    act_item( const item_location &loc, int count, int consumed_moves )
        : loc( loc ),
          count( count ),
          consumed_moves( consumed_moves ) {}
};

struct pick_drop_selection {
    item_location target;
    cata::optional<int> quantity;
    std::vector<item_location> children;

    void serialize( JsonOut &jsout ) const;
    void deserialize( JsonIn &jin );
};

struct stacked_items {
    cata::optional<item_stack::iterator> parent;
    std::vector<std::list<item_stack::iterator>> stacked_children;
};

// TODO: This should get information on whether children are consecutive
/** Finds possible parent-child relations in picked up items to save moves */
std::vector<pick_drop_selection> optimize_pickup( const std::vector<item_location> &targets,
        const std::vector<int> &quantities );
std::list<act_item> reorder_for_dropping( Character &p, const drop_locations &drop );
std::list<item> obtain_and_tokenize_items( player &p, std::list<act_item> &items );
std::vector<item_location> extract_children( std::vector<item_location> &targets,
        item_location &stack_top );
std::vector<stacked_items> stack_for_pickup_ui( const
        std::vector<item_stack::iterator> &unstacked );
// TODO: This probably shouldn't return raw iterators
std::vector<std::list<item_stack::iterator>> flatten( const std::vector<stacked_items> &stacked );

} // namespace pickup

#endif // CATA_SRC_PICKUP_TOKEN_H
