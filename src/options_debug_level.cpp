#include "options_debug_level.h"
#include "translations.h"

// If you change default values here, update documentation in debug.h
// No option for 'DebugMsg' class because it's used only for errors.
extern const std::vector<debug_log_class> debug_log_classes = { {
        {
            DC::Game, "DEBUGLOG_CL_GAME", translate_marker( "Game" ),
            translate_marker( "Messages from main game class." ),
            false
        },
        {
            DC::DebugModeMsg, "DEBUGLOG_CL_DEBUGMODE", translate_marker( "Debug mode" ),
            translate_marker( "Debug-type messages from in-game message log (when debug mode is enabled)." ),
            false
        },
        {
            DC::Main, "DEBUGLOG_CL_MAIN", translate_marker( "Main" ),
            translate_marker( "Generic messages related to game startup and operation." ),
            true
        },
        {
            DC::Map, "DEBUGLOG_CL_MAP", translate_marker( "Map" ),
            translate_marker( "Messages related to map and mapbuffer (map.cpp, mapbuffer.cpp)." ),
            false
        },
        {
            DC::MapGen, "DEBUGLOG_CL_MAPGEN", translate_marker( "Mapgen" ),
            translate_marker( "Messages related to mapgen (mapgen*.cpp) and overmap (overmap.cpp)." ),
            false
        },
        {
            DC::MapMem, "DEBUGLOG_CL_MAPMEM", translate_marker( "Map memory" ),
            translate_marker( "Messages related to tile memory (map_memory.cpp)." ),
            false
        },
        {
            DC::NPC, "DEBUGLOG_CL_NPC", translate_marker( "NPC" ),
            translate_marker( "Messages related to NPCs (npcs*.cpp)." ),
            false
        },
        {
            //~ SDL stands for Simple DirectMedia Layer, leave it as is.
            DC::SDL, "DEBUGLOG_CL_SDL", translate_marker( "SDL" ),
            //~ SDL stands for Simple DirectMedia Layer, leave it as is.
            translate_marker( "Messages related to SDL, tiles, tilesets and sound." ),
            false
        },
        {
            DC::Lua, "DEBUGLOG_CL_LUA", translate_marker( "Lua" ),
            translate_marker( "Messages from Lua scripts." ),
            true
        },
    }
};
// If you change default values here, update documentation in debug.h
// No option for 'Error' level because errors are serious
// and should be enabled at all times.
extern const std::vector<debug_log_level> debug_log_levels = { {
        {
            DL::Debug, "DEBUGLOG_LEV_DEBUG", translate_marker( "Debug" ),
            translate_marker( "Debug information" ),
            false
        },
        {
            DL::Info, "DEBUGLOG_LEV_INFO", translate_marker( "Info" ),
            translate_marker( "General information" ),
            true
        },
        {
            DL::Warn, "DEBUGLOG_LEV_WARNING", translate_marker( "Warning" ),
            translate_marker( "Warnings" ),
            true
        },
    }
};
