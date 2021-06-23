#include <algorithm>
#include <climits>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "activity_handlers.h"
#include "avatar.h"
#include "calendar.h"
#include "catch/catch.hpp"
#include "drop_token.h"
#include "item.h"
#include "player_helpers.h"

class inventory;
struct act_item;

std::list<act_item> reorder_for_dropping( Character &p, const drop_locations &drop );
std::list<item> obtain_and_tokenize_items( player &p, std::list<act_item> &items );

std::ostream &operator<<( std::ostream &os, const item_drop_token &dt )
{
    os << "{turn:" << to_turn<int>( dt.turn )
       << ", drop:" << dt.drop_number
       << ", parent:" << dt.parent_number
       << '}';
    return os;
}

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

TEST_CASE( "full backpack drop", "[activity][drop_token]" )
{
    avatar dummy;
    item an_item( "bottle_glass" );
    item backpack( "backpack" );
    item duffel_bag( "duffelbag" );

    // Filling item and container types can be freely changed, as long as they meet this:
    REQUIRE( backpack.get_storage() % an_item.volume() == 0_ml );
    REQUIRE( backpack.get_storage() / an_item.volume() > 1 );
    REQUIRE( duffel_bag.get_storage() % an_item.volume() > 0_ml );
    REQUIRE( duffel_bag.get_storage() / an_item.volume() > 1 );

    GIVEN( "a character with a backpack full of items and no other containers" ) {
        REQUIRE( dummy.wear_item( backpack, false ) );
        while( dummy.can_pickWeight( an_item, true ) && dummy.can_pickVolume( an_item ) ) {
            dummy.i_add( an_item );
        }

        REQUIRE( dummy.worn.size() == 1 );
        REQUIRE( dummy.inv.size() >= 1 );

        WHEN( "he considers dropping the backpack" ) {
            drop_locations drop;
            drop.push_back( drop_location( item_location( dummy, &dummy.worn.front() ), 1 ) );
            std::list<act_item> drop_list = reorder_for_dropping( dummy, drop );
            THEN( "he will try to drop all carried items" ) {
                // TODO: Check that all items will be dropped. inv.size() doesn't work because stacks
                AND_THEN( "all of them will have identical drop tokens, marking the backpack as parent" ) {
                    item_drop_token first_token = *drop_list.front().loc->drop_token;
                    for( const act_item &ait : drop_list ) {
                        CHECK( *ait.loc->drop_token == first_token );
                    }
                }
                AND_THEN( "the backpack will have non-zero drop cost, while all contents will have zero drop cost" ) {
                    REQUIRE( drop_list.front().loc->typeId() == backpack.typeId() );
                    auto iter = drop_list.begin();
                    CHECK( iter->consumed_moves > 0 );
                    iter++;
                    while( iter != drop_list.end() ) {
                        CHECK( iter->consumed_moves == 0 );
                        iter++;
                    }
                }
                AND_THEN( "total volume dropped will equal volume of backpack and all items in inventory" ) {
                    units::volume total_dropped = std::accumulate( drop_list.begin(), drop_list.end(), 0_ml,
                    []( units::volume acc, const act_item & ait ) {
                        return acc + ait.loc->volume();
                    } );
                    units::volume inventory_volume = dummy.volume_carried();
                    units::volume worn_volume = std::accumulate( dummy.worn.begin(), dummy.worn.end(), 0_ml,
                    []( units::volume acc, const item & it ) {
                        return acc + it.volume();
                    } );
                    CHECK( total_dropped == inventory_volume + worn_volume );
                }
            }
        }
    }

    GIVEN( "a character with two duffel bags and two backpacks full of items and no other containers" ) {
        REQUIRE( dummy.wear_item( duffel_bag, false ) );
        REQUIRE( dummy.wear_item( duffel_bag, false ) );
        REQUIRE( dummy.wear_item( backpack, false ) );
        REQUIRE( dummy.wear_item( backpack, false ) );
        while( dummy.can_pickWeight( an_item, true ) && dummy.can_pickVolume( an_item ) ) {
            dummy.i_add( an_item );
        }

        REQUIRE( dummy.worn.size() == 4 );
        REQUIRE( dummy.inv.size() >= 1 );

        WHEN( "he considers dropping one duffel bag and one backpack" ) {
            drop_locations drop;
            auto first_duffel_iter = std::find_if( dummy.worn.begin(), dummy.worn.end(),
            [&]( const item & it ) {
                return it.typeId() == duffel_bag.typeId();
            } );
            auto first_backpack_iter = std::find_if( dummy.worn.begin(), dummy.worn.end(),
            [&]( const item & it ) {
                return it.typeId() == backpack.typeId();
            } );
            REQUIRE( first_duffel_iter != dummy.worn.end() );
            REQUIRE( first_backpack_iter != dummy.worn.end() );
            drop.push_back( drop_location( item_location( dummy, &*first_duffel_iter ), 1 ) );
            drop.push_back( drop_location( item_location( dummy, &*first_backpack_iter ), 1 ) );
            std::list<act_item> drop_list = reorder_for_dropping( dummy, drop );
            THEN( "he will try to drop some, but not all of the carried items" ) {
                REQUIRE( drop_list.size() > 4 );

                units::volume total_dropped = std::accumulate( drop_list.begin(), drop_list.end(), 0_ml,
                []( units::volume acc, const act_item & ait ) {
                    return acc + ait.loc->volume();
                } );
                units::volume inventory_volume = dummy.volume_carried();
                units::volume worn_dropped_volume = first_duffel_iter->volume() + first_backpack_iter->volume();
                CHECK( total_dropped - worn_dropped_volume < inventory_volume );

                AND_THEN( "dropped duffel bag and backpack will have non-zero drop cost, while all contents will have zero drop cost" ) {
                    auto drop_duffel_iter = std::find_if( drop_list.begin(), drop_list.end(),
                    [&]( const act_item & ait ) {
                        return ait.loc->typeId() == duffel_bag.typeId();
                    } );
                    auto drop_backpack_iter = std::find_if( drop_list.begin(), drop_list.end(),
                    [&]( const act_item & ait ) {
                        return ait.loc->typeId() == backpack.typeId();
                    } );
                    REQUIRE( drop_duffel_iter != drop_list.end() );
                    REQUIRE( drop_backpack_iter != drop_list.end() );
                    for( auto iter = drop_list.begin(); iter != drop_list.end(); iter++ ) {
                        if( iter == drop_duffel_iter || iter == drop_backpack_iter ) {
                            CHECK( iter->consumed_moves > 0 );
                        } else {
                            CHECK( iter->consumed_moves == 0 );
                        }
                    }

                    AND_THEN( "both containers will be followed by enough same-drop-token items to fill them" ) {
                        const int expected_duffel_content_count = duffel_bag.get_total_capacity() / an_item.volume();
                        const int expected_backpack_content_count = backpack.get_total_capacity() / an_item.volume();

                        int actual_duffel_content_count = 0;
                        for( auto iter = std::next( drop_duffel_iter ); iter != drop_list.end(); iter++ ) {
                            if( iter->consumed_moves == 0 && iter->loc->typeId() == an_item.typeId() ) {
                                actual_duffel_content_count++;
                            } else {
                                break;
                            }
                        }

                        int actual_backpack_content_count = 0;
                        for( auto iter = std::next( drop_backpack_iter ); iter != drop_list.end(); iter++ ) {
                            if( iter->consumed_moves == 0 && iter->loc->typeId() == an_item.typeId() ) {
                                actual_backpack_content_count++;
                            } else {
                                break;
                            }
                        }

                        // +1 because we aren't checking tokens, but the last item is still zero cost
                        CHECK( actual_duffel_content_count >= expected_duffel_content_count );
                        CHECK( actual_duffel_content_count <= expected_duffel_content_count + 1 );
                        CHECK( actual_backpack_content_count >= expected_backpack_content_count );
                        CHECK( actual_backpack_content_count <= expected_backpack_content_count + 1 );

                        AND_THEN( "when actually dropping items, each container will be followed by enough items of same token to fill it, "
                                  "followed by one un-contained item" ) {
                            dummy.set_moves( 1000 );
                            std::list<item> tokenized_dropped = obtain_and_tokenize_items( dummy, drop_list );
                            auto tokenized_duffel_iter = std::find_if( tokenized_dropped.begin(), tokenized_dropped.end(),
                            [&]( const item & it ) {
                                return it.typeId() == duffel_bag.typeId();
                            } );
                            auto tokenized_backpack_iter = std::find_if( tokenized_dropped.begin(), tokenized_dropped.end(),
                            [&]( const item & it ) {
                                return it.typeId() == backpack.typeId();
                            } );
                            REQUIRE( tokenized_duffel_iter != tokenized_dropped.end() );
                            REQUIRE( tokenized_backpack_iter != tokenized_dropped.end() );
                            REQUIRE( *tokenized_duffel_iter->drop_token != *tokenized_backpack_iter->drop_token );
                            int actual_duffel_tokens = std::count_if( tokenized_dropped.begin(), tokenized_dropped.end(),
                            [&]( const item & it ) {
                                return it.drop_token == tokenized_duffel_iter->drop_token;
                            } );
                            int actual_backpack_tokens = std::count_if( tokenized_dropped.begin(), tokenized_dropped.end(),
                            [&]( const item & it ) {
                                return it.drop_token == tokenized_backpack_iter->drop_token;
                            } );
                            const int expected_duffel_token_count = 1 + duffel_bag.get_total_capacity() / an_item.volume();
                            const int expected_backpack_token_count = 1 + backpack.get_total_capacity() / an_item.volume();
                            CHECK( actual_duffel_tokens == expected_duffel_token_count );
                            CHECK( actual_backpack_tokens == expected_backpack_token_count );
                            int all_items = tokenized_dropped.size();
                            CHECK( all_items == actual_duffel_tokens + actual_backpack_tokens + 1 );
                        }
                    }
                }
            }
        }
    }

    GIVEN( "a character with two duffel bags full of items" ) {
        REQUIRE( dummy.wear_item( duffel_bag, false ) );
        REQUIRE( dummy.wear_item( duffel_bag, false ) );
        while( dummy.can_pickWeight( an_item, true ) && dummy.can_pickVolume( an_item ) ) {
            dummy.i_add( an_item );
        }

        WHEN( "he considers dropping only one of the bags, but all of the items" ) {
            drop_locations drop;
            drop.push_back( drop_location( item_location( dummy, &dummy.worn.front() ), 1 ) );
            std::vector<item *> dump;
            dummy.inv.dump( dump );
            for( item *it : dump ) {
                drop.push_back( drop_location( item_location( dummy, it ), 1 ) );
            }

            std::list<act_item> drop_list = reorder_for_dropping( dummy, drop );
            THEN( "at most half of the non-bag items will have zero drop cost" ) {
                const size_t actual_zero_cost = std::count_if( drop_list.begin(), drop_list.end(),
                [&]( const act_item & ait ) {
                    return ait.consumed_moves == 0;
                } );
                const size_t expected_zero_cost = ( drop_list.size() - 1 ) / 2;
                CHECK( actual_zero_cost == expected_zero_cost );
            }
        }
    }
}
