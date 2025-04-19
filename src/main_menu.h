#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "cuboid_rectangle.h"
#include "cursesdef.h"
#include "input.h"
#include "point.h"
#include "worldfactory.h"
#include "enums.h"
#include "color.h"

class main_menu
{
        friend class sound_on_move_uilist_callback;
    public:
        main_menu() : ctxt( "MAIN_MENU" ) { }
        // Shows the main menu and returns whether a game was started or not
        bool opening_screen();

    private:
        // ASCII art that says "Cataclysm Dark Days Ahead"
        std::vector<std::string> mmenu_title;
        std::string mmenu_motd;
        std::string mmenu_credits;
        int mmenu_motd_len;
        int mmenu_credits_len;
        std::vector<std::string> vMenuItems; // MOTD, New Game, Load Game, etc.
        std::vector<std::string> vWorldSubItems;
        std::vector<std::string> vNewGameSubItems;
        std::vector<std::string> vNewGameHints;
        std::vector<char> vWorldHotkeys;
        std::vector<std::string> vSettingsSubItems;
        std::vector< std::vector<std::string> > vSettingsHotkeys;
        std::vector< std::vector<std::string> > vMenuHotkeys; // hotkeys for the vMenuItems
        std::vector< std::vector<std::string> > vNewGameHotkeys;
        std::string vdaytip; //tip of the day

        /**
         * Does what it sounds like, but this function also exists in order to gracefully handle
         * the case where the player goes to the 'Settings' tab and changes the language.
        */
        void init_strings();
        /** Helper function for @ref init_strings */
        std::vector<std::string> load_file( const std::string &path,
                                            const std::string &alt_text ) const;

        // Play a sound whenever the user moves left or right in the main menu or its tabs
        void on_move() const;

        // Play a sound *once* when an error occurs in the main menu or its tabs; sets errflag
        void on_error();

        // Tab functions. They return whether a game was started or not. The ones that can never
        // start a game have a void return type.
        bool new_character_tab();
        bool load_character_tab( const std::string &worldname );
        void world_tab( const std::string &worldname );

        /*
         * Load character templates from template folder
         */
        void load_char_templates();

        // These variables are shared between @opening_screen and the tab functions.
        // TODO: But this is an ugly short-term solution.
        input_context ctxt;
        // what main menu item is currently activated
        // activated item can be drawn differently (higlighted)
        // "activate" means "do the action this menu is about"
        int activated_menu_item_ = 1;
        int sel2 = 1;
        // color constants
        // default main menu item color (not highlighted, not selected/activated)
        const nc_color menu_item_default_color_ { c_light_gray };
        // default color for shortcut character used in menu item text
        const nc_color shortcut_character_default_color_ { c_yellow };

        // Used to optimise mouse input handling
        // if set to true - do not redraw UI next cycle
        // Will be reset back to "false" after redraw happened
        // Mouse cursor movement generates a lot of input per second,
        // each input is usually followed by UI redraw with lots of recalculations
        // But most of the time mouse movement changes nothing on the screen
        // so there is no point calculating and redrawing the same thing if nothing was changed
        // If mouse input handling logic detects there was no change in UI - it sets
        // this variable to "true" to skip unnesesary redrawing
        bool skip_next_redraw_{ false };

        point LAST_TERM;
        catacurses::window w_open;
        point menu_offset;
        std::vector<std::string> templates;
        int extra_w = 0;
        std::vector<save_t> savegames;
        std::vector<std::pair<inclusive_rectangle<point>, std::pair<int, int>>> main_menu_sub_button_map;
        std::vector<std::pair<inclusive_rectangle<point>, int>> main_menu_button_map;

        /**
         * Prints a horizontal list of options
         *
         * @param w_in Window we are printing in
         * @param vItems Main menu items
         * @param iSel Which index of vItems is selected. This menu item will be highlighted to
         * make it stand out from the other menu items.
         * @param offset Offset of menu items
         * @param spacing: How many spaces to print between each menu item
         * @returns A list of horizontal offsets, one for each menu item
         */
        std::vector<int> print_menu_items( const catacurses::window &w_in,
                                           const std::vector<std::string> &vItems, size_t iSel,
                                           point offset, int spacing = 1, bool main = false );

        /**
         * Called by @ref opening_screen, this prints all the text that you see on the main menu
         *
         * @param w_open Window to print menu in
         * @param iSel which index in vMenuItems is selected
         * @param offset Menu location in window
         */
        void print_menu( const catacurses::window &w_open, int iSel, const point &offset, int sel_line );

        void display_text( const std::string &text, const std::string &title, int &selected );

        void display_sub_menu( int sel, const point &bottom_left, int sel_line );

        void init_windows();

        // checks if mouse cursor is over one on the main menu item
        std::optional<int> isMouseOverMenuItem() const;
        // checks if mouse cursor is over one of the submenu item
        std::optional<int> isMouseOverSubmenuItem() const;

        holiday get_holiday_from_time();

        holiday current_holiday = holiday::none;

        static std::string halloween_spider();
        std::string halloween_graves();
};



