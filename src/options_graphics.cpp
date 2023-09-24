#include "options.h"
#include "options_category.h"

#if defined(TILES)
#include "cata_tiles.h"
#endif // TILES

#if defined(__ANDROID__)
#   include "options_android.h"
#endif

auto options_manager::add_options_graphics() -> void
{
    const auto add_empty_line = [&]() {
        this->add_empty_line( graphics );
    };

    add( "ANIMATIONS", graphics, translate_marker( "Animations" ),
         translate_marker( "If true, will display enabled animations." ),
         true
       );

    add( "ANIMATION_RAIN", graphics, translate_marker( "Rain animation" ),
         translate_marker( "If true, will display weather animations." ),
         true
       );

    get_option( "ANIMATION_RAIN" ).setPrerequisite( "ANIMATIONS" );

    add( "ANIMATION_PROJECTILES", graphics, translate_marker( "Projectile animation" ),
         translate_marker( "If true, will display animations for projectiles like bullets, arrows, and thrown items." ),
         true
       );

    get_option( "ANIMATION_PROJECTILES" ).setPrerequisite( "ANIMATIONS" );

    add( "ANIMATION_SCT", graphics, translate_marker( "SCT animation" ),
         translate_marker( "If true, will display scrolling combat text animations." ),
         true
       );

    get_option( "ANIMATION_SCT" ).setPrerequisite( "ANIMATIONS" );

    add( "ANIMATION_SCT_USE_FONT", graphics, translate_marker( "SCT with Unicode font" ),
         translate_marker( "If true, will display scrolling combat text with Unicode font." ),
         true
       );

    get_option( "ANIMATION_SCT_USE_FONT" ).setPrerequisite( "ANIMATION_SCT" );

    add( "ANIMATION_DELAY", graphics, translate_marker( "Animation delay" ),
         translate_marker( "The amount of time to pause between animation frames in ms." ),
         0, 100, 10
       );

    get_option( "ANIMATION_DELAY" ).setPrerequisite( "ANIMATIONS" );

    add( "BULLETS_AS_LASERS", graphics, translate_marker( "Draw bullets as lines" ),
         translate_marker( "If true, bullets are drawn as lines of images, and the animation lasts only one frame." ),
         false
       );

    add( "BLINK_SPEED", graphics, translate_marker( "Blinking effects speed" ),
         translate_marker( "The speed of every blinking effects in ms." ),
         100, 5000, 800
       );

    add( "FORCE_REDRAW", graphics, translate_marker( "Force redraw" ),
         translate_marker( "If true, forces the game to redraw at least once per turn." ),
         true
       );

    add_empty_line();

    add( "TERMINAL_X", graphics, translate_marker( "Terminal width" ),
         translate_marker( "Set the size of the terminal along the X axis." ),
         80, 960, 80, COPT_POSIX_CURSES_HIDE
       );

    add( "TERMINAL_Y", graphics, translate_marker( "Terminal height" ),
         translate_marker( "Set the size of the terminal along the Y axis." ),
         24, 270, 24, COPT_POSIX_CURSES_HIDE
       );

    add_empty_line();

#if defined(TILES)
    add_option_group( graphics, Group( "font_params", to_translation( "Font settings" ),
                                       to_translation( "Font display settings.  To change font type or source file, edit fonts.json in config directory." ) ),
    [&]( auto & page_id ) {
        add( "USE_DRAW_ASCII_LINES_ROUTINE", page_id, translate_marker( "SDL ASCII lines" ),
             translate_marker( "Use SDL ASCII line drawing routine instead of Unicode Line Drawing characters.  Use this option when your selected font doesn't contain necessary glyphs." ),
             true, COPT_CURSES_HIDE );

        add( "FONT_BLENDING", page_id, translate_marker( "Font blending" ),
             translate_marker( "If true, fonts will look better." ),
             false, COPT_CURSES_HIDE );

        add( "FONT_WIDTH", page_id, translate_marker( "Font width" ),
             translate_marker( "Set the font width. Requires restart." ),
             8, 100, 8, COPT_CURSES_HIDE );

        static auto font_size_options = std::array<std::array<std::string, 3>, 8> {{
                {"FONT_HEIGHT",         translate_marker( "Font height" ),         translate_marker( "Set the font height.  Requires restart." )},
                {"FONT_SIZE",           translate_marker( "Font size" ),           translate_marker( "Set the font size.  Requires restart." )},
                {"MAP_FONT_WIDTH",      translate_marker( "Map font width" ),      translate_marker( "Set the map font width.  Requires restart." )},
                {"MAP_FONT_HEIGHT",     translate_marker( "Map font height" ),     translate_marker( "Set the map font height.  Requires restart." )},
                {"MAP_FONT_SIZE",       translate_marker( "Map font size" ),       translate_marker( "Set the map font size.  Requires restart." )},
                {"OVERMAP_FONT_WIDTH",  translate_marker( "Overmap font width" ),  translate_marker( "Set the overmap font width.  Requires restart." )},
                {"OVERMAP_FONT_HEIGHT", translate_marker( "Overmap font height" ), translate_marker( "Set the overmap font height.  Requires restart." )},
                {"OVERMAP_FONT_SIZE",   translate_marker( "Overmap font size" ),   translate_marker( "Set the overmap font size.  Requires restart." )}
            }
        };
        for( auto &&[option, option_name, option_desc] : font_size_options ) {
            add( option, page_id, option_name, option_desc, 8, 100, 16, COPT_CURSES_HIDE );
        }
    } );
#endif // TILES

    add( "ENABLE_ASCII_ART_ITEM", graphics,
         translate_marker( "Enable ASCII art in item descriptions" ),
         translate_marker( "When available item description will show a picture of the item in ascii art." ),
         true, COPT_NO_HIDE
       );

    add_empty_line();

    add( "USE_TILES", graphics, translate_marker( "Use tiles" ),
         translate_marker( "If true, replaces some TTF rendered text with tiles." ),
         true, COPT_CURSES_HIDE
       );

    add( "TILES", graphics, translate_marker( "Choose tileset" ),
         translate_marker( "Choose the tileset you want to use." ),
         build_tilesets_list(), "UNDEAD_PEOPLE_BASE", COPT_CURSES_HIDE
       ); // populate the options dynamically

    get_option( "TILES" ).setPrerequisite( "USE_TILES" );

    add( "USE_TILES_OVERMAP", graphics, translate_marker( "Use tiles to display overmap" ),
         translate_marker( "If true, replaces some TTF-rendered text with tiles for overmap display." ),
         true, COPT_CURSES_HIDE
       );

    get_option( "USE_TILES_OVERMAP" ).setPrerequisite( "USE_TILES" );

    add_empty_line();

    add( "MEMORY_MAP_MODE", graphics, translate_marker( "Memory map drawing mode" ),
    translate_marker( "Specified the mode in which the memory map is drawn." ), {
        { "color_pixel_darken", translate_marker( "Darkened" ) },
        { "color_pixel_sepia", translate_marker( "Sepia" ) }
    }, "color_pixel_sepia", COPT_CURSES_HIDE
       );

    add( "STATICZEFFECT", graphics, translate_marker( "Static z level effect" ),
         translate_marker( "If true, lower z levels will look the same no matter how far down they are.  Increases rendering performance." ),
         false, COPT_CURSES_HIDE
       );

    add_empty_line();

    add( "PIXEL_MINIMAP", graphics, translate_marker( "Pixel minimap" ),
         translate_marker( "If true, shows the pixel-detail minimap in game after the save is loaded.  Use the 'Toggle Pixel Minimap' action key to change its visibility during gameplay." ),
         true, COPT_CURSES_HIDE
       );

    add( "PIXEL_MINIMAP_MODE", graphics, translate_marker( "Pixel minimap drawing mode" ),
    translate_marker( "Specified the mode in which the minimap drawn." ), {
        { "solid", translate_marker( "Solid" ) },
        { "squares", translate_marker( "Squares" ) },
        { "dots", translate_marker( "Dots" ) }
    }, "dots", COPT_CURSES_HIDE
       );

    get_option( "PIXEL_MINIMAP_MODE" ).setPrerequisite( "PIXEL_MINIMAP" );

    add( "PIXEL_MINIMAP_BRIGHTNESS", graphics, translate_marker( "Pixel minimap brightness" ),
         translate_marker( "Overall brightness of pixel-detail minimap." ),
         10, 300, 100, COPT_CURSES_HIDE
       );

    get_option( "PIXEL_MINIMAP_BRIGHTNESS" ).setPrerequisite( "PIXEL_MINIMAP" );

    add( "PIXEL_MINIMAP_HEIGHT", graphics, translate_marker( "Pixel minimap height" ),
         translate_marker( "Height of pixel-detail minimap, measured in terminal rows.  Set to 0 for default spacing." ),
         0, 100, 0, COPT_CURSES_HIDE
       );

    get_option( "PIXEL_MINIMAP_HEIGHT" ).setPrerequisite( "PIXEL_MINIMAP" );

    add( "PIXEL_MINIMAP_SCALE_TO_FIT", graphics, translate_marker( "Scale pixel minimap" ),
         translate_marker( "Scale pixel minimap to fit its surroundings.  May produce crappy results, especially in modes other than \"Solid\"." ),
         false, COPT_CURSES_HIDE
       );

    get_option( "PIXEL_MINIMAP_SCALE_TO_FIT" ).setPrerequisite( "PIXEL_MINIMAP" );

    add( "PIXEL_MINIMAP_RATIO", graphics, translate_marker( "Maintain pixel minimap aspect ratio" ),
         translate_marker( "Preserves the square shape of tiles shown on the pixel minimap." ),
         true, COPT_CURSES_HIDE
       );

    get_option( "PIXEL_MINIMAP_RATIO" ).setPrerequisite( "PIXEL_MINIMAP" );

    add( "PIXEL_MINIMAP_BEACON_SIZE", graphics,
         translate_marker( "Creature beacon size" ),
         translate_marker( "Controls how big the creature beacons are.  Value is in minimap tiles." ),
         1, 4, 2, COPT_CURSES_HIDE
       );

    get_option( "PIXEL_MINIMAP_BEACON_SIZE" ).setPrerequisite( "PIXEL_MINIMAP" );

    add( "PIXEL_MINIMAP_BLINK", graphics, translate_marker( "Hostile creature beacon blink speed" ),
         translate_marker( "Controls how fast the hostile creature beacons blink on the pixel minimap.  Value is multiplied by 200 ms.  Set to 0 to disable." ),
         0, 50, 10, COPT_CURSES_HIDE
       );

    get_option( "PIXEL_MINIMAP_BLINK" ).setPrerequisite( "PIXEL_MINIMAP" );

    add_empty_line();

#if defined(TILES)
    std::vector<options_manager::id_and_option> display_list = cata_tiles::build_display_list();
    add( "DISPLAY", graphics, translate_marker( "Display" ),
         translate_marker( "Sets which video display will be used to show the game.  Requires restart." ),
         display_list,
         display_list.front().first, COPT_CURSES_HIDE );
#endif

#if !defined(__ANDROID__) // Android is always fullscreen
    add( "FULLSCREEN", graphics, translate_marker( "Fullscreen" ),
         translate_marker( "Starts Cataclysm in one of the fullscreen modes.  Requires restart." ),
    { { "no", translate_marker( "No" ) }, { "maximized", translate_marker( "Maximized" ) }, { "fullscreen", translate_marker( "Fullscreen" ) }, { "windowedbl", translate_marker( "Windowed borderless" ) } },
    "windowedbl", COPT_CURSES_HIDE
       );

    add( "MINIMIZE_ON_FOCUS_LOSS", graphics,
         translate_marker( "Minimize on focus loss" ),
         translate_marker( "Minimize fullscreen window when it loses focus.  Requires restart." ), false );
#endif

#if !defined(__ANDROID__)
#   if !defined(TILES)
    // No renderer selection in non-TILES mode
    add( "RENDERER", graphics, translate_marker( "Renderer" ),
    translate_marker( "Set which renderer to use.  Requires restart." ),   {   { "software", translate_marker( "software" ) } },
    "software", COPT_CURSES_HIDE );
#   else
    std::vector<options_manager::id_and_option> renderer_list = cata_tiles::build_renderer_list();
    std::string default_renderer = renderer_list.front().first;
#   if defined(_WIN32)
    for( const id_and_option &renderer : renderer_list ) {
        if( renderer.first == "direct3d11" ) {
            default_renderer = renderer.first;
            break;
        }
    }
#   endif
    add( "RENDERER", graphics, translate_marker( "Renderer" ),
         translate_marker( "Set which renderer to use.  Requires restart." ), renderer_list,
         default_renderer, COPT_CURSES_HIDE );
#   endif

#else
    add( "SOFTWARE_RENDERING", graphics, translate_marker( "Software rendering" ),
         translate_marker( "Use software renderer instead of graphics card acceleration.  Requires restart." ),
         // take default setting from pre-game settings screen - important as both software + hardware rendering have issues with specific devices
         android_get_default_setting( "Software rendering", false ),
         COPT_CURSES_HIDE
       );
#endif

#if defined(SDL_HINT_RENDER_BATCHING)
    add( "RENDER_BATCHING", graphics, translate_marker( "Allow render batching" ),
         translate_marker( "Use render batching for 2D render API to make it more efficient.  Requires restart." ),
         true, COPT_CURSES_HIDE
       );
#endif
    add( "FRAMEBUFFER_ACCEL", graphics, translate_marker( "Software framebuffer acceleration" ),
         translate_marker( "Use hardware acceleration for the framebuffer when using software rendering.  Requires restart." ),
         false, COPT_CURSES_HIDE
       );

#if defined(__ANDROID__)
    get_option( "FRAMEBUFFER_ACCEL" ).setPrerequisite( "SOFTWARE_RENDERING" );
#else
    get_option( "FRAMEBUFFER_ACCEL" ).setPrerequisite( "RENDERER", "software" );
#endif

    add( "USE_COLOR_MODULATED_TEXTURES", graphics, translate_marker( "Use color modulated textures" ),
         translate_marker( "If true, tries to use color modulated textures to speed-up ASCII drawing.  Requires restart." ),
         false, COPT_CURSES_HIDE
       );

#if !defined(__ANDROID__)
    add( "SCALING_FACTOR", graphics, translate_marker( "Display scaling factor" ),
    translate_marker( "Factor by which to scale the game display, 1x means no scaling.  Requires restart." ), {
        { "1", translate_marker( "1x" ) },
        { "2", translate_marker( "2x" ) },
        { "4", translate_marker( "4x" ) }
    },
    "1", COPT_CURSES_HIDE );
#endif

}
