#include "options.h"
#include "options_category.h"

void options_manager::add_options_world_default()
{
    const auto add_empty_line = [&]() {
        this->add_empty_line( world_default );
    };

    add_empty_line();

    add( "WORLD_END", world_default, translate_marker( "World end handling" ),
    translate_marker( "Handling of game world when last character dies." ), {
        { "reset", translate_marker( "Reset" ) }, { "delete", translate_marker( "Delete" ) },
        { "query", translate_marker( "Query" ) }, { "keep", translate_marker( "Keep" ) }
    }, "reset"
       );

    add_empty_line();

    add( "CITY_SIZE", world_default, translate_marker( "Size of cities" ),
         translate_marker( "A number determining how large cities are.  0 disables cities, roads and any scenario requiring a city start." ),
         0, 16, 8
       );

    add( "CITY_SPACING", world_default, translate_marker( "City spacing" ),
         translate_marker( "A number determining how far apart cities are.  Warning, small numbers lead to very slow mapgen." ),
         0, 8, 4
       );

    add( "SPAWN_DENSITY", world_default, translate_marker( "Spawn rate scaling factor" ),
         translate_marker( "A scaling factor that determines density of monster spawns." ),
         0.0, 50.0, 1.0, 0.1
       );
    add( "SPAWN_ANIMAL_DENSITY", world_default, translate_marker( "Animal spawn rate scaling factor" ),
         translate_marker( "A scaling factor that determines density of animal spawns." ),
         0.0, 50.0, 1.0, 0.1
       );

    add( "CARRION_SPAWNRATE", world_default, translate_marker( "Carrion spawn rate scaling factor" ),
         translate_marker( "A scaling factor that determines how often creatures spawn from rotting material." ),
         0.0, 10.0, 1.0, 0.01, COPT_NO_HIDE
       );

    add( "ITEM_SPAWNRATE", world_default, translate_marker( "Item spawn scaling factor" ),
         translate_marker( "A scaling factor that determines density of item spawns." ),
         0.01, 10.0, 1.0, 0.01
       );

    add( "NPC_DENSITY", world_default, translate_marker( "NPC spawn rate scaling factor" ),
         translate_marker( "A scaling factor that determines density of dynamic NPC spawns." ),
         0.0, 100.0, 0.1, 0.01
       );

    add( "MONSTER_UPGRADE_FACTOR", world_default,
         translate_marker( "Monster evolution scaling factor" ),
         translate_marker( "A scaling factor that determines the time between monster upgrades.  A higher number means slower evolution.  Set to 0.00 to turn off monster upgrades." ),
         0.0, 100, 2.0, 0.01
       );

    add_empty_line();

    add( "MONSTER_SPEED", world_default, translate_marker( "Monster speed" ),
         translate_marker( "Determines the movement rate of monsters.  A higher value increases monster speed and a lower reduces it.  Requires world reset." ),
         1, 1000, 100, COPT_NO_HIDE, "%i%%"
       );

    add( "MONSTER_RESILIENCE", world_default, translate_marker( "Monster resilience" ),
         translate_marker( "Determines how much damage monsters can take.  A higher value makes monsters more resilient and a lower makes them more flimsy.  Requires world reset." ),
         1, 1000, 100, COPT_NO_HIDE, "%i%%"
       );

    add_empty_line();

    add( "DEFAULT_REGION", world_default, translate_marker( "Default region type" ),
         translate_marker( "( WIP feature ) Determines terrain, shops, plants, and more." ),
    { { "default", "default" } }, "default"
       );

    add_empty_line();

    add( "INITIAL_TIME", world_default, translate_marker( "Initial time" ),
         translate_marker( "Initial starting time of day on character generation." ),
         0, 23, 8
       );

    add( "INITIAL_DAY", world_default, translate_marker( "Initial day" ),
         translate_marker( "How many days into the year the cataclysm occurred.  Day 0 is Spring 1.  Day -1 randomizes the start date.  Can be overridden by scenarios.  This does not advance food rot or monster evolution." ),
         -1, 999, 7
       );

    add( "SPAWN_DELAY", world_default, translate_marker( "Spawn delay" ),
         translate_marker( "How many days after the cataclysm the player spawns.  Day 0 is the day of the cataclysm.  Can be overridden by scenarios.  Increasing this will cause food rot and monster evolution to advance." ),
         0, 9999, 0
       );

    add( "SEASON_LENGTH", world_default, translate_marker( "Season length" ),
         translate_marker( "Season length, in days." ),
         14, 127, 14
       );

    add( "CONSTRUCTION_SCALING", world_default, translate_marker( "Construction scaling" ),
         translate_marker( "Sets the time of construction in percents.  '50' is two times faster than default, '200' is two times longer.  '0' automatically scales construction time to match the world's season length." ),
         0, 1000, 100
       );

    add( "ETERNAL_SEASON", world_default, translate_marker( "Eternal season" ),
         translate_marker( "Keep the initial season for ever." ),
         false
       );

    add_empty_line();

    add( "WANDER_SPAWNS", world_default, translate_marker( "Wander spawns" ),
         translate_marker( "Emulation of zombie hordes.  Zombie spawn points wander around cities and may go to noise.  Must reset world directory after changing for it to take effect." ),
         false
       );

    add( "BLACK_ROAD", world_default, translate_marker( "Surrounded start" ),
         translate_marker( "If true, spawn zombies at shelters.  Makes the starting game a lot harder." ),
         false
       );

    add_empty_line();

    add( "STATIC_NPC", world_default, translate_marker( "Static NPCs" ),
         translate_marker( "If true, static NPCs will spawn at pre-defined locations.  Requires world reset." ),
         true
       );

    add( "STARTING_NPC", world_default, translate_marker( "Starting NPCs spawn" ),
         translate_marker( "Determines whether starting NPCs should spawn, and if they do, how exactly." ),
    { { "never", translate_marker( "Never" ) }, { "always", translate_marker( "Always" ) }, { "scenario", translate_marker( "Scenario-based" ) } },
    "scenario"
       );

    get_option( "STARTING_NPC" ).setPrerequisite( "STATIC_NPC" );

    add( "RANDOM_NPC", world_default, translate_marker( "Random NPCs" ),
         translate_marker( "If true, the game will randomly spawn NPCs during gameplay." ),
         false
       );

    add_empty_line();

    add( "RAD_MUTATION", world_default, translate_marker( "Mutations by radiation" ),
         translate_marker( "If true, radiation causes the player to mutate." ),
         true
       );

    add_empty_line();

    add( "ZLEVELS", world_default, translate_marker( "Z-levels" ),
         translate_marker( "If true, enables several features related to vertical movement, such as hauling items up stairs, climbing downspouts, flying aircraft and ramps.  May cause problems if toggled mid-game." ),
         true
       );

    add_empty_line();

    add( "CHARACTER_POINT_POOLS", world_default, translate_marker( "Character point pools" ),
         translate_marker( "Allowed point pools for character generation." ),
    { { "any", translate_marker( "Any" ) }, { "multi_pool", translate_marker( "Multi-pool only" ) }, { "no_freeform", translate_marker( "No freeform" ) } },
    "any"
       );

    add( "DISABLE_LIFTING", world_default,
         translate_marker( "Disables lifting requirements for vehicle parts." ),
         translate_marker( "If true, strength checks and/or lifting qualities no longer need to be met in order to change parts." ),
         false, COPT_ALWAYS_HIDE
       );

    add( "ELEVATED_BRIDGES", world_default,
         translate_marker( "Generate elevated bridges." ),
         translate_marker( "If true, bridges are generated at z+1 level, allowing boats to pass underneath." ),
         false, COPT_ALWAYS_HIDE
       );
}
