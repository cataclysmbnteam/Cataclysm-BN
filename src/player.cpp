#include "player.h"

#include <map>
#include <memory>
#include <string>

#include "avatar.h"
#include "character_display.h"
#include "character_martial_arts.h"
#include "enums.h"
#include "flag.h"
#include "game.h"
#include "item.h"
#include "martialarts.h"
#include "messages.h"
#include "output.h"
#include "string_formatter.h"
#include "string_id.h"
#include "translations.h"
#include "ui.h"
#include "visitable.h"
#include "vitamin.h"

static const bionic_id bio_cqb( "bio_cqb" );

player::player()
{
    str_cur = 8;
    str_max = 8;
    dex_cur = 8;
    dex_max = 8;
    int_cur = 8;
    int_max = 8;
    per_cur = 8;
    per_max = 8;
    dodges_left = 1;
    blocks_left = 1;
    set_power_level( 0_kJ );
    set_max_power_level( 0_kJ );
    scent = 500;
    male = true;
    remove_primary_weapon();

    start_location = start_location_id( "sloc_shelter" );
    moves = 100;
    movecounter = 0;
    oxygen = 0;
    in_vehicle = false;
    controlling_vehicle = false;
    grab_point = tripoint_zero;
    hauling = false;
    focus_pool = 100;
    last_item = itype_id( "null" );
    sight_max = 9999;
    last_batch = 0;
    lastconsumed = itype_id( "null" );
    death_drops = true;

    volume = 0;

    set_value( "THIEF_MODE", "THIEF_ASK" );

    for( const auto &v : vitamin::all() ) {
        vitamin_levels[ v.first ] = 0;
    }

    if( g != nullptr && json_flag::is_ready() && get_anatomy().is_valid() ) {
        // TODO: Remove the set_body here
        set_body();
        recalc_sight_limits();
        reset_encumbrance();
    }
}

player::~player() = default;
player::player( player && )  noexcept = default;
player &player::operator=( player && )  noexcept = default;

detached_ptr<item> player::reduce_charges( int position, int quantity )
{
    item &it = i_at( position );
    if( it.is_null() ) {
        debugmsg( "invalid item position %d for reduce_charges", position );
        return detached_ptr<item>();
    }
    if( it.charges <= quantity ) {
        return i_rem( position );
    }
    it.mod_charges( -quantity );

    auto taken = item::spawn( it );
    taken->charges = quantity;
    return taken;
}

detached_ptr<item> player::reduce_charges( item *it, int quantity )
{
    if( !has_item( *it ) ) {
        debugmsg( "invalid item (name %s) for reduce_charges", it->tname() );
        return detached_ptr<item>();
    }
    if( it->charges <= quantity ) {
        return it->detach();
    }
    it->mod_charges( -quantity );

    auto taken = item::spawn( *it );
    taken->charges = quantity;
    return taken;
}

// ids of martial art styles that are available with the bio_cqb bionic.
static const std::vector<matype_id> bio_cqb_styles{ {
        matype_id{ "style_aikido" },
        matype_id{ "style_biojutsu" },
        matype_id{ "style_boxing" },
        matype_id{ "style_capoeira" },
        matype_id{ "style_crane" },
        matype_id{ "style_dragon" },
        matype_id{ "style_judo" },
        matype_id{ "style_karate" },
        matype_id{ "style_krav_maga" },
        matype_id{ "style_leopard" },
        matype_id{ "style_muay_thai" },
        matype_id{ "style_ninjutsu" },
        matype_id{ "style_pankration" },
        matype_id{ "style_snake" },
        matype_id{ "style_taekwondo" },
        matype_id{ "style_tai_chi" },
        matype_id{ "style_tiger" },
        matype_id{ "style_wingchun" },
        matype_id{ "style_zui_quan" }
    }};

bool character_martial_arts::pick_style( const avatar &you )    // Style selection menu
{
    enum style_selection {
        KEEP_HANDS_FREE = 0,
        STYLE_OFFSET
    };

    // If there are style already, cursor starts there
    // if no selected styles, cursor starts from no-style

    // Any other keys quit the menu
    const std::vector<matype_id> &selectable_styles = you.has_active_bionic(
                bio_cqb ) ? bio_cqb_styles :
            ma_styles;

    input_context ctxt( "MELEE_STYLE_PICKER" );
    ctxt.register_action( "SHOW_DESCRIPTION" );

    uilist kmenu;
    kmenu.text = string_format( _( "Select a style.\n"
                                   "\n"
                                   "STR: <color_white>%d</color>, DEX: <color_white>%d</color>, "
                                   "PER: <color_white>%d</color>, INT: <color_white>%d</color>\n"
                                   "Base empty-handed damage: %3d\n"
                                   "Effective dodge rating: %4.1f\n"
                                   "Press [<color_yellow>%s</color>] for more info.\n" ),
                                you.get_str(), you.get_dex(), you.get_per(), you.get_int(),
                                character_display::display_empty_handed_base_damage( you ), you.get_dodge(),
                                ctxt.get_desc( "SHOW_DESCRIPTION" ) );
    ma_style_callback callback( static_cast<size_t>( STYLE_OFFSET ), selectable_styles );
    kmenu.callback = &callback;
    kmenu.input_category = "MELEE_STYLE_PICKER";
    kmenu.additional_actions.emplace_back( "SHOW_DESCRIPTION", translation() );
    kmenu.desc_enabled = true;
    kmenu.addentry_desc( KEEP_HANDS_FREE, true, 'h',
                         keep_hands_free ? _( "Keep hands free (on)" ) : _( "Keep hands free (off)" ),
                         _( "When this is enabled, player won't wield things unless explicitly told to." ) );

    kmenu.selected = STYLE_OFFSET;

    for( size_t i = 0; i < selectable_styles.size(); i++ ) {
        auto &style = selectable_styles[i].obj();
        //Check if this style is currently selected
        const bool selected = selectable_styles[i] == style_selected;
        std::string entry_text = style.name.translated();
        if( selected ) {
            kmenu.selected = i + STYLE_OFFSET;
            entry_text = colorize( entry_text, c_pink );
        }
        kmenu.addentry_desc( i + STYLE_OFFSET, true, -1, entry_text, style.description.translated() );
    }

    kmenu.query();
    int selection = kmenu.ret;

    if( selection >= STYLE_OFFSET ) {
        style_selected = selectable_styles[selection - STYLE_OFFSET];
        martialart_use_message( you );
    } else if( selection == KEEP_HANDS_FREE ) {
        keep_hands_free = !keep_hands_free;
    } else {
        return false;
    }

    return true;
}

nc_color encumb_color( int level )
{
    if( level < 0 ) {
        return c_green;
    }
    if( level < 10 ) {
        return c_light_gray;
    }
    if( level < 40 ) {
        return c_yellow;
    }
    if( level < 70 ) {
        return c_light_red;
    }
    return c_red;
}

//message related stuff
void player::add_msg_if_player( const std::string &msg ) const
{
    Messages::add_msg( msg );
}

void player::add_msg_player_or_npc( const std::string &player_msg,
                                    const std::string &/*npc_msg*/ ) const
{
    Messages::add_msg( player_msg );
}

void player::add_msg_if_player( const game_message_params &params,
                                const std::string &msg ) const
{
    Messages::add_msg( params, msg );
}

void player::add_msg_player_or_npc( const game_message_params &params,
                                    const std::string &player_msg,
                                    const std::string &/*npc_msg*/ ) const
{
    Messages::add_msg( params, player_msg );
}

void player::add_msg_player_or_say( const std::string &player_msg,
                                    const std::string &/*npc_speech*/ ) const
{
    Messages::add_msg( player_msg );
}

void player::add_msg_player_or_say( const game_message_params &params,
                                    const std::string &player_msg,
                                    const std::string &/*npc_speech*/ ) const
{
    Messages::add_msg( params, player_msg );
}

bool player::query_yn( const std::string &mes ) const
{
    return ::query_yn( mes );
}
