#include "game_constants.h"
#include "options.h"
#include "options_category.h"

auto options_manager::add_options_general() -> void
{
    const auto add_empty_line = [&]() {
        this->add_empty_line( general );
    };

    add( "PROMPT_ON_CHARACTER_DEATH", general, translate_marker( "Prompt on character death" ),
         translate_marker( "If enabled, when your character dies, the player is given a prompt that gives the option to cancel savefile deletion and other death effects, returning to the main menu without saving instead." ),
         false
       );

    add_empty_line();

    add( "DEF_CHAR_NAME", general, translate_marker( "Default character name" ),
         translate_marker( "Set a default character name that will be used instead of a random name on character creation." ),
         "", 30
       );

    add_empty_line();

    add( "AUTO_PICKUP", general, translate_marker( "Auto pickup enabled" ),
         translate_marker( "Enable item auto pickup.  Change pickup rules with the Auto Pickup Manager." ),
         false
       );

    add( "AUTO_PICKUP_ADJACENT", general, translate_marker( "Auto pickup adjacent" ),
         translate_marker( "If true, will enable to pickup items one tile around to the player.  You can assign No Auto Pickup zones with the Zones Manager 'Y' key for e.g.  your homebase." ),
         false
       );

    get_option( "AUTO_PICKUP_ADJACENT" ).setPrerequisite( "AUTO_PICKUP" );

    add( "AUTO_PICKUP_WEIGHT_LIMIT", general, translate_marker( "Auto pickup weight limit" ),
         translate_marker( "Auto pickup items with weight less than or equal to [option] * 50 grams.  You must also set the small items option.  '0' disables this option" ),
         0, 20, 0
       );

    get_option( "AUTO_PICKUP_WEIGHT_LIMIT" ).setPrerequisite( "AUTO_PICKUP" );

    add( "AUTO_PICKUP_VOL_LIMIT", general, translate_marker( "Auto pickup volume limit" ),
         translate_marker( "Auto pickup items with volume less than or equal to [option] * 50 milliliters.  You must also set the light items option.  '0' disables this option" ),
         0, 20, 0
       );

    get_option( "AUTO_PICKUP_VOL_LIMIT" ).setPrerequisite( "AUTO_PICKUP" );

    add( "AUTO_PICKUP_SAFEMODE", general, translate_marker( "Auto pickup safe mode" ),
         translate_marker( "Auto pickup is disabled as long as you can see monsters nearby.  This is affected by 'Safe Mode proximity distance'." ),
         false
       );

    get_option( "AUTO_PICKUP_SAFEMODE" ).setPrerequisite( "AUTO_PICKUP" );

    add( "NO_AUTO_PICKUP_ZONES_LIST_ITEMS", general,
         translate_marker( "List items within no auto pickup zones" ),
         translate_marker( "If false, you will not see messages about items, you step on, within no auto pickup zones." ),
         true
       );

    get_option( "NO_AUTO_PICKUP_ZONES_LIST_ITEMS" ).setPrerequisite( "AUTO_PICKUP" );

    add_empty_line();

    add( "AUTO_FEATURES", general, translate_marker( "Additional auto features" ),
         translate_marker( "If true, enables configured auto features below.  Disabled as long as any enemy monster is seen." ),
         false
       );

    add( "AUTO_PULP_BUTCHER", general, translate_marker( "Auto pulp or butcher" ),
         translate_marker( "Action to perform when 'Auto pulp or butcher' is enabled.  Pulp: Pulp corpses you stand on.  - Pulp Adjacent: Also pulp corpses adjacent from you.  - Butcher: Butcher corpses you stand on." ),
    { { "off", to_translation( "options", "Disabled" ) }, { "pulp", translate_marker( "Pulp" ) }, { "pulp_adjacent", translate_marker( "Pulp Adjacent" ) }, { "butcher", translate_marker( "Butcher" ) } },
    "off"
       );

    get_option( "AUTO_PULP_BUTCHER" ).setPrerequisite( "AUTO_FEATURES" );

    add( "AUTO_MINING", general, translate_marker( "Auto mining" ),
         translate_marker( "If true, enables automatic use of wielded pickaxes and jackhammers whenever trying to move into mineable terrain." ),
         false
       );

    get_option( "AUTO_MINING" ).setPrerequisite( "AUTO_FEATURES" );

    add( "AUTO_FORAGING", general, translate_marker( "Auto foraging" ),
         translate_marker( "Action to perform when 'Auto foraging' is enabled.  Bushes: Only forage bushes.  - Trees: Only forage trees.  - Everything: Forage bushes, trees, and everything else including flowers, cattails etc." ),
    { { "off", to_translation( "options", "Disabled" ) }, { "bushes", translate_marker( "Bushes" ) }, { "trees", translate_marker( "Trees" ) }, { "both", translate_marker( "Everything" ) } },
    "off"
       );

    get_option( "AUTO_FORAGING" ).setPrerequisite( "AUTO_FEATURES" );

    add_empty_line();

    add( "DANGEROUS_PICKUPS", general, translate_marker( "Dangerous pickups" ),
         translate_marker( "If false, will cause player to drop new items that cause them to exceed the weight limit." ),
         false
       );

    add( "DANGEROUS_TERRAIN_WARNING_PROMPT", general,
         translate_marker( "Dangerous terrain warning prompt" ),
         translate_marker( "Always: You will be prompted to move onto dangerous tiles.  Running: You will only be able to move onto dangerous tiles while running and will be prompted.  Crouching: You will only be able to move onto a dangerous tile while crouching and will be prompted.  Never:  You will not be able to move onto a dangerous tile unless running and will not be warned or prompted.  Ignore:  You will be able to move onto a dangerous tile without any warnings or prompts." ),
    {
        { "ALWAYS", to_translation( "Always" ) },
        { "RUNNING", translate_marker( "Running" ) },
        { "CROUCHING", translate_marker( "Crouching" ) },
        { "NEVER", translate_marker( "Never" ) },
        { "IGNORE", translate_marker( "Ignore" ) }
    },
    "ALWAYS"
       );

    add_empty_line();

    add( "SAFEMODE", general, translate_marker( "Safe mode" ),
         translate_marker( "If true, will hold the game and display a warning if a hostile monster/npc is approaching." ),
         true
       );

    add( "SAFEMODEPROXIMITY", general, translate_marker( "Safe mode proximity distance" ),
         translate_marker( "If safe mode is enabled, distance to hostiles at which safe mode should show a warning.  0 = Max player view distance.  This option only has effect when no safe mode rule is specified.  Otherwise, edit the default rule in Safe Mode Manager instead of this value." ),
         0, MAX_VIEW_DISTANCE, 0
       );

    add( "SAFEMODEVEH", general, translate_marker( "Safe mode when driving" ),
         translate_marker( "When true, safe mode will alert you of hostiles while you are driving a vehicle." ),
         false
       );

    add( "AUTOSAFEMODE", general, translate_marker( "Auto reactivate safe mode" ),
         translate_marker( "If true, safe mode will automatically reactivate after a certain number of turns.  See option 'Turns to auto reactivate safe mode.'" ),
         false
       );

    add( "AUTOSAFEMODETURNS", general, translate_marker( "Turns to auto reactivate safe mode" ),
         translate_marker( "Number of turns after which safe mode is reactivated.  Will only reactivate if no hostiles are in 'Safe mode proximity distance.'" ),
         1, 600, 50
       );

    add( "SAFEMODEIGNORETURNS", general, translate_marker( "Turns to remember ignored monsters" ),
         translate_marker( "Number of turns an ignored monster stays ignored after it is no longer seen.  0 disables this option and monsters are permanently ignored." ),
         0, 3600, 200
       );

    add_empty_line();

    add( "TURN_DURATION", general, translate_marker( "Realtime turn progression" ),
         translate_marker( "If enabled, monsters will take periodic gameplay turns.  This value is the delay between each turn, in seconds.  Works best with Safe Mode disabled.  0 = disabled." ),
         0.0, 10.0, 0.0, 0.05
       );

    add_empty_line();

    add( "AUTOSAVE", general, translate_marker( "Autosave" ),
         translate_marker( "If true, game will periodically save the map.  Autosaves occur based on in-game turns or real-time minutes, whichever is larger." ),
         true
       );

    add( "AUTOSAVE_TURNS", general, translate_marker( "Game turns between autosaves" ),
         translate_marker( "Number of game turns between autosaves" ),
         10, 1000, 50
       );

    get_option( "AUTOSAVE_TURNS" ).setPrerequisite( "AUTOSAVE" );

    add( "AUTOSAVE_MINUTES", general, translate_marker( "Real minutes between autosaves" ),
         translate_marker( "Number of real time minutes between autosaves" ),
         0, 127, 5
       );

    get_option( "AUTOSAVE_MINUTES" ).setPrerequisite( "AUTOSAVE" );

    add_empty_line();

    add( "AUTO_NOTES", general, translate_marker( "Auto notes" ),
         translate_marker( "If true, automatically sets notes" ),
         true
       );

    add( "AUTO_NOTES_STAIRS", general, translate_marker( "Auto notes (stairs)" ),
         translate_marker( "If true, automatically sets notes on places that have stairs that go up or down" ),
         false
       );

    get_option( "AUTO_NOTES_STAIRS" ).setPrerequisite( "AUTO_NOTES" );

    add( "AUTO_NOTES_MAP_EXTRAS", general, translate_marker( "Auto notes (map extras)" ),
         translate_marker( "If true, automatically sets notes on places that contain various map extras" ),
         true
       );

    get_option( "AUTO_NOTES_MAP_EXTRAS" ).setPrerequisite( "AUTO_NOTES" );

    add_empty_line();

    add( "CIRCLEDIST", general, translate_marker( "Circular distances" ),
         translate_marker( "If true, the game will calculate range in a realistic way: light sources will be circles, diagonal movement will cover more ground and take longer.  If disabled, everything is square: moving to the northwest corner of a building takes as long as moving to the north wall." ),
         true
       );

    add( "DROP_EMPTY", general, translate_marker( "Drop empty containers" ),
         translate_marker( "Set to drop empty containers after use.  No: Don't drop any.  - Watertight: All except watertight containers.  - All: Drop all containers." ),
    { { "no", translate_marker( "No" ) }, { "watertight", translate_marker( "Watertight" ) }, { "all", translate_marker( "All" ) } },
    "no"
       );

    add( "DEATHCAM", general, translate_marker( "DeathCam" ),
         translate_marker( "Always: Always start deathcam.  Ask: Query upon death.  Never: Never show deathcam." ),
    { { "always", translate_marker( "Always" ) }, { "ask", translate_marker( "Ask" ) }, { "never", translate_marker( "Never" ) } },
    "ask"
       );

    add_empty_line();

    add( "SOUND_ENABLED", general, translate_marker( "Sound Enabled" ),
         translate_marker( "If true, music and sound are enabled." ),
         true, COPT_NO_SOUND_HIDE
       );

    add( "SOUNDPACKS", general, translate_marker( "Choose soundpack" ),
         translate_marker( "Choose the soundpack you want to use.  Requires restart." ),
         build_soundpacks_list(), "basic", COPT_NO_SOUND_HIDE
       ); // populate the options dynamically

    get_option( "SOUNDPACKS" ).setPrerequisite( "SOUND_ENABLED" );

    add( "MUSIC_VOLUME", general, translate_marker( "Music volume" ),
         translate_marker( "Adjust the volume of the music being played in the background." ),
         0, 128, 100, COPT_NO_SOUND_HIDE
       );

    get_option( "MUSIC_VOLUME" ).setPrerequisite( "SOUND_ENABLED" );

    add( "SOUND_EFFECT_VOLUME", general, translate_marker( "Sound effect volume" ),
         translate_marker( "Adjust the volume of sound effects being played by the game." ),
         0, 128, 100, COPT_NO_SOUND_HIDE
       );

    get_option( "SOUND_EFFECT_VOLUME" ).setPrerequisite( "SOUND_ENABLED" );

    add( "AMBIENT_SOUND_VOLUME", general, translate_marker( "Ambient sound volume" ),
         translate_marker( "Adjust the volume of ambient sounds being played by the game." ),
         0, 128, 100, COPT_NO_SOUND_HIDE
       );

    get_option( "AMBIENT_SOUND_VOLUME" ).setPrerequisite( "SOUND_ENABLED" );
}
