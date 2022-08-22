#include "examine_item_menu.h"

#include "auto_pickup.h"
#include "avatar_action.h"
#include "avatar.h"
#include "game_inventory.h"
#include "input.h"
#include "item.h"
#include "messages.h"
#include "output.h"
#include "ui_manager.h"
#include "ui.h"

struct action_entry {
    std::string action;
    std::function<bool()> on_select;
};

namespace examine_item_menu
{

bool run(
    item_location loc,
    const std::function<int()> &func_pos_x,
    const std::function<int()> &func_width,
    menu_pos_t menu_pos
)
{
    avatar &you = get_avatar();
    item &itm = *loc;

    // Sanity check
    if( !you.has_item( itm ) ) {
        return true;
    }

    catacurses::window w_info;

    input_context ctxt( "INVENTORY_ITEM" );
    ctxt.register_cardinal();
    ctxt.register_action( "PAGE_UP" );
    ctxt.register_action( "PAGE_DOWN" );
    ctxt.register_action( "SCROLL_UP" );
    ctxt.register_action( "SCROLL_DOWN" );
    ctxt.register_action( "CONFIRM" );
    ctxt.register_action( "QUIT" );
    ctxt.register_action( "HELP_KEYBINDINGS" );

    int info_area_scroll_pos = 0;
    constexpr int info_area_scroll_step = 3;
    std::vector<iteminfo> item_info_vals;
    std::vector<iteminfo> dummy_compare;
    itm.info( true, item_info_vals );
    item_info_data data( itm.tname(), itm.type_name(), item_info_vals, dummy_compare,
                         info_area_scroll_pos );

    // The following line stops the game from crashing. Why..?
    data.without_getch = true;

    const bool has_pickup_rule = get_auto_pickup().has_rule( &itm );
    const bool is_wielded = you.is_wielding( itm );
    const bool cant_unwield_if_weapon = is_wielded && !you.can_unwield( itm ).success();
    const bool cant_unwield_existing_weapon = you.is_armed() &&
            !you.can_unwield( you.weapon ).success();
    const bool cant_takeoff_if_worn = you.is_wearing( itm ) &&
                                      !you.can_takeoff( itm ).success();
    const bool cant_dispose_of = cant_unwield_if_weapon || cant_takeoff_if_worn;
    const bool cant_acquare = cant_unwield_existing_weapon || cant_takeoff_if_worn;
    const hint_rating rate_unwield_item = cant_dispose_of ? hint_rating::cant :
                                          hint_rating::good;
    const hint_rating rate_wield_item = cant_acquare ? hint_rating::cant :
                                        hint_rating::good;
    const hint_rating rate_drop_item = cant_dispose_of ? hint_rating::cant :
                                       hint_rating::good;

    std::vector<action_entry> actions;
    uilist action_list;

    const auto add_entry = [&]( const char *act, hint_rating hint, std::function<bool()> &&on_select ) {
        action_entry ae;
        ae.action = act;
        ae.on_select = std::move( on_select );
        actions.push_back( std::move( ae ) );

        ctxt.register_action( act );

        std::string bound_key = ctxt.key_bound_to( act );
        int bound_key_i = bound_key.size() == 1 ? bound_key[0] : '?';
        std::string act_name = ctxt.get_action_name( act );
        action_list.addentry( actions.size(), true, bound_key_i, act_name );

        auto &list_entry = action_list.entries.back();
        switch( hint ) {
            case hint_rating::cant:
                list_entry.text_color = c_light_gray;
                break;
            case hint_rating::iffy:
                list_entry.text_color = c_light_red;
                break;
            case hint_rating::good:
                list_entry.text_color = c_light_green;
                break;
        }
    };

    add_entry( "ACTIVATE", you.rate_action_use( itm ), [&]() {
        avatar_action::use_item( you, loc );
        return true;
    } );

    add_entry( "READ", you.rate_action_read( itm ), [&]() {
        you.read( loc );
        return true;
    } );

    add_entry( "EAT", you.rate_action_eat( itm ), [&]() {
        avatar_action::eat( you, loc );
        return true;
    } );

    add_entry( "WEAR", you.rate_action_wear( itm ), [&]() {
        you.wear( itm );
        return true;
    } );

    if( !is_wielded ) {
        add_entry( "WIELD", rate_wield_item, [&]() {
            avatar_action::wield( loc );
            return true;
        } );
    } else {
        add_entry( "UNWIELD", rate_unwield_item, [&]() {
            avatar_action::wield( loc );
            return true;
        } );
    }

    add_entry( "THROW", rate_drop_item, [&]() {
        avatar_action::plthrow( you, loc );
        return true;
    } );

    add_entry( "CHANGE_SIDE", you.rate_action_change_side( itm ), [&]() {
        you.change_side( loc );
        return true;
    } );

    add_entry( "TAKE_OFF", you.rate_action_takeoff( itm ), [&]() {
        you.takeoff( itm );
        return true;
    } );

    add_entry( "DROP", rate_drop_item, [&]() {
        you.drop( loc, you.pos() );
        return true;
    } );

    add_entry( "UNLOAD", you.rate_action_unload( itm ), [&]() {
        you.unload( loc );
        return true;
    } );

    add_entry( "RELOAD", you.rate_action_reload( itm ), [&]() {
        avatar_action::reload( loc );
        return true;
    } );

    add_entry( "PART_RELOAD", you.rate_action_reload( itm ), [&]() {
        avatar_action::reload( loc, true );
        return true;
    } );

    add_entry( "MEND", you.rate_action_mend( itm ), [&]() {
        avatar_action::mend( you, loc );
        return true;
    } );

    add_entry( "DISASSEMBLE", you.rate_action_disassemble( itm ), [&]() {
        you.disassemble( loc, false );
        return true;
    } );

    if( !itm.is_favorite ) {
        add_entry( "FAVORITE_ADD",
        hint_rating::good, [&]() {
            itm.is_favorite = true;
            return false;
        } );
    } else {
        add_entry( "FAVORITE_REMOVE",
        hint_rating::good, [&]() {
            itm.is_favorite = false;
            return false;
        } );
    }

    add_entry( "REASSIGN", hint_rating::good, [&]() {
        game_menus::inv::reassign_letter( you, itm );
        return false;
    } );

    if( !has_pickup_rule ) {
        add_entry( "AUTOPICKUP_ADD", hint_rating::good, [&]() {
            get_auto_pickup().add_rule( &itm );
            add_msg( m_info, _( "'%s' added to character pickup rules." ), itm.tname( 1, false ) );
            return false;
        } );
    } else {
        add_entry( "AUTOPICKUP_REMOVE", hint_rating::iffy, [&]() {
            get_auto_pickup().remove_rule( &itm );
            add_msg( m_info, _( "'%s' removed from character pickup rules." ), itm.tname( 1, false ) );
            return false;
        } );
    }

    int selected_action = 0;
    int num_actions = static_cast<int>( actions.size() );

    std::unique_ptr<ui_adaptor> ui = std::make_unique<ui_adaptor>();
    ui->on_screen_resize( [&]( ui_adaptor & ui ) {
        int width = func_width();
        int pos_x = func_pos_x();
        w_info = catacurses::newwin( TERMY, width, point( pos_x, 0 ) );
        ui.position_from_window( w_info );
    } );
    action_list.w_y_setup = 0;
    action_list.w_x_setup = [&]( const int popup_width ) -> int {
        switch( menu_pos )
        {
            default:
            case menu_pos_t::left:
                return func_pos_x() - popup_width;
            case menu_pos_t::right:
                return func_pos_x() + func_width();
        }
    };
    ui->mark_resize();
    ui->on_redraw( [&]( const ui_adaptor & ) {
        draw_item_info( w_info, data );
        action_list.show();
    } );

    bool exit = false;
    bool ret_val = true;
    while( !exit ) {
        ui->invalidate_ui();
        ui_manager::redraw();

        const std::string &action = ctxt.handle_input();

        if( action == "QUIT" || action == "LEFT" ) {
            ret_val = false;
            exit = true;
            continue;
        } else if( action == "CONFIRM" || action == "RIGHT" ) {
            ret_val = actions[selected_action].on_select();
            exit = true;
        } else if( action == "PAGE_UP" ) {
            info_area_scroll_pos -= info_area_scroll_step;
        } else if( action == "PAGE_DOWN" ) {
            info_area_scroll_pos += info_area_scroll_step;
        } else if( action == "SCROLL_UP" || action == "UP" ) {
            selected_action = ( selected_action - 1 + num_actions ) % num_actions;
            action_list.set_selected( selected_action );
        } else if( action == "SCROLL_DOWN" || action == "DOWN" ) {
            selected_action = ( selected_action + 1 + num_actions ) % num_actions;
            action_list.set_selected( selected_action );
        } else {
            for( action_entry &entry : actions ) {
                if( entry.action == action ) {
                    ret_val = entry.on_select();
                    exit = true;
                    break;
                }
            }
        }
    }
    return ret_val;
}

} // namespace examine_item_menu
