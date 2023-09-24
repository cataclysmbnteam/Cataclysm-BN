#include "options.h"
#include "options_category.h"
#include "language.h"

void options_manager::add_options_interface()
{
    const auto add_empty_line = [&]() {
        this->add_empty_line( interface );
    };

    std::vector<options_manager::id_and_option> lang_options = {
        { "", translate_marker( "System language" ) },
    };
    for( const language_info &info : list_available_languages() ) {
        lang_options.push_back( {info.id, no_translation( info.name )} );
    }

    add( "USE_LANG", interface, translate_marker( "Language" ),
         translate_marker( "Switch Language." ), lang_options, "" );

    add_empty_line();

    add( "USE_CELSIUS", interface, translate_marker( "Temperature units" ),
         translate_marker( "Switch between Celsius, Fahrenheit and Kelvin." ),
    { { "celsius", translate_marker( "Celsius" ) }, { "fahrenheit", translate_marker( "Fahrenheit" ) }, { "kelvin", translate_marker( "Kelvin" ) } },
    "celsius"
       );

    add( "USE_METRIC_SPEEDS", interface, translate_marker( "Speed units" ),
         translate_marker( "Switch between km/h, mph and tiles/turn." ),
    { { "km/h", translate_marker( "km/h" ) }, { "mph", translate_marker( "mph" ) }, { "t/t", translate_marker( "tiles/turn" ) } },
    "km/h"
       );

    add( "USE_METRIC_WEIGHTS", interface, translate_marker( "Mass units" ),
         translate_marker( "Switch between kg and lbs." ),
    {  { "kg", translate_marker( "kg" ) }, { "lbs", translate_marker( "lbs" ) } }, "kg"
       );

    add( "VOLUME_UNITS", interface, translate_marker( "Volume units" ),
         translate_marker( "Switch between the Liter ( L ), Cup ( c ), or Quart ( qt )." ),
    {  { "l", translate_marker( "Liter" ) }, { "c", translate_marker( "Cup" ) }, { "qt", translate_marker( "Quart" ) } },
    "l"
       );
    add( "DISTANCE_UNITS", interface, translate_marker( "Distance units" ),
         translate_marker( "Metric or Imperial" ),
    { { "metric", translate_marker( "Metric" ) }, { "imperial", translate_marker( "Imperial" ) } },
    "metric" );

    add(
        "OVERMAP_COORDINATE_FORMAT",
        interface,
        translate_marker( "Overmap coordinates format" ),
        translate_marker( "Are overmap coordinates displayed using absolute format like 338, 416 or subdivided into two components like 1'158, 2'56?" ),
    { { "subdivided", translate_marker( "Subdivided" ) }, { "absolute", translate_marker( "Absolute" ) } },
    "absolute"
    );

    add( "24_HOUR", interface, translate_marker( "Time format" ),
         translate_marker( "12h: AM/PM, e.g. 7:31 AM - Military: 24h Military, e.g. 0731 - 24h: Normal 24h, e.g. 7:31" ),
         //~ 12h time, e.g.  11:59pm
    {   { "12h", translate_marker( "12h" ) },
        //~ Military time, e.g.  2359
        { "military", translate_marker( "Military" ) },
        //~ 24h time, e.g.  23:59
        { "24h", translate_marker( "24h" ) }
    },
    "12h" );

    add_empty_line();

    add( "FORCE_CAPITAL_YN", interface, translate_marker( "Force Y/N in prompts" ),
         translate_marker( "If true, Y/N prompts are case-sensitive and y and n are not accepted." ),
         true
       );

    add( "SNAP_TO_TARGET", interface, translate_marker( "Snap to target" ),
         translate_marker( "If true, automatically follow the crosshair when firing/throwing." ),
         false
       );

    add( "AIM_AFTER_FIRING", interface, translate_marker( "Reaim after firing" ),
         translate_marker( "If true, after firing automatically aim again if targets are available." ),
         true
       );

    add( "QUERY_DISASSEMBLE", interface, translate_marker( "Query on disassembly while butchering" ),
         translate_marker( "If true, will query before disassembling items while butchering." ),
         true
       );

    add( "QUERY_KEYBIND_REMOVAL", interface, translate_marker( "Query on keybinding removal" ),
         translate_marker( "If true, will query before removing a keybinding from a hotkey." ),
         true
       );

    add( "CLOSE_ADV_INV", interface, translate_marker( "Close advanced inventory on move all" ),
         translate_marker( "If true, will close the advanced inventory when the move all items command is used." ),
         false
       );

    add( "OPEN_DEFAULT_ADV_INV", interface,
         translate_marker( "Open default advanced inventory layout" ),
         translate_marker( "Open default advanced inventory layout instead of last opened layout" ),
         false
       );

    add( "INV_USE_ACTION_NAMES", interface, translate_marker( "Display actions in Use Item menu" ),
         translate_marker( "If true, actions ( like \"Read\", \"Smoke\", \"Wrap tighter\" ) will be displayed next to the corresponding items." ),
         true
       );

    add( "AUTOSELECT_SINGLE_VALID_TARGET", interface,
         translate_marker( "Autoselect if exactly one valid target" ),
         translate_marker( "If true, directional actions ( like \"Examine\", \"Open\", \"Pickup\" ) "
                           "will autoselect an adjacent tile if there is exactly one valid target." ),
         true
       );

    add_empty_line();

    add( "DIAG_MOVE_WITH_MODIFIERS_MODE", interface,
         translate_marker( "Diagonal movement with cursor keys and modifiers" ),
         /*
         Possible modes:

         # None

         # Mode 1: Numpad Emulation

         * Press and keep holding Ctrl
         * Press and release ↑ to set it as the modifier (until Ctrl is released)
         * Press and release → to get the move ↑ + → = ↗ i.e. just like pressing and releasing 9
         * Holding → results in repeated ↗, so just like holding 9
         * If I press any other direction, they are similarly modified by ↑, both for single presses and while holding.

         # Mode 2: CW/CCW

         * `Shift` + `Cursor Left` -> `7` = `Move Northwest`;
         * `Shift` + `Cursor Right` -> `3` = `Move Southeast`;
         * `Shift` + `Cursor Up` -> `9` = `Move Northeast`;
         * `Shift` + `Cursor Down` -> `1` = `Move Southwest`.

         and

         * `Ctrl` + `Cursor Left` -> `1` = `Move Southwest`;
         * `Ctrl` + `Cursor Right` -> `9` = `Move Northeast`;
         * `Ctrl` + `Cursor Up` -> `7` = `Move Northwest`;
         * `Ctrl` + `Cursor Down` -> `3` = `Move Southeast`.

         # Mode 3: L/R Tilt

         * `Shift` + `Cursor Left` -> `7` = `Move Northwest`;
         * `Ctrl` + `Cursor Left` -> `3` = `Move Southeast`;
         * `Shift` + `Cursor Right` -> `9` = `Move Northeast`;
         * `Ctrl` + `Cursor Right` -> `1` = `Move Southwest`.

         */
    translate_marker( "Allows diagonal movement with cursor keys using CTRL and SHIFT modifiers.  Diagonal movement action keys are taken from keybindings, so you need these to be configured." ), { { "none", translate_marker( "None" ) }, { "mode1", translate_marker( "Mode 1: Numpad Emulation" ) }, { "mode2", translate_marker( "Mode 2: CW/CCW" ) }, { "mode3", translate_marker( "Mode 3: L/R Tilt" ) } },
    "none", COPT_CURSES_HIDE );

    add_empty_line();

    add( "VEHICLE_ARMOR_COLOR", interface, translate_marker( "Vehicle plating changes part color" ),
         translate_marker( "If true, vehicle parts will change color if they are armor plated" ),
         true
       );

    add( "DRIVING_VIEW_OFFSET", interface, translate_marker( "Auto-shift the view while driving" ),
         translate_marker( "If true, view will automatically shift towards the driving direction" ),
         true
       );

    add( "VEHICLE_DIR_INDICATOR", interface, translate_marker( "Draw vehicle facing indicator" ),
         translate_marker( "If true, when controlling a vehicle, a white 'X' ( in curses version ) or a crosshair ( in tiles version ) at distance 10 from the center will display its current facing." ),
         true
       );

    add( "REVERSE_STEERING", interface, translate_marker( "Reverse steering direction in reverse" ),
         translate_marker( "If true, when driving a vehicle in reverse, steering should also reverse like real life." ),
         false
       );

    add_empty_line();

    add( "SIDEBAR_POSITION", interface, translate_marker( "Sidebar position" ),
         translate_marker( "Switch between sidebar on the left or on the right side.  Requires restart." ),
         //~ sidebar position
    { { "left", translate_marker( "Left" ) }, { "right", translate_marker( "Right" ) } }, "right"
       );

    add( "SIDEBAR_SPACERS", interface, translate_marker( "Draw sidebar spacers" ),
         translate_marker( "If true, adds an extra space between sidebar panels." ),
         false
       );

    add( "LOG_FLOW", interface, translate_marker( "Message log flow" ),
         translate_marker( "Where new log messages should show." ),
         //~ sidebar/message log flow direction
    { { "new_top", translate_marker( "Top" ) }, { "new_bottom", translate_marker( "Bottom" ) } },
    "new_bottom"
       );

    add( "MESSAGE_TTL", interface, translate_marker( "Sidebar log message display duration" ),
         translate_marker( "Number of turns after which a message will be removed from the sidebar log.  '0' disables this option." ),
         0, 1000, 0
       );

    add( "MESSAGE_COOLDOWN", interface, translate_marker( "Message cooldown" ),
         translate_marker( "Number of turns during which similar messages are hidden.  '0' disables this option." ),
         0, 1000, 0
       );

    add( "MESSAGE_LIMIT", interface, translate_marker( "Limit message history" ),
         translate_marker( "Number of messages to preserve in the history, and when saving." ),
         1, 10000, 255
       );

    add( "NO_UNKNOWN_COMMAND_MSG", interface,
         translate_marker( "Suppress \"unknown command\" messages" ),
         translate_marker( "If true, pressing a key with no set function will not display a notice in the chat log." ),
         false
       );

    add( "LOOKAROUND_POSITION", interface, translate_marker( "Look around position" ),
         translate_marker( "Switch between look around panel being left or right." ),
    { { "left", translate_marker( "Left" ) }, { "right", translate_marker( "Right" ) } },
    "right"
       );

    add( "PICKUP_POSITION", interface, translate_marker( "Pickup position" ),
         translate_marker( "Switch between pickup panel being left, right, or overlapping the sidebar." ),
    { { "left", translate_marker( "Left" ) }, { "right", translate_marker( "Right" ) }, { "overlapping", translate_marker( "Overlapping" ) } },
    "left"
       );

    add( "ACCURACY_DISPLAY", interface, translate_marker( "Aim window display style" ),
         translate_marker( "How should confidence and steadiness be communicated to the player." ),
         //~ aim bar style - bars or numbers
    { { "numbers", translate_marker( "Numbers" ) }, { "bars", translate_marker( "Bars" ) } }, "bars"
       );

    add( "MORALE_STYLE", interface, translate_marker( "Morale style" ),
         translate_marker( "Morale display style in sidebar." ),
    { { "vertical", translate_marker( "Vertical" ) }, { "horizontal", translate_marker( "Horizontal" ) } },
    "Vertical"
       );

    add( "AIM_WIDTH", interface, translate_marker( "Full screen Advanced Inventory Manager" ),
         translate_marker( "If true, Advanced Inventory Manager menu will fit full screen, otherwise it will leave sidebar visible." ),
         false
       );

    add_empty_line();

    add( "MOVE_VIEW_OFFSET", interface, translate_marker( "Move view offset" ),
         translate_marker( "Move view by how many squares per keypress." ),
         1, 50, 1
       );

    add( "FAST_SCROLL_OFFSET", interface, translate_marker( "Overmap fast scroll offset" ),
         translate_marker( "With Fast Scroll option enabled, shift view on the overmap and while looking around by this many squares per keypress." ),
         1, 50, 5
       );

    add( "MENU_SCROLL", interface, translate_marker( "Centered menu scrolling" ),
         translate_marker( "If true, menus will start scrolling in the center of the list, and keep the list centered." ),
         true
       );

    add( "SHIFT_LIST_ITEM_VIEW", interface, translate_marker( "Shift list item view" ),
         translate_marker( "Centered or to edge, shift the view toward the selected item if it is outside of your current viewport." ),
    { { "false", translate_marker( "False" ) }, { "centered", translate_marker( "Centered" ) }, { "edge", translate_marker( "To edge" ) } },
    "centered"
       );

    add( "AUTO_INV_ASSIGN", interface, translate_marker( "Auto inventory letters" ),
         translate_marker( "Enabled: automatically assign letters to any carried items that lack them.  Disabled: do not auto-assign letters.  "
    "Favorites: only auto-assign letters to favorited items." ), {
        { "disabled", translate_marker( "Disabled" ) },
        { "enabled", translate_marker( "Enabled" ) },
        { "favorites", translate_marker( "Favorites" ) }
    },
    "favorites" );

    add( "ITEM_HEALTH_BAR", interface, translate_marker( "Show item health bars" ),
         // NOLINTNEXTLINE(cata-text-style): one space after "etc."
         translate_marker( "If true, show item health bars instead of reinforced, scratched etc. text." ),
         true
       );

    add( "ITEM_SYMBOLS", interface, translate_marker( "Show item symbols" ),
         translate_marker( "If true, show item symbols in inventory and pick up menu." ),
         false
       );
    add( "AMMO_IN_NAMES", interface, translate_marker( "Add ammo to weapon/magazine names" ),
         translate_marker( "If true, the default ammo is added to weapon and magazine names.  For example \"Mosin-Nagant M44 (4/5)\" becomes \"Mosin-Nagant M44 (4/5 7.62x54mm)\"." ),
         true
       );

    add_empty_line();

    add( "ENABLE_JOYSTICK", interface, translate_marker( "Enable joystick" ),
         translate_marker( "Enable input from joystick." ),
         true, COPT_CURSES_HIDE
       );

    add( "HIDE_CURSOR", interface, translate_marker( "Hide mouse cursor" ),
         translate_marker( "Show: Cursor is always shown.  Hide: Cursor is hidden.  HideKB: Cursor is hidden on keyboard input and unhidden on mouse movement." ),
         //~ show mouse cursor
    {   { "show", translate_marker( "Show" ) },
        //~ hide mouse cursor
        { "hide", translate_marker( "Hide" ) },
        //~ hide mouse cursor when keyboard is used
        { "hidekb", translate_marker( "HideKB" ) }
    },
    "show", COPT_CURSES_HIDE );

    add( "EDGE_SCROLL", interface, translate_marker( "Edge scrolling" ),
    translate_marker( "Edge scrolling with the mouse." ), {
        std::make_tuple( -1, translate_marker( "Disabled" ) ),
        std::make_tuple( 100, translate_marker( "Slow" ) ),
        std::make_tuple( 30, translate_marker( "Normal" ) ),
        std::make_tuple( 10, translate_marker( "Fast" ) )
    },
    30, 30, COPT_CURSES_HIDE );

}
