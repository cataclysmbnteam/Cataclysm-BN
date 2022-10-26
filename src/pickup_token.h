#pragma once
#ifndef CATA_SRC_PICKUP_TOKEN_H
#define CATA_SRC_PICKUP_TOKEN_H

#include <list>
#include <vector>
#include "item_handling_util.h"
#include "item_location.h"
#include "item_stack.h"
#include "optional.h"

class JsonIn;
class JsonOut;
template<typename T> struct enum_traits;

namespace pickup
{

/** Activity-associated item */
template <class ItemLocation = class item_location>
struct act_item_t {
    /// inventory item
    ItemLocation loc;
    /// How many items need to be processed
    int count = 0;
    /// Amount of moves that processing will consume
    int consumed_moves = 0;

    act_item_t() = default;
    act_item_t( const ItemLocation &loc, int count, int consumed_moves )
        : loc( loc )
        , count( count )
        , consumed_moves( consumed_moves ) {}

    void serialize( JsonOut &jsout ) const;
    void deserialize( JsonIn &jsin );
};

enum class drop_intent {
    none = 0,
    specific,
    implied,
    last
};

using act_item = act_item_t<item_location>;
using const_act_item = act_item_t<const_item_location>;

struct pick_drop_selection {
    item_location target;
    cata::optional<int> quantity;
    std::vector<item_location> children;
    drop_intent intent = drop_intent::specific;

    void serialize( JsonOut &jsout ) const;
    void deserialize( JsonIn &jin );
};

struct drop_selection {
    pick_drop_selection selection;
    drop_intent intent;
};

struct stacked_items {
    cata::optional<item_stack::iterator> parent;
    std::vector<std::list<item_stack::iterator>> stacked_children;
};

struct nonconst {
};

// TODO: This should get information on whether children are consecutive
/** Finds possible parent-child relations in picked up items to save moves */
std::vector<pick_drop_selection> optimize_pickup( const std::vector<item_location> &targets,
        const std::vector<int> &quantities );
std::list<act_item> reorder_for_dropping( Character &p, const drop_locations &drop, nonconst );
std::list<const_act_item> reorder_for_dropping( const Character &p, const drop_locations &drop );
std::list<item> obtain_and_tokenize_items( player &p, std::list<act_item> &items );
std::vector<stacked_items> stack_for_pickup_ui( const
        std::vector<item_stack::iterator> &unstacked );
// TODO: This probably shouldn't return raw iterators
std::vector<std::list<item_stack::iterator>> flatten( const std::vector<stacked_items> &stacked );

} // namespace pickup

template<>
struct enum_traits<pickup::drop_intent> {
    static constexpr pickup::drop_intent last = pickup::drop_intent::last;
};

#endif // CATA_SRC_PICKUP_TOKEN_H
