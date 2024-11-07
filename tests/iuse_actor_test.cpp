#include "catch/catch.hpp"

#include <climits>
#include <list>
#include <memory>

#include "avatar.h"
#include "game.h"
#include "inventory.h"
#include "item.h"
#include "item_factory.h"
#include "itype.h"
#include "iuse_actor.h"
#include "map.h"
#include "map_selector.h"
#include "material.h"
#include "monster.h"
#include "mtype.h"
#include "player.h"
#include "player_helpers.h"
#include "point.h"
#include "state_helpers.h"
#include "string_id.h"
#include "type_id.h"

static monster *find_adjacent_monster( const tripoint &pos )
{
    tripoint target = pos;
    for( target.x = pos.x - 1; target.x <= pos.x + 1; target.x++ ) {
        for( target.y = pos.y - 1; target.y <= pos.y + 1; target.y++ ) {
            if( target == pos ) {
                continue;
            }
            if( monster *const candidate = g->critter_at<monster>( target ) ) {
                return candidate;
            }
        }
    }
    return nullptr;
}

TEST_CASE( "manhack", "[iuse_actor][manhack]" )
{
    clear_all_state();
    player &dummy = g->u;

    g->clear_zombies();
    detached_ptr<item> det = item::spawn( "bot_manhack", calendar::start_of_cataclysm,
                                          item::default_charges_tag{} );
    item &test_item = *det;
    dummy.i_add( std::move( det ) );

    int test_item_pos = dummy.inv_position_by_item( &test_item );
    REQUIRE( test_item_pos != INT_MIN );

    monster *new_manhack = find_adjacent_monster( dummy.pos() );
    REQUIRE( new_manhack == nullptr );

    dummy.invoke_item( &test_item );

    test_item_pos = dummy.inv_position_by_item( &test_item );
    REQUIRE( test_item_pos == INT_MIN );

    new_manhack = find_adjacent_monster( dummy.pos() );
    REQUIRE( new_manhack != nullptr );
    REQUIRE( new_manhack->type->id == mtype_id( "mon_manhack" ) );
    g->clear_zombies();
}

namespace
{

auto cut_up_yields( const std::string &target ) -> void
{
    map &here = get_map();
    player &you = get_avatar();
    clear_avatar();
    // Nominal dex to avoid yield penalty.
    you.dex_cur = 12;
    //guy.set_skill_level( skill_id( "fabrication" ), 10 );
    here.i_at( you.pos() ).clear();

    CAPTURE( target );
    salvage_actor test_actor;
    detached_ptr<item> cut_up_target = item::spawn( target );
    detached_ptr<item> tool = item::spawn( "knife_butcher" );

    const std::vector<material_id> &target_materials = cut_up_target->made_of();
    units::mass smallest_yield_mass = units::mass_max;
    for( const material_id &mater : target_materials ) {
        if( const std::optional<itype_id> item_id = mater->salvaged_into() ) {
            smallest_yield_mass = std::min( smallest_yield_mass, item_id->obj().weight );
        }
    }
    REQUIRE( smallest_yield_mass != units::mass_max );

    units::mass cut_up_target_mass = cut_up_target->weight();
    item &item_to_cut = *cut_up_target;
    here.add_item_or_charges( you.pos(), std::move( cut_up_target ) );

    REQUIRE( smallest_yield_mass <= cut_up_target_mass );

    test_actor.cut_up( you, *tool, item_to_cut );

    map_stack salvaged_items = here.i_at( you.pos() );
    const units::mass salvaged_mass = std::accumulate( salvaged_items.begin(), salvaged_items.end(),
    0_gram, []( const units::mass acc, const item * salvage ) {
        return acc + salvage->weight();
    } );

    CHECK( salvaged_mass <= cut_up_target_mass );
    CHECK( salvaged_mass >= ( cut_up_target_mass * 0.99 ) - smallest_yield_mass );
}
} // namespace

TEST_CASE( "cut_up_yields" )
{
    cut_up_yields( "blanket" );
    cut_up_yields( "backpack_hiking" );
    cut_up_yields( "boxpack" );
    cut_up_yields( "case_violin" );
    cut_up_yields( "down_mattress" );
    cut_up_yields( "plastic_boat_hull" );
    cut_up_yields( "bunker_coat" );
    cut_up_yields( "bunker_pants" );
    cut_up_yields( "kevlar" );
    cut_up_yields( "touring_suit" );
    cut_up_yields( "dress_wedding" );
    cut_up_yields( "wetsuit" );
    cut_up_yields( "wetsuit_booties" );
    cut_up_yields( "wetsuit_hood" );
    cut_up_yields( "wetsuit_spring" );
    cut_up_yields( "peacoat" );
    cut_up_yields( "log" );
    cut_up_yields( "stick" );
    cut_up_yields( "stick_long" );
    cut_up_yields( "tazer" );
    cut_up_yields( "control_laptop" );
    cut_up_yields( "voltmeter" );
    cut_up_yields( "eink_tablet_pc" );
    cut_up_yields( "camera" );
    cut_up_yields( "cell_phone" );
    cut_up_yields( "laptop" );
    cut_up_yields( "radio" );
    cut_up_yields( "under_armor" );
    cut_up_yields( "acidchitin_armor_small_quadruped" );
    cut_up_yields( "chitin_armor_small_quadruped" );
    cut_up_yields( "leather_armor_small_quadruped" );
    cut_up_yields( "leatherbone_armor_small_quadruped" );
    cut_up_yields( "kevlar_armor_small_quadruped" );
    cut_up_yields( "rubber_armor_small_quadruped" );
}

TEST_CASE( "cut_up_yields_all", "[.]" )
{
    int i = 0;
    for( const itype *itp : item_controller->all() ) {
        i++;
        SECTION( string_format( "Item number %d", i ) ) {
            cut_up_yields( itp->get_id().str() );
        }
    }
}
