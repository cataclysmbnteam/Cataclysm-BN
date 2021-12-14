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

// '!' and '=' are uses as default bindings in the menu
const invlet_wrapper
mutation_chars( "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\"#&()*+./:;@[\\]^_{|}" );

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
    examining,
    reassigning,
};

static void show_mutations_titlebar( const catacurses::window &window,
                                     const mutation_menu_mode menu_mode, const input_context &ctxt )
{
    werase( window );
    std::string desc;
    if( menu_mode == mutation_menu_mode::reassigning ) {
        desc += std::string( _( "Reassigning." ) ) + "  " +
                _( "Select a mutation to reassign or press [<color_yellow>SPACE</color>] to cancel. " );
    }
    if( menu_mode == mutation_menu_mode::activating ) {
        desc += colorize( _( "Activating" ),
                          c_green ) + "  " + shortcut_desc( _( "%s to examine mutation, " ),
                                  ctxt.get_desc( "TOGGLE_EXAMINE" ) );
    }
    if( menu_mode == mutation_menu_mode::examining ) {
        desc += colorize( _( "Examining" ),
                          c_light_blue ) + "  " + shortcut_desc( _( "%s to activate mutation, " ),
                                  ctxt.get_desc( "TOGGLE_EXAMINE" ) );
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
        popup( _( "You cannot activate %s!  To read a description of "
                  "%s, press '!', then '%c'." ),
               mut_data.name(), mut_data.name(), c.get_mutation_states().at( mut_id ).key );
    }
    return {};
}

power_mut_ui_result power_mutations_ui( Character &c )
{
    std::vector<trait_id> passive;
    std::vector<trait_id> active;
    for( const std::pair<const trait_id, mutation_state> &mut : c.get_mutation_states() ) {
        if( !mut.first->activated && ! mut.first->transform ) {
            passive.push_back( mut.first );
        } else {
            active.push_back( mut.first );
        }
        // New mutations are initialized with no key at all, so we have to do this here.
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

    int scroll_position = 0;
    int max_scroll_position = 0;
    int list_height = 0;
    mutation_menu_mode menu_mode = mutation_menu_mode::activating;
    const auto recalc_max_scroll_position = [&]() {
        list_height = ( menu_mode == mutation_menu_mode::examining ?
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
    ctxt.register_updown();
    ctxt.register_action( "ANY_INPUT" );
    ctxt.register_action( "TOGGLE_EXAMINE" );
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

    // TODO: Structure instead of ugly hack - biggest problem is Android testing
    for( const auto &a : active ) {
        ctxt.register_action( string_format( "mutation-%s", a.str() ), to_translation( a.obj().name() ) );
    }

    cata::optional<trait_id> examine_id;

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

        if( menu_mode == mutation_menu_mode::examining ) {
            draw_exam_window( wBio, DESCRIPTION_LINE_Y );
        }
        nc_color type;
        if( passive.empty() ) {
            mvwprintz( wBio, point( 2, list_start_y ), c_light_gray, _( "None" ) );
        } else {
            for( int i = scroll_position; static_cast<size_t>( i ) < passive.size(); i++ ) {
                const mutation_branch &md = passive[i].obj();
                const mutation_state &td = c.get_mutation_states().at( passive[i] );
                if( i - scroll_position == list_height ) {
                    break;
                }
                type = c.has_base_trait( passive[i] ) ? c_cyan : c_light_cyan;
                mvwprintz( wBio, point( 2, list_start_y + i - scroll_position ),
                           type, "%c %s", td.key, md.name() );
            }
        }

        if( active.empty() ) {
            mvwprintz( wBio, point( second_column, list_start_y ), c_light_gray, _( "None" ) );
        } else {
            for( int i = scroll_position; static_cast<size_t>( i ) < active.size(); i++ ) {
                const mutation_branch &md = active[i].obj();
                const mutation_state &td = c.get_mutation_states().at( active[i] );
                if( i - scroll_position == list_height ) {
                    break;
                }
                if( td.powered ) {
                    type = c.has_base_trait( active[i] ) ? c_green : c_light_green;
                } else {
                    type = c.has_base_trait( active[i] ) ? c_red : c_light_red;
                }
                // TODO: track resource(s) used and specify
                mvwputch( wBio, point( second_column, list_start_y + i - scroll_position ),
                          type, td.key );
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
                if( td.powered ) {
                    mut_desc += _( " - Active" );
                }
                mvwprintz( wBio, point( second_column + 2, list_start_y + i - scroll_position ),
                           type, mut_desc );
            }
        }

        draw_scrollbar( wBio, scroll_position, list_height, mutations_count,
                        point( 0, list_start_y ), c_white, true );
        wnoutrefresh( wBio );
        show_mutations_titlebar( w_title, menu_mode, ctxt );

        if( menu_mode == mutation_menu_mode::examining && examine_id.has_value() ) {
            werase( w_description );
            fold_and_print( w_description, point_zero, WIDTH - 2, c_light_blue, examine_id.value()->desc() );
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
        if( action == "ANY_INPUT" && evt.type == CATA_INPUT_KEYBOARD && !evt.sequence.empty() ) {
            const int ch = evt.get_first_input();
            const trait_id mut_id = c.trait_by_invlet( ch );
            if( !mut_id.is_null() ) {
                const mutation_branch &mut_data = mut_id.obj();
                switch( menu_mode ) {
                    case mutation_menu_mode::reassigning: {
                        query_popup pop;
                        pop.message( _( "%s; enter new letter." ),
                                     mutation_branch::get_name( mut_id ) )
                        .context( "POPUP_WAIT" )
                        .allow_cancel( true )
                        .allow_anykey( true );

                        bool pop_exit = false;
                        while( !pop_exit ) {
                            const query_popup::result ret = pop.query();
                            bool pop_handled = false;
                            if( ret.evt.type == CATA_INPUT_KEYBOARD && !ret.evt.sequence.empty() ) {
                                const int newch = ret.evt.get_first_input();
                                if( mutation_chars.valid( newch ) ) {
                                    const trait_id other_mut_id = c.trait_by_invlet( newch );
                                    const std::unordered_map<trait_id, mutation_state> &states = c.get_mutation_states();
                                    mutation_state this_state = states.at( mut_id );
                                    if( !other_mut_id.is_null() ) {
                                        mutation_state other_state = states.at( other_mut_id );
                                        std::swap( this_state.key, other_state.key );
                                        c.set_mutation_state( other_mut_id, other_state );
                                    } else {
                                        this_state.key = newch;
                                    }
                                    c.set_mutation_state( mut_id, this_state );
                                    pop_exit = true;
                                    pop_handled = true;
                                }
                            }
                            if( !pop_handled ) {
                                if( ret.action == "QUIT" ) {
                                    pop_exit = true;
                                } else if( ret.action != "HELP_KEYBINDINGS" &&
                                           ret.evt.type == CATA_INPUT_KEYBOARD ) {
                                    popup( _( "Invalid mutation letter.  Only those characters are valid:\n\n%s" ),
                                           mutation_chars.get_allowed_chars() );
                                }
                            }
                        }

                        menu_mode = mutation_menu_mode::activating;
                        examine_id = cata::nullopt;
                        // TODO: show a message like when reassigning a key to an item?
                        break;
                    }
                    case mutation_menu_mode::activating: {
                        cata::optional<power_mut_ui_result> maybe_ret = try_activate_mutation( mut_data, c );
                        if( maybe_ret ) {
                            ret = *maybe_ret;
                            exit = true;
                        }
                        break;
                    }
                    case mutation_menu_mode::examining:
                        // Describing mutations, not activating them!
                        examine_id = mut_id;
                        break;
                }
                handled = true;
            } else if( mutation_chars.valid( ch ) ) {
                handled = true;
            }
        }
        if( !handled ) {
            if( action == "DOWN" ) {
                if( scroll_position < max_scroll_position ) {
                    scroll_position++;
                }
            } else if( action == "UP" ) {
                if( scroll_position > 0 ) {
                    scroll_position--;
                }
            } else if( action == "REASSIGN" ) {
                menu_mode = mutation_menu_mode::reassigning;
                examine_id = cata::nullopt;
            } else if( action == "TOGGLE_EXAMINE" ) {
                // switches between activation and examination
                menu_mode = menu_mode == mutation_menu_mode::activating ?
                            mutation_menu_mode::examining : mutation_menu_mode::activating;
                examine_id = cata::nullopt;
            } else if( action == "QUIT" ) {
                ret.cmd = power_mut_ui_cmd::Exit;
                exit = true;
            }
        }
    }

    return ret;
}

} // namespace mutations
