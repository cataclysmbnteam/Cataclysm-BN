#include "player.h" // IWYU pragma: associated

#include <algorithm> //std::min
#include <cstddef>
#include <memory>
#include <unordered_map>

#include "enums.h"
#include "input.h"
#include "inventory.h"
#include "mutation.h"
#include "output.h"
#include "popup.h"
#include "string_formatter.h"
#include "string_id.h"
#include "translations.h"
#include "ui_manager.h"
#include "value_ptr.h"

const invlet_wrapper
mutation_chars( "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\"#&()*+./:;@[\\]^_{|}!=" );

static void draw_exam_window( const catacurses::window &win, const int border_y )
{
    const int width = getmaxx( win );
    mvwputch( win, point( 0, border_y ), BORDER_COLOR, LINE_XXXO );
    mvwhline( win, point( 1, border_y ), LINE_OXOX, width - 2 );
    mvwputch( win, point( width - 1, border_y ), BORDER_COLOR, LINE_XOXX );
}

const auto shortcut_desc = []( const std::string &comment, const std::string &keys )
{
    return string_format( comment, string_format( "[<color_yellow>%s</color>]", keys ) );
};

enum class mutation_menu_mode {
    activating,
    reassigning,
};
enum class mutation_tab_mode {
    active,
    passive
};

static void show_mutations_titlebar( const catacurses::window &window,
                                     const mutation_menu_mode menu_mode,
                                     cata::optional<trait_id> &reassigning_id,
                                     const input_context &ctxt )
{
    werase( window );
    std::string desc;
    if( menu_mode == mutation_menu_mode::reassigning && !reassigning_id ) {
        desc += std::string( _( "Reassigning." ) ) + "  " +
                _( "Select a mutation to reassign. " );
    } else if( menu_mode == mutation_menu_mode::reassigning && reassigning_id ) {
        desc += string_format( _( "Reassigning %s.  Press a key to assign it to." ),
                               colorize( reassigning_id->obj().name(), c_blue ) );
    } else if( menu_mode == mutation_menu_mode::activating ) {
        desc += colorize( _( "Activating " ), c_green );
    }
    if( menu_mode != mutation_menu_mode::reassigning ) {
        desc += shortcut_desc( _( "%s to reassign invlet, " ), ctxt.get_desc( "REASSIGN" ) );
    }
    desc += shortcut_desc( _( "%s to change keybindings." ), ctxt.get_desc( "HELP_KEYBINDINGS" ) );
    // NOLINTNEXTLINE(cata-use-named-point-constants)
    fold_and_print( window, point( 1, 0 ), getmaxx( window ) - 1, c_white, desc );
    wnoutrefresh( window );
}

namespace mutations
{

void power_mutations( Character &c )
{
    if( !c.is_player() ) {
        // TODO: Implement NPCs activating mutations
        return;
    }

    // Mutation selection UI obscures the terrain,
    // but some mutations may ask to select a tile when activated.
    // As such, we must (de-)activate only after destroying the UI.
    power_mut_ui_result res = mutations::power_mutations_ui( c );

    switch( res.cmd ) {
        case mutations::power_mut_ui_cmd::Exit:
            break;
        case mutations::power_mut_ui_cmd::Deactivate:
            c.deactivate_mutation( res.mut );
            break;
        case mutations::power_mut_ui_cmd::Activate:
            c.activate_mutation( res.mut );
            break;
    }
}


static cata::optional<power_mut_ui_result> try_activate_mutation( const mutation_branch &mut_data,
        Character &c )
{
    const trait_id &mut_id = mut_data.id;
    const cata::value_ptr<mut_transform> &trans = mut_data.transform;
    if( mut_data.activated || trans ) {
        if( c.get_mutation_states().at( mut_id ).powered ) {
            if( trans && !trans->msg_transform.empty() ) {
                c.add_msg_if_player( m_neutral, trans->msg_transform );
            } else {
                c.add_msg_if_player( m_neutral, _( "You stop using your %s." ), mut_data.name() );
            }
            return cata::optional<power_mut_ui_result>( cata::in_place, power_mut_ui_cmd::Deactivate, mut_id );
        } else if( ( !mut_data.hunger || c.get_kcal_percent() >= 0.8f ) &&
                   ( !mut_data.thirst || c.get_thirst() <= thirst_levels::dehydrated ) &&
                   ( !mut_data.fatigue || c.get_fatigue() <= 400 ) ) {
            if( trans && !trans->msg_transform.empty() ) {
                c.add_msg_if_player( m_neutral, trans->msg_transform );
            } else {
                c.add_msg_if_player( m_neutral, _( "You activate your %s." ), mut_data.name() );
            }
            return cata::optional<power_mut_ui_result>( cata::in_place, power_mut_ui_cmd::Activate, mut_id );
        } else {
            popup( _( "You don't have enough in you to activate your %s!" ), mut_data.name() );
        }
    } else {
        popup( _( "You cannot activate %s!" ), mut_data.name() );
    }
    return {};
}

// TODO: Change this to a proper structure
static const std::string mut_string = "mutation-";

static void register_mutation( input_context &ctx, const trait_id &mut )
{
    ctx.register_action( string_format( "%s%s", mut_string.c_str(), mut.str() ),
                         to_translation( mut.obj().name() ) );
}

static trait_id action_to_mutation( const std::string &action )
{
    if( std::equal( mut_string.begin(), mut_string.end(), action.begin() ) ) {
        std::string substring = action.substr( mut_string.length() );
        trait_id mut( substring );
        assert( mut.is_valid() );
        assert( mut );
        return mut;
    }

    return trait_id::NULL_ID();
}

power_mut_ui_result power_mutations_ui( Character &c )
{
    std::vector<trait_id> passive;
    std::vector<trait_id> active;
    invlet_wrapper free_chars = mutation_chars;
    for( const std::pair<const trait_id, mutation_state> &mut : c.get_mutation_states() ) {
        if( !mut.first->activated && !mut.first->transform ) {
            passive.push_back( mut.first );
        } else {
            active.push_back( mut.first );
        }
    }

    // maximal number of rows in both columns
    const int mutations_count = std::max( passive.size(), active.size() );

    const int TITLE_HEIGHT = 2;
    const int DESCRIPTION_HEIGHT = 5;
    // + lines with text in titlebar, local
    const int HEADER_LINE_Y = TITLE_HEIGHT + 1;
    const int list_start_y = HEADER_LINE_Y + 2;

    // Main window
    /** Total required height is:
    * top frame line:                                         + 1
    * height of title window:                                 + TITLE_HEIGHT
    * line after the title:                                   + 1
    * line with active/passive mutation captions:               + 1
    * height of the biggest list of active/passive mutations:   + mutations_count
    * line before mutation description:                         + 1
    * height of description window:                           + DESCRIPTION_HEIGHT
    * bottom frame line:                                      + 1
    * TOTAL: TITLE_HEIGHT + mutations_count + DESCRIPTION_HEIGHT + 5
    */

    int HEIGHT = 0;
    int WIDTH = 0;
    catacurses::window wBio;

    int DESCRIPTION_LINE_Y = 0;
    catacurses::window w_description;

    catacurses::window w_title;

    int second_column = 0;

    int cursor = 0;
    int scroll_position = 0;
    int max_scroll_position = 0;
    int list_height = 0;
    mutation_menu_mode menu_mode = mutation_menu_mode::activating;
    mutation_tab_mode tab_mode = mutation_tab_mode::active;
    const auto recalc_max_scroll_position = [&]() {
        list_height = ( true ?
                        DESCRIPTION_LINE_Y : HEIGHT - 1 ) - list_start_y;
        max_scroll_position = mutations_count - list_height;
        if( max_scroll_position < 0 ) {
            scroll_position = 0;
        } else if( scroll_position > max_scroll_position ) {
            scroll_position = max_scroll_position;
        }
    };

    ui_adaptor ui;
    ui.on_screen_resize( [&]( ui_adaptor & ui ) {
        HEIGHT = std::min( TERMY, std::max( FULL_SCREEN_HEIGHT,
                                            TITLE_HEIGHT + mutations_count + DESCRIPTION_HEIGHT + 5 ) );
        WIDTH = FULL_SCREEN_WIDTH + ( TERMX - FULL_SCREEN_WIDTH ) / 2;
        const point START( ( TERMX - WIDTH ) / 2, ( TERMY - HEIGHT ) / 2 );
        wBio = catacurses::newwin( HEIGHT, WIDTH, START );

        // Description window @ the bottom of the bionic window
        const int DESCRIPTION_START_Y = START.y + HEIGHT - DESCRIPTION_HEIGHT - 1;
        DESCRIPTION_LINE_Y = DESCRIPTION_START_Y - START.y - 1;
        w_description = catacurses::newwin( DESCRIPTION_HEIGHT, WIDTH - 2,
                                            point( START.x + 1, DESCRIPTION_START_Y ) );

        // Title window
        const int TITLE_START_Y = START.y + 1;
        w_title = catacurses::newwin( TITLE_HEIGHT, WIDTH - 2,
                                      point( START.x + 1, TITLE_START_Y ) );

        recalc_max_scroll_position();

        // X-coordinate of the list of active mutations
        second_column = 32 + ( TERMX - FULL_SCREEN_WIDTH ) / 4;

        ui.position_from_window( wBio );
    } );
    ui.mark_resize();

    input_context ctxt( "MUTATIONS" );
    ctxt.register_cardinal();
    ctxt.register_action( "ANY_INPUT" );
    ctxt.register_action( "CONFIRM" );
    ctxt.register_action( "REASSIGN" );
    ctxt.register_action( "HELP_KEYBINDINGS" );
    ctxt.register_action( "QUIT" );
#if defined(__ANDROID__)
    for( const auto &p : passive ) {
        ctxt.register_manual_key( my_mutations[p].key, p.obj().name() );
    }
    for( const auto &a : active ) {
        ctxt.register_manual_key( my_mutations[a].key, a.obj().name() );
    }
#endif

    // We do this
    for( const std::pair<const trait_id, mutation_state> &mut : c.get_mutation_states() ) {
        if( mut.second.key == ' ' ) {
            for( const auto &letter : mutation_chars ) {
                if( c.trait_by_invlet( letter ).is_null() ) {
                    mutation_state this_state = mut.second;
                    this_state.key = letter;
                    c.set_mutation_state( mut.first, this_state );
                    break;
                }
            }
        }
    }

    // TODO: Structure instead of ugly hack - biggest problem is Android testing
    for( const auto &a : active ) {
        register_mutation( ctxt, a );
    }

    cata::optional<trait_id> reassigning_id;

    ui.on_redraw( [&]( const ui_adaptor & ) {
        werase( wBio );
        draw_border( wBio, BORDER_COLOR, _( " MUTATIONS " ) );
        // Draw line under title
        mvwhline( wBio, point( 1, HEADER_LINE_Y ), LINE_OXOX, WIDTH - 2 );
        // Draw symbols to connect additional lines to border
        mvwputch( wBio, point( 0, HEADER_LINE_Y ), BORDER_COLOR, LINE_XXXO ); // |-
        mvwputch( wBio, point( WIDTH - 1, HEADER_LINE_Y ), BORDER_COLOR, LINE_XOXX ); // -|

        // Captions
        mvwprintz( wBio, point( 2, HEADER_LINE_Y + 1 ), c_light_blue, _( "Passive:" ) );
        mvwprintz( wBio, point( second_column, HEADER_LINE_Y + 1 ), c_light_blue, _( "Active:" ) );

        if( true ) {
            draw_exam_window( wBio, DESCRIPTION_LINE_Y );
        }

        const std::vector<trait_id> &mut_list = tab_mode == mutation_tab_mode::active ? active : passive;
        const trait_id &under_cursor = ( cursor >= 0 && cursor < static_cast<int>( mut_list.size() ) )
                                       ? mut_list[cursor]
                                       : trait_id::NULL_ID();
        if( passive.empty() ) {
            mvwprintz( wBio, point( 2, list_start_y ), c_light_gray, _( "None" ) );
        } else {
            for( int i = scroll_position; static_cast<size_t>( i ) < passive.size(); i++ ) {
                const trait_id &cur = passive[i];
                const mutation_branch &md = cur.obj();
                const mutation_state &state = c.get_mutation_states().at( cur );
                if( i - scroll_position == list_height ) {
                    break;
                }
                nc_color type = c.has_base_trait( cur ) ? c_cyan : c_light_cyan;
                if( reassigning_id && cur == *reassigning_id ) {
                    type = red_background( type );
                } else if( cur == under_cursor ) {
                    type = cyan_background( type );
                }
                mvwprintz( wBio, point( 2, list_start_y + i - scroll_position ),
                           type, "%c %s", state.key, md.name() );
            }
        }

        if( active.empty() ) {
            mvwprintz( wBio, point( second_column, list_start_y ), c_light_gray, _( "None" ) );
        } else {
            for( int i = scroll_position; static_cast<size_t>( i ) < active.size(); i++ ) {
                const trait_id &cur = active[i];
                const mutation_branch &md = active[i].obj();
                const mutation_state &state = c.get_mutation_states().at( cur );
                if( i - scroll_position == list_height ) {
                    break;
                }
                nc_color type;
                if( state.powered ) {
                    type = c.has_base_trait( cur ) ? c_green : c_light_green;
                } else {
                    type = c.has_base_trait( cur ) ? c_red : c_light_red;
                }
                if( reassigning_id && cur == *reassigning_id ) {
                    type = red_background( type );
                } else if( cur == under_cursor ) {
                    type = cyan_background( type );
                }
                // TODO: track resource(s) used and specify
                mvwputch( wBio, point( second_column, list_start_y + i - scroll_position ),
                          type, state.key );
                std::string mut_desc;
                mut_desc += md.name();
                if( md.cost > 0 && md.cooldown > 0 ) {
                    //~ RU means Resource Units
                    mut_desc += string_format( _( " - %d RU / %d turns" ),
                                               md.cost, md.cooldown );
                } else if( md.cost > 0 ) {
                    //~ RU means Resource Units
                    mut_desc += string_format( _( " - %d RU" ), md.cost );
                } else if( md.cooldown > 0 ) {
                    mut_desc += string_format( _( " - %d turns" ), md.cooldown );
                }
                if( state.powered ) {
                    mut_desc += _( " - Active" );
                }
                mvwprintz( wBio, point( second_column + 2, list_start_y + i - scroll_position ),
                           type, mut_desc );
            }
        }

        draw_scrollbar( wBio, scroll_position, list_height, mutations_count,
                        point( 0, list_start_y ), c_white, true );
        wnoutrefresh( wBio );
        show_mutations_titlebar( w_title, menu_mode, reassigning_id, ctxt );

        if( under_cursor ) {
            werase( w_description );
            fold_and_print( w_description, point_zero, WIDTH - 2, c_light_blue, under_cursor->desc() );
            wnoutrefresh( w_description );
        }
    } );

    power_mut_ui_result ret;
    bool exit = false;
    while( !exit ) {
        recalc_max_scroll_position();
        ui_manager::redraw();
        bool handled = false;
        const std::string action = ctxt.handle_input();
        const input_event evt = ctxt.get_raw_input();
        trait_id mut_id = trait_id::NULL_ID();
        if( action == "ANY_INPUT" && evt.type == CATA_INPUT_KEYBOARD && !evt.sequence.empty() ) {
            const int ch = evt.get_first_input();
            mut_id = c.trait_by_invlet( ch );
            // This case has to be special cased
            if( !mut_id && reassigning_id ) {
                const std::unordered_map<trait_id, mutation_state> &states = c.get_mutation_states();
                mutation_state this_state = states.at( *reassigning_id );
                c.set_mutation_state( *reassigning_id, this_state );
            }
        }

        if( !mut_id && menu_mode == mutation_menu_mode::activating ) {
            trait_id mut = action_to_mutation( action );
            if( mut ) {
                cata::optional<power_mut_ui_result> maybe_ret = try_activate_mutation( *mut, c );
                handled = true;
                if( maybe_ret ) {
                    ret = *maybe_ret;
                    exit = true;
                }
            }
        }

        if( !mut_id ) {
            if( action == "DOWN" || action == "UP" ) {
                int dir = action == "UP" ? -1 : 1;
                int max = tab_mode == mutation_tab_mode::active ? active.size() : passive.size();
                cursor = ( cursor + max + dir ) % max;
            } else if( action == "NEXT_TAB" || action == "PREV_TAB"
                       || action == "LEFT" || action == "RIGHT" ) {
                if( tab_mode == mutation_tab_mode::active ) {
                    tab_mode = mutation_tab_mode::passive;
                    cursor = std::min<int>( cursor, passive.size() - 1 );
                } else {
                    tab_mode = mutation_tab_mode::active;
                    cursor = std::min<int>( cursor, active.size() - 1 );
                }
            } else if( action == "CONFIRM" ) {
                const std::vector<trait_id> &mut_list = tab_mode == mutation_tab_mode::active ? active : passive;
                if( !mut_list.empty() ) {
                    mut_id = mut_list[cursor];
                }
            } else if( action == "REASSIGN" ) {
                menu_mode = menu_mode == mutation_menu_mode::reassigning
                            ? mutation_menu_mode::activating
                            : mutation_menu_mode::reassigning;
            } else if( action == "QUIT" ) {
                ret.cmd = power_mut_ui_cmd::Exit;
                exit = true;
            }
        }

        if( mut_id ) {
            if( reassigning_id && *reassigning_id != mut_id ) {
                const std::unordered_map<trait_id, mutation_state> &states = c.get_mutation_states();
                mutation_state mut_state = states.at( mut_id );
                mutation_state reassigning_state = states.at( *reassigning_id );
                std::swap( mut_state.key, reassigning_state.key );
                c.set_mutation_state( *reassigning_id, reassigning_state );
                c.set_mutation_state( mut_id, mut_state );
                reassigning_id.reset();
            } else if( menu_mode == mutation_menu_mode::reassigning ) {
                reassigning_id = mut_id;
            } else if( menu_mode == mutation_menu_mode::activating ) {
                cata::optional<power_mut_ui_result> maybe_ret = try_activate_mutation( *mut_id, c );
                if( maybe_ret ) {
                    ret = *maybe_ret;
                    exit = true;
                }
            }
        }
    }

    return ret;
}

} // namespace mutations
