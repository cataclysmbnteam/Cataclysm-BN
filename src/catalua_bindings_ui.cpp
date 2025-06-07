#include "catalua_bindings.h"
#include "catalua_bindings_utils.h"
#include "catalua_luna.h"
#include "catalua_luna_doc.h"

#include "ui.h"
#include "popup.h"
#include "string_input_popup.h"

void cata::detail::reg_ui_elements( sol::state &lua )
{
    {
        sol::usertype<uilist> ut =
            luna::new_usertype<uilist>(
                lua,
                luna::no_bases,
                luna::constructors <
                uilist()
                > ()
            );
        DOC( "Sets title which is on the top line." );
        luna::set_fx( ut, "title", []( uilist & ui, const std::string & text ) {
            ui.title = text;
        } );
        DOC( "Sets text which is in upper box." );
        luna::set_fx( ut, "text", []( uilist & ui, const std::string & input ) {
            ui.text = input;
        } );
        DOC( "Sets footer text which is in lower box. It overwrites descs of entries unless is empty." );
        luna::set_fx( ut, "footer", []( uilist & ui, const std::string & text ) {
            ui.footer_text = text;
        } );
        DOC( "Puts a lower box. Footer or entry desc appears on it." );
        luna::set_fx( ut, "desc_enabled", []( uilist & ui, bool value ) {
            ui.desc_enabled = value;
        } );
        DOC( "Adds an entry. `string` is its name, and `int` is what it returns. If `int` is `-1`, the number is decided orderly." );
        luna::set_fx( ut, "add", []( uilist & ui, int retval, const std::string & text ) {
            ui.addentry( retval, true, MENU_AUTOASSIGN, text );
        } );
        DOC( "Adds an entry with desc(second `string`). `desc_enabled(true)` is required for showing desc." );
        luna::set_fx( ut, "add_w_desc", []( uilist & ui, int retval, const std::string & text,
        const std::string & desc ) {
            ui.addentry_desc( retval, true, MENU_AUTOASSIGN, text, desc );
        } );
        DOC( "Adds an entry with desc and col(third `string`). col is additional text on the right of the entry name." );
        luna::set_fx( ut, "add_w_col", []( uilist & ui, int retval, const std::string & text,
        const std::string & desc, const std::string col ) {
            ui.addentry_col( retval, true, MENU_AUTOASSIGN, text, col, desc );
        } );
        DOC( "Entries from uilist. Remember, in lua, the first element of vector is `entries[1]`, not `entries[0]`." );
        luna::set( ut, "entries", &uilist::entries );
        DOC( "Changes the color. Default color is `c_magenta`." );
        luna::set_fx( ut, "border_color", []( uilist & ui, color_id col ) {
            ui.border_color = get_all_colors().get( col );
        } );
        DOC( "Changes the color. Default color is `c_light_gray`." );
        luna::set_fx( ut, "text_color", []( uilist & ui, color_id col ) {
            ui.text_color = get_all_colors().get( col );
        } );
        DOC( "Changes the color. Default color is `c_green`." );
        luna::set_fx( ut, "title_color", []( uilist & ui, color_id col ) {
            ui.title_color = get_all_colors().get( col );
        } );
        DOC( "Changes the color. Default color is `h_white`." );
        luna::set_fx( ut, "hilight_color", []( uilist & ui, color_id col ) {
            ui.hilight_color = get_all_colors().get( col );
        } );
        DOC( "Changes the color. Default color is `c_light_green`." );
        luna::set_fx( ut, "hotkey_color", []( uilist & ui, color_id col ) {
            ui.hotkey_color = get_all_colors().get( col );
        } );
        DOC( "Returns retval for selected entry, or a negative number on fail/cancel" );
        luna::set_fx( ut, "query", []( uilist & ui ) {
            ui.query();
            return ui.ret;
        } );
    }
    {
        DOC( "This type came from UiList." );
        sol::usertype<uilist_entry> ut =
            luna::new_usertype<uilist_entry>(
                lua,
                luna::no_bases,
                luna::no_constructor
            );
        DOC( "Entry whether it's enabled or not. Default is `true`." );
        luna::set( ut, "enable", &uilist_entry::enabled );
        DOC( "Entry text" );
        luna::set( ut, "txt", &uilist_entry::txt );
        DOC( "Entry description" );
        luna::set( ut, "desc", &uilist_entry::desc );
        DOC( "Entry text of column." );
        luna::set( ut, "ctxt",  &uilist_entry::ctxt );
        DOC( "Entry text color. Its default color is `c_red_red`, which makes color of the entry same as what `uilist` decides. So if you want to make color different, choose one except `c_red_red`." );
        luna::set_fx( ut, "txt_color", []( uilist_entry & ui_entry, color_id col ) {
            ui_entry.text_color = get_all_colors().get( col );
        } );
    }

    {
        sol::usertype<query_popup> ut =
            luna::new_usertype<query_popup>(
                lua,
                luna::no_bases,
                luna::constructors <
                query_popup()
                > ()
            );
        luna::set_fx( ut, "message", []( query_popup & popup, sol::variadic_args va ) {
            popup.message( "%s", cata::detail::fmt_lua_va( va ) );
        } );
        luna::set_fx( ut, "message_color", []( query_popup & popup, color_id col ) {
            popup.default_color( get_all_colors().get( col ) );
        } );
        DOC( "Set whether to allow any key" );
        luna::set_fx( ut, "allow_any_key", []( query_popup & popup, bool val ) {
            popup.allow_anykey( val );
        } );
        DOC( "Returns selected action" );
        luna::set_fx( ut, "query", []( query_popup & popup ) {
            return popup.query().action;
        } );
        DOC( "Returns `YES` or `NO`. If ESC pressed, returns `NO`." );
        luna::set_fx( ut, "query_yn", []( query_popup & popup ) {
            return popup
                   .context( "YESNO" )
                   .option( "YES" )
                   .option( "NO" )
                   .query()
                   .action;
        } );
        DOC( "Returns `YES`, `NO` or `QUIT`. If ESC pressed, returns `QUIT`." );
        luna::set_fx( ut, "query_ynq", []( query_popup & popup ) {
            return popup
                   .context( "YESNOQUIT" )
                   .option( "YES" )
                   .option( "NO" )
                   .option( "QUIT" )
                   .query()
                   .action;
        } );
    }

    {
        sol::usertype<string_input_popup> ut =
            luna::new_usertype<string_input_popup>(
                lua,
                luna::no_bases,
                luna::constructors <
                string_input_popup()
                > ()
            );
        DOC( "`title` is on the left of input field." );
        luna::set_fx( ut, "title", []( string_input_popup & sipop, const std::string & text ) {
            sipop.title( text );
        } );
        DOC( "`desc` is above input field." );
        luna::set_fx( ut, "desc", []( string_input_popup & sipop, const std::string & text ) {
            sipop.description( text );
        } );
        DOC( "Returns your input." );
        luna::set_fx( ut, "query_str", []( string_input_popup & sipop ) {
            sipop.only_digits( false );
            return sipop.query_string();
        } );
        DOC( "Returns your input, but allows numbers only." );
        luna::set_fx( ut, "query_int", []( string_input_popup & sipop ) {
            sipop.only_digits( true );
            return sipop.query_int();
        } );
    }
}
