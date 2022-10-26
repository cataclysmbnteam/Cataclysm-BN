#include "avatar.h"
#include "catch/catch.hpp"
#include "inventory_ui.h"
#include "output.h"

class debug_inventory_selector : public inventory_drop_selector
{
    public:
        debug_inventory_selector( player &p )
            : inventory_drop_selector( p )
        {}

        using inventory_drop_selector::get_raw_stats;
        using inventory_drop_selector::set_chosen_drop_count;
        using inventory_drop_selector::get_all_columns;

        excluded_stacks get_implied_drops() const {
            // Just to make it public
            return inventory_drop_selector::get_implied_drops();
        }

        const inventory_selector_preset &get_preset() {
            return preset;
        }
};

static void equip_clothing( player &p, const std::vector<itype_id> &clothing )
{
    for( const itype_id &c : clothing ) {
        const item article( c, calendar::start_of_cataclysm );
        p.wear_item( article );
    }
}

static void set_up_drop( player &u,
                         debug_inventory_selector &ui,
                         const itype_id &to_drop_type )
{
    const auto &worn = u.get_worn();
    const auto to_drop_iter = std::find_if( worn.begin(), worn.end(),
    [to_drop_type]( const item & it ) {
        return it.typeId() == to_drop_type;
    } );
    CAPTURE( to_drop_type.str() );
    REQUIRE( to_drop_iter != worn.end() );
    // TODO: Remove cast
    const item_location to_drop_loc( u, const_cast<item *>( &*to_drop_iter ) );

    bool did_select = ui.select( to_drop_loc );
    REQUIRE( did_select );
    const auto selected( ui.get_active_column().get_all_selected() );
    size_t num_dropped_items = 0;
    for( const auto &elem : selected ) {
        ui.set_chosen_drop_count( *elem, elem->get_available_count() );
        num_dropped_items += elem->chosen_count;
    }
    REQUIRE( num_dropped_items == 1 );
}

static void test_drop_time( player &u,
                            debug_inventory_selector &ui,
                            const itype_id &to_drop_type,
                            const std::string &expected_time_string )
{
    set_up_drop( u, ui, to_drop_type );
    inventory_selector::stats stats = ui.get_raw_stats();
    const std::string &time_stat = stats.back().at( 1 );

    THEN( "Adequate move cost is shown" ) {
        CAPTURE( stats.back() );
        CHECK( remove_color_tags( time_stat ) == expected_time_string );
    }
}

TEST_CASE( "expected move cost is displayed in drop ui", "[ui][drop_token]" )
{
    avatar u;
    debug_inventory_selector ui( u );

    GIVEN( "A character wearing a backpack and a duffelbag, with inventory full of bottles" ) {
        const std::vector<itype_id> clothing = {{
                itype_id( "backpack" ), itype_id( "duffelbag" )
            }
        };

        equip_clothing( u, clothing );

        item filler( itype_id( "bottle_glass" ) );
        size_t filler_count = 0;
        while( u.can_pick_volume( filler ) ) {
            u.i_add( filler );
            filler_count++;
        }
        CAPTURE( filler_count );

        ui.add_character_items( u );
        u.set_moves( 100 );
        REQUIRE( u.get_speed() == 100 );

        WHEN( "The character wants to drop a backpack" ) {
            test_drop_time( u, ui, itype_id( "backpack" ), "2.00s" );
        }

        WHEN( "The character wants to drop a duffel bag" ) {
            test_drop_time( u, ui, itype_id( "duffelbag" ), "3.00s" );
        }
    }
}

const auto true_fun = []( const inventory_entry & )
{
    return true;
};

const auto is_item = []( const inventory_entry &ie )
{
    return ie.is_item();
};

static void verify_has_entries( const debug_inventory_selector &ui, const item &filler )
{
    const itype_id filler_id = filler.typeId();
    size_t total_item_entry_count = 0;
    for( const inventory_column *col : ui.get_all_columns() ) {
        auto entries = col->get_entries( true_fun );
        for( auto &entry : entries ) {
            if( entry->is_item() && entry->any_item()->typeId() == filler_id ) {
                total_item_entry_count += entry->get_stack_size();
            }
        }
    }

    REQUIRE( total_item_entry_count > 0 );
}

static void test_drop_implications( player &u,
                                    debug_inventory_selector &ui,
                                    const item &pack,
                                    const item &filler )
{
    verify_has_entries( ui, filler );
    set_up_drop( u, ui, pack.typeId() );
    REQUIRE( ui.has_available_choices() );
    size_t expected_filler_count_min = pack.get_total_capacity() / filler.volume();
    size_t expected_filler_count_max = expected_filler_count_min + 1;
    size_t total_item_count = 0;
    u.visit_items( [&total_item_count]( const item * it ) {
        total_item_count += it->count();
        return VisitResponse::NEXT;
    } );
    CAPTURE( total_item_count );


    ui.set_filter( "" );
    ui.select_position( ui.get_selection_position() );

    THEN( "Expected number of bottles is predicted to be dropped" ) {
        const selection_column_base *selection = nullptr;
        for( const inventory_column *col : ui.get_all_columns() ) {
            // Horrible!
            const selection_column_base *cast_col = dynamic_cast<const selection_column_base *>( col );
            if( cast_col != nullptr ) {
                REQUIRE( selection == nullptr );
                selection = cast_col;
            }
        }

        REQUIRE( selection != nullptr );
        auto entries = selection->get_entries( is_item );

        REQUIRE( entries.size() > 0 );

        size_t actual_implied_filler_count = std::accumulate( entries.begin(), entries.end(), 0,
        [&filler]( int acc, const inventory_entry * ie ) {
            return acc + ( ie->any_item()->typeId() == filler.typeId() ? ie->chosen_count : 0 );
        } );

        CHECK( actual_implied_filler_count >= expected_filler_count_min );
        CHECK( actual_implied_filler_count <= expected_filler_count_max );
    }
}

TEST_CASE( "when dropping bags, the ui adds implied drops to 'selection' column",
           "[ui][drop_token]" )
{
    avatar u;
    debug_inventory_selector ui( u );

    GIVEN( "A character wearing a backpack and a duffelbag, with inventory full of bottles" ) {
        item backpack( itype_id( "backpack" ), calendar::start_of_cataclysm );
        item duffelbag( itype_id( "duffelbag" ), calendar::start_of_cataclysm );

        u.wear_item( backpack );
        u.wear_item( duffelbag );

        item filler( itype_id( "bottle_glass" ) );
        size_t filler_count = 0;
        while( u.can_pick_volume( filler ) ) {
            u.i_add( filler );
            filler_count++;
        }
        REQUIRE( filler_count > 0 );
        CAPTURE( filler_count );

        size_t filler_count_in_inv = 0;
        for( const auto &elem : u.inv.slice() ) {
            for( auto iter = elem->begin(); iter != elem->end(); iter++ ) {
                if( iter->typeId() == filler.typeId() ) {
                    filler_count_in_inv++;
                }
            }
        }
        REQUIRE( filler_count == filler_count_in_inv );

        ui.add_character_items( u );

        WHEN( "The character wants to drop a backpack" ) {
            test_drop_implications( u, ui, backpack, filler );
        }

        WHEN( "The character wants to drop a duffel bag" ) {
            test_drop_implications( u, ui, duffelbag, filler );
        }
    }
}

static void test_drop_colors( player &u,
                              debug_inventory_selector &ui,
                              const item &pack,
                              const item &filler )
{
    set_up_drop( u, ui, pack.typeId() );
    size_t expected_bottle_count_min = pack.get_total_capacity() / filler.volume();
    size_t expected_bottle_count_max = expected_bottle_count_min + 1;
    THEN( "Expected number of bottles is predicted to be dropped" ) {
        const selection_column_base *selection = nullptr;
        for( const inventory_column *col : ui.get_all_columns() ) {
            // Horrible!
            const selection_column_base *cast_col = dynamic_cast<const selection_column_base *>( col );
            if( cast_col != nullptr ) {
                selection = cast_col;
                break;
            }
        }

        REQUIRE( selection != nullptr );
        auto entries = selection->get_entries( []( const inventory_entry & ie ) {
            return ie.is_item();
        } );
        REQUIRE( entries.size() > 0 );

        size_t actual_bottle_count = std::accumulate( entries.begin(), entries.end(), 0,
        [&filler]( int acc, const inventory_entry * ie ) {
            return acc + ( ie->any_item()->typeId() == filler.typeId() ? ie->get_stack_size() : 0 );
        } );

        CHECK( actual_bottle_count >= expected_bottle_count_min );
        CHECK( actual_bottle_count <= expected_bottle_count_max );
    }
}

TEST_CASE( "when dropping bags, the ui colors implied drops in 'TODO: find good way to reference color here'",
           "[ui][drop_token][.]" )
{
    return;

    avatar u;
    debug_inventory_selector ui( u );

    GIVEN( "A character wearing a backpack and a duffelbag, with inventory full of bottles" ) {
        item backpack( itype_id( "backpack" ), calendar::start_of_cataclysm );
        item duffelbag( itype_id( "duffelbag" ), calendar::start_of_cataclysm );

        u.wear_item( backpack );
        u.wear_item( duffelbag );

        item filler( itype_id( "bottle_glass" ) );
        while( u.can_pick_volume( filler ) ) {
            u.i_add( filler );
        }

        ui.add_character_items( u );

        WHEN( "The character wants to drop a backpack" ) {
            test_drop_colors( u, ui, backpack, filler );
        }

        WHEN( "The character wants to drop a duffel bag" ) {
            test_drop_colors( u, ui, duffelbag, filler );
        }
    }
}
