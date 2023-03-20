#include "gamemode_tutorial.h" // IWYU pragma: associated

#include <array>
#include <cstdlib>
#include <memory>
#include <optional>
#include <string>

#include "action.h"
#include "avatar.h"
#include "calendar.h"
#include "character_functions.h"
#include "clzones.h"
#include "coordinate_conversions.h"
#include "debug.h"
#include "game.h"
#include "game_constants.h"
#include "int_id.h"
#include "inventory.h"
#include "item.h"
#include "map.h"
#include "map_iterator.h"
#include "mapdata.h"
#include "output.h"
#include "overmap.h"
#include "overmapbuffer.h"
#include "pldata.h"
#include "point.h"
#include "profession.h"
#include "scent_map.h"
#include "string_utils.h"
#include "text_snippets.h"
#include "translations.h"
#include "trap.h"
#include "type_id.h"
#include "units.h"
#include "weather.h"

static const zone_type_id zone_TUT_MARKER( "TUT_MARKER" );

static void fill_om_layer_with_tile( overmap &om, int z, const oter_id &tile )
{
    for( int x = 0; x < OMAPX; x++ ) {
        for( int y = 0; y < OMAPY; y++ ) {
            tripoint_om_omt p( x, y, z );
            om.ter_set( p, tile );
            om.seen( p ) = true;
        }
    }
}

static tripoint_abs_omt build_tutorial_overmap()
{
    // Place tutorial room at 'lp' coords within the zero overmap
    const tripoint_om_omt lp( 50, 50, 0 );
    const tripoint_abs_omt lp_abs = project_combine( point_abs_om(), lp );
    overmap &om = overmap_buffer.get( point_abs_om() );

    // Clear zero overmap to avoid obstacles
    const oter_id empty_rock( "empty_rock" );
    const oter_id field( "field" );
    const oter_id open_air( "open_air" );
    for( int z = -OVERMAP_DEPTH; z < 0; z++ ) {
        fill_om_layer_with_tile( om, z, empty_rock );
    }
    fill_om_layer_with_tile( om, 0, field );
    for( int z = 1; z <= OVERMAP_HEIGHT; z++ ) {
        fill_om_layer_with_tile( om, z, open_air );
    }

    // Surround with impenetrable wall to avoid spoilers
    for( int z = -OVERMAP_DEPTH; z <= OVERMAP_HEIGHT; z++ ) {
        om.place_special_forced(
            overmap_special_id( "world" ),
            tripoint_om_omt( 0, 0, z ),
            om_direction::type::north
        );
    }

    // Place the tutorial complex
    om.place_special_forced( overmap_special_id( "tutorial_complex" ), lp, om_direction::type::north );

    // Ensure nothing nasty interferes
    om.clear_mon_groups();

    return lp_abs;
}

void tutorial_game::update_tutorial_msg()
{
    const zone_manager &zones = zone_manager::get_manager();
    const zone_data *zone = zones.get_zone_at(
                                get_map().getabs( get_avatar().pos() ),
                                zone_TUT_MARKER
                            );

    if( !zone ) {
        return;
    }

    add_msg( m_mixed, string_format( "Zone found: %s.", zone->get_name() ) );
    std::vector<std::string> messages = string_split( zone->get_name(), ';' );
    for( const std::string &message : messages ) {
        add_message( snippet_id( message ) );
    }
}

bool tutorial_game::init()
{
    tutorials_seen_new.clear();

    // Start at noon
    calendar::turn = calendar::turn_zero + 12_hours;

    avatar &you = get_avatar();
    character_funcs::normalize( you );
    //~ default name for the tutorial
    you.name = _( "John Smith" );
    you.prof = profession::generic();

    const tripoint_abs_omt lp_abs = build_tutorial_overmap();

    // Place character on specific spot in the tutorial room
    g->load_map( project_to<coords::sm>( lp_abs ) );
    you.setx( 3 );
    you.sety( 3 );

    // Ensure the game will skip a turn on start and trigger gamemode's per-turn check
    you.moves = 0;

    // This shifts the map to center on the player
    g->update_map( you );

    return true;
}

void tutorial_game::per_turn()
{
    update_tutorial_msg();
}

void tutorial_game::pre_action( action_id &act )
{
    switch( act ) {
        case ACTION_SAVE:
        case ACTION_QUICKSAVE:
            popup( _( "You're saving a tutorial - the tutorial world lacks certain features of normal worlds.  "
                      "Weird things might happen when you load this save.  You have been warned." ) );
            break;
        default:
            // Other actions are fine.
            break;
    }
}

void tutorial_game::post_action( action_id /*act*/ )
{
    // TODO: remove dead code
}

void tutorial_game::add_message( const snippet_id &lesson_id )
{
    if( tutorials_seen_new.count( lesson_id ) > 0 ) {
        return;
    }
    tutorials_seen_new.insert( lesson_id );

    std::string text = SNIPPET.get_snippet_by_id( lesson_id ).value_or( translation() ).translated();

    text = replace_keybind_tags( text );

    popup( text, PF_ON_TOP );
    add_msg( m_info, text );
}
