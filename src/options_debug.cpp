#include "game_constants.h"
#include "options.h"
#include "options_category.h"
#include "options_debug_level.h"

extern const std::vector<debug_log_level> debug_log_levels;
extern const std::vector<debug_log_class> debug_log_classes;

auto options_manager::add_options_debug() -> void
{
    const auto add_empty_line = [&]() {
        this->add_empty_line( debug );
    };

    add( "STRICT_JSON_CHECKS", debug, translate_marker( "Strict JSON checks" ),
         translate_marker( "If true, will show additional warnings for JSON data correctness." ),
         true
       );

    add( "FORCE_TILESET_RELOAD", debug, translate_marker( "Force tileset reload" ),
         translate_marker( "If false, the game will keep tileset in memory after first load to speed up subsequent loadings of game data.  Enable this if you're working on a tileset for the game or a mod." ),
         false
       );

    add_empty_line();

    add( "MOD_SOURCE", debug, translate_marker( "Display Mod Source" ),
         translate_marker( "Displays what content pack a piece of furniture, terrain, item or monster comes from or is affected by.  Disable if it's annoying." ),
         true
       );

    add( "SHOW_IDS", debug, translate_marker( "Display Object IDs" ),
         translate_marker( "Displays internal IDs of game objects and creatures.  Warning: IDs may contain spoilers." ),
         false
       );

    add_empty_line();

    add_option_group( debug, Group( "debug_log", to_translation( "Logging" ),
                                    to_translation( "Configure debug.log verbosity." ) ),
    [&]( auto & page_id ) {
        for( const debug_log_level &e : debug_log_levels ) {
            add( e.opt_id, page_id, e.opt_name, e.opt_descr, e.opt_default );
        }

        add_empty_line();

        for( const debug_log_class &e : debug_log_classes ) {
            add( e.opt_id, page_id, e.opt_name, e.opt_descr, e.opt_default );
        }
    } );

    add_empty_line();

    add( "DISTANCE_INITIAL_VISIBILITY", debug, translate_marker( "Distance initial visibility" ),
         translate_marker( "Determines the scope, which is known in the beginning of the game." ),
         3, 20, 15
       );

    add( "INITIAL_STAT_POINTS", debug, translate_marker( "Initial stat points" ),
         translate_marker( "Initial points available to spend on stats on character generation." ),
         0, 1000, 6
       );

    add( "INITIAL_TRAIT_POINTS", debug, translate_marker( "Initial trait points" ),
         translate_marker( "Initial points available to spend on traits on character generation." ),
         0, 1000, 0
       );

    add( "INITIAL_SKILL_POINTS", debug, translate_marker( "Initial skill points" ),
         translate_marker( "Initial points available to spend on skills on character generation." ),
         0, 1000, 2
       );

    add( "MAX_TRAIT_POINTS", debug, translate_marker( "Maximum trait points" ),
         translate_marker( "Maximum trait points available for character generation." ),
         0, 1000, 12
       );

    add_empty_line();

    add( "SKILL_TRAINING_SPEED", debug, translate_marker( "Skill training speed" ),
         translate_marker( "Scales experience gained from practicing skills and reading books.  0.5 is half as fast as default, 2.0 is twice as fast, 0.0 disables skill training except for NPC training." ),
         0.0, 100.0, 1.0, 0.1
       );

    add( "SKILL_RUST", debug, translate_marker( "Skill rust" ),
         translate_marker( "Set the level of skill rust.  Vanilla: Vanilla Cataclysm - Capped: Capped at skill levels 2 - Int: Intelligence dependent - IntCap: Intelligence dependent, capped - Off: None at all." ),
         //~ plain, default, normal
    {   { "vanilla", translate_marker( "Vanilla" ) },
        //~ capped at a value
        { "capped", translate_marker( "Capped" ) },
        //~ based on intelligence
        { "int", translate_marker( "Int" ) },
        //~ based on intelligence and capped
        { "intcap", translate_marker( "IntCap" ) },
        { "off", translate_marker( "Off" ) }
    },
    "off" );

    add_empty_line();

    add( "PICKUP_RANGE", debug, translate_marker( "Crafting range" ),
         translate_marker( "Maximum distance at which items are considered available for crafting (or some other actions)." ),
         1, 30, 6
       );

    add_empty_line();

    add( "FOV_3D", debug, translate_marker( "Experimental 3D field of vision" ),
         translate_marker( "If false, vision is limited to current z-level.  If true and the world is in z-level mode, the vision will extend beyond current z-level.  Currently very bugged!" ),
         false
       );

    add( "FOV_3D_Z_RANGE", debug, translate_marker( "Vertical range of 3D field of vision" ),
         translate_marker( "How many levels up and down the experimental 3D field of vision reaches.  (This many levels up, this many levels down.)  3D vision of the full height of the world can slow the game down a lot.  Seeing fewer Z-levels is faster." ),
         0, OVERMAP_LAYERS, 4
       );

    get_option( "FOV_3D_Z_RANGE" ).setPrerequisite( "FOV_3D" );

    add( "ENABLE_EVENTS", debug, translate_marker( "Event bus system" ),
         translate_marker( "If false, achievements and some Magiclysm functionality won't work, but performance will be better." ),
         true
       );

    add( "ELECTRIC_GRID", debug, translate_marker( "Electric grid testing" ),
         translate_marker( "If true, enables somewhat unfinished electric grid system that may slow the game down." ),
         true
       );

    add( "MADE_OF_EXPLODIUM", debug, translate_marker( "Made of explodium" ),
         translate_marker( "Explosive items and traps will detonate when hit by damage exceeding the threshold.  A higher number means more damage is required to detonate.  Set to 0 to disable." ),
         0, 1000, 30 );

    add( "OLD_EXPLOSIONS", debug, translate_marker( "Old explosions system" ),
         translate_marker( "If true, disables new raycasting based explosive system in favor of old system.  With new system obstacles (impassable terrain, furniture or vehicle parts) will block shrapnel, while blast will bash obstacles and throw creatures outward.  If obstacles are destroyed, blast continues outward." ),
         false );

    get_option( "MADE_OF_EXPLODIUM" ).setPrerequisite( "OLD_EXPLOSIONS", "false" );
}
