#include "options.h"
#include "options_category.h"

#if defined(__ANDROID__)
#   include "options_android.h"
#endif

#if defined(__ANDROID__)
bool android_get_default_setting( const char *settings_name, bool default_value )
{
    JNIEnv *env = ( JNIEnv * )SDL_AndroidGetJNIEnv();
    jobject activity = ( jobject )SDL_AndroidGetActivity();
    jclass clazz( env->GetObjectClass( activity ) );
    jmethodID method_id = env->GetMethodID( clazz, "getDefaultSetting", "(Ljava/lang/String;Z)Z" );
    jboolean ans = env->CallBooleanMethod( activity, method_id, env->NewStringUTF( settings_name ),
                                           default_value );
    env->DeleteLocalRef( activity );
    env->DeleteLocalRef( clazz );
    return ans;
}
#endif


auto options_manager::add_options_android() -> void
{
#if defined(__ANDROID__)
    const auto add_empty_line = [&]() {
        this->add_empty_line( android );
    };

    add( "ANDROID_QUICKSAVE", android, translate_marker( "Quicksave on app lose focus" ),
         translate_marker( "If true, quicksave whenever the app loses focus (screen locked, app moved into background etc.) WARNING: Experimental. This may result in corrupt save games." ),
         false
       );

    add_empty_line();

    add( "ANDROID_TRAP_BACK_BUTTON", android, translate_marker( "Trap Back button" ),
         translate_marker( "If true, the back button will NOT back out of the app and will be passed to the application as SDL_SCANCODE_AC_BACK.  Requires restart." ),
         // take default setting from pre-game settings screen - important as there are issues with Back button on Android 9 with specific devices
         android_get_default_setting( "Trap Back button", true )
       );

    add( "ANDROID_AUTO_KEYBOARD", android, translate_marker( "Auto-manage virtual keyboard" ),
         translate_marker( "If true, automatically show/hide the virtual keyboard when necessary based on context. If false, virtual keyboard must be toggled manually." ),
         true
       );

    add( "ANDROID_KEYBOARD_SCREEN_SCALE", android,
         translate_marker( "Virtual keyboard screen scale" ),
         translate_marker( "When the virtual keyboard is visible, scale the screen to prevent overlapping. Useful for text entry so you can see what you're typing." ),
         true
       );

    add_empty_line();

    add( "ANDROID_VIBRATION", android, translate_marker( "Vibration duration" ),
         translate_marker( "If non-zero, vibrate the device for this long on input, in milliseconds. Ignored if hardware keyboard connected." ),
         0, 200, 10
       );

    add_empty_line();

    add( "ANDROID_SHOW_VIRTUAL_JOYSTICK", android, translate_marker( "Show virtual joystick" ),
         translate_marker( "If true, show the virtual joystick when touching and holding the screen. Gives a visual indicator of deadzone and stick deflection." ),
         true
       );

    add( "ANDROID_VIRTUAL_JOYSTICK_OPACITY", android, translate_marker( "Virtual joystick opacity" ),
         translate_marker( "The opacity of the on-screen virtual joystick, as a percentage." ),
         0, 100, 20
       );

    add( "ANDROID_DEADZONE_RANGE", android, translate_marker( "Virtual joystick deadzone size" ),
         translate_marker( "While using the virtual joystick, deflecting the stick beyond this distance will trigger directional input. Specified as a percentage of longest screen edge." ),
         0.01f, 0.2f, 0.03f, 0.001f, COPT_NO_HIDE, "%.3f"
       );

    add( "ANDROID_REPEAT_DELAY_RANGE", android, translate_marker( "Virtual joystick size" ),
         translate_marker( "While using the virtual joystick, deflecting the stick by this much will repeat input at the deflected rate (see below). Specified as a percentage of longest screen edge." ),
         0.05f, 0.5f, 0.10f, 0.001f, COPT_NO_HIDE, "%.3f"
       );

    add( "ANDROID_VIRTUAL_JOYSTICK_FOLLOW", android,
         translate_marker( "Virtual joystick follows finger" ),
         translate_marker( "If true, the virtual joystick will follow when sliding beyond its range." ),
         false
       );

    add( "ANDROID_REPEAT_DELAY_MAX", android,
         translate_marker( "Virtual joystick repeat rate (centered)" ),
         translate_marker( "When the virtual joystick is centered, how fast should input events repeat, in milliseconds." ),
         50, 1000, 500
       );

    add( "ANDROID_REPEAT_DELAY_MIN", android,
         translate_marker( "Virtual joystick repeat rate (deflected)" ),
         translate_marker( "When the virtual joystick is fully deflected, how fast should input events repeat, in milliseconds." ),
         50, 1000, 100
       );

    add( "ANDROID_SENSITIVITY_POWER", android,
         translate_marker( "Virtual joystick repeat rate sensitivity" ),
         translate_marker( "As the virtual joystick moves from centered to fully deflected, this value is an exponent that controls the blend between the two repeat rates defined above. 1.0 = linear." ),
         0.1f, 5.0f, 0.75f, 0.05f, COPT_NO_HIDE, "%.2f"
       );

    add( "ANDROID_INITIAL_DELAY", android, translate_marker( "Input repeat delay" ),
         translate_marker( "While touching the screen, wait this long before showing the virtual joystick and repeating input, in milliseconds. Also used to determine tap/double-tap detection, flick detection and toggling quick shortcuts." ),
         150, 1000, 300
       );

    add( "ANDROID_HIDE_HOLDS", android, translate_marker( "Virtual joystick hides shortcuts" ),
         translate_marker( "If true, hides on-screen keyboard shortcuts while using the virtual joystick. Helps keep the view uncluttered while traveling long distances and navigating menus." ),
         true
       );

    add_empty_line();

    add( "ANDROID_SHORTCUT_DEFAULTS", android, translate_marker( "Default gameplay shortcuts" ),
         translate_marker( "The default set of gameplay shortcuts to show. Used on starting a new game and whenever all gameplay shortcuts are removed." ),
         "0mi", 30
       );

    add( "ANDROID_ACTIONMENU_AUTOADD", android,
         translate_marker( "Add shortcuts for action menu selections" ),
         translate_marker( "If true, automatically add a shortcut for actions selected via the in-game action menu." ),
         true
       );

    add( "ANDROID_INVENTORY_AUTOADD", android,
         translate_marker( "Add shortcuts for inventory selections" ),
         translate_marker( "If true, automatically add a shortcut for items selected via the inventory." ),
         true
       );

    add_empty_line();

    add( "ANDROID_TAP_KEY", android, translate_marker( "Tap key (in-game)" ),
         translate_marker( "The key to press when tapping during gameplay." ),
         ".", 1
       );

    add( "ANDROID_2_TAP_KEY", android, translate_marker( "Two-finger tap key (in-game)" ),
         translate_marker( "The key to press when tapping with two fingers during gameplay." ),
         "i", 1
       );

    add( "ANDROID_2_SWIPE_UP_KEY", android, translate_marker( "Two-finger swipe up key (in-game)" ),
         translate_marker( "The key to press when swiping up with two fingers during gameplay." ),
         "K", 1
       );

    add( "ANDROID_2_SWIPE_DOWN_KEY", android,
         translate_marker( "Two-finger swipe down key (in-game)" ),
         translate_marker( "The key to press when swiping down with two fingers during gameplay." ),
         "J", 1
       );

    add( "ANDROID_2_SWIPE_LEFT_KEY", android,
         translate_marker( "Two-finger swipe left key (in-game)" ),
         translate_marker( "The key to press when swiping left with two fingers during gameplay." ),
         "L", 1
       );

    add( "ANDROID_2_SWIPE_RIGHT_KEY", android,
         translate_marker( "Two-finger swipe right key (in-game)" ),
         translate_marker( "The key to press when swiping right with two fingers during gameplay." ),
         "H", 1
       );

    add( "ANDROID_PINCH_IN_KEY", android, translate_marker( "Pinch in key (in-game)" ),
         translate_marker( "The key to press when pinching in during gameplay." ),
         "Z", 1
       );

    add( "ANDROID_PINCH_OUT_KEY", android, translate_marker( "Pinch out key (in-game)" ),
         translate_marker( "The key to press when pinching out during gameplay." ),
         "z", 1
       );

    add_empty_line();

    add( "ANDROID_SHORTCUT_AUTOADD", android,
         translate_marker( "Auto-manage contextual gameplay shortcuts" ),
         translate_marker( "If true, contextual in-game shortcuts are added and removed automatically as needed: examine, close, butcher, move up/down, control vehicle, pickup, toggle enemy + safe mode, sleep." ),
         true
       );

    add( "ANDROID_SHORTCUT_AUTOADD_FRONT", android,
         translate_marker( "Move contextual gameplay shortcuts to front" ),
         translate_marker( "If the above option is enabled, specifies whether contextual in-game shortcuts will be added to the front or back of the shortcuts list. True makes them easier to reach, False reduces shuffling of shortcut positions." ),
         false
       );

    add( "ANDROID_SHORTCUT_MOVE_FRONT", android, translate_marker( "Move used shortcuts to front" ),
         translate_marker( "If true, using an existing shortcut will always move it to the front of the shortcuts list. If false, only shortcuts typed via keyboard will move to the front." ),
         false
       );

    add( "ANDROID_SHORTCUT_ZONE", android,
         translate_marker( "Separate shortcuts for No Auto Pickup zones" ),
         translate_marker( "If true, separate gameplay shortcuts will be used within No Auto Pickup zones. Useful for keeping home base actions separate from exploring actions." ),
         true
       );

    add( "ANDROID_SHORTCUT_REMOVE_TURNS", android,
         translate_marker( "Turns to remove unused gameplay shortcuts" ),
         translate_marker( "If non-zero, unused gameplay shortcuts will be removed after this many turns (as in discrete player actions, not world calendar turns)." ),
         0, 1000, 0
       );

    add( "ANDROID_SHORTCUT_PERSISTENCE", android, translate_marker( "Shortcuts persistence" ),
         translate_marker( "If true, shortcuts are saved/restored with each save game. If false, shortcuts reset between sessions." ),
         true
       );

    add_empty_line();

    add( "ANDROID_SHORTCUT_POSITION", android, translate_marker( "Shortcuts position" ),
         translate_marker( "Switch between shortcuts on the left or on the right side of the screen." ),
    { { "left", translate_marker( "Left" ) }, { "right", translate_marker( "Right" ) } }, "left"
       );

    add( "ANDROID_SHORTCUT_SCREEN_PERCENTAGE", android,
         translate_marker( "Shortcuts screen percentage" ),
         translate_marker( "How much of the screen can shortcuts occupy, as a percentage of total screen width." ),
         10, 100, 100
       );

    add( "ANDROID_SHORTCUT_OVERLAP", android, translate_marker( "Shortcuts overlap screen" ),
         translate_marker( "If true, shortcuts will be drawn transparently overlapping the game screen. If false, the game screen size will be reduced to fit the shortcuts below." ),
         true
       );

    add( "ANDROID_SHORTCUT_OPACITY_BG", android, translate_marker( "Shortcut opacity (background)" ),
         translate_marker( "The background opacity of on-screen keyboard shortcuts, as a percentage." ),
         0, 100, 75
       );

    add( "ANDROID_SHORTCUT_OPACITY_SHADOW", android, translate_marker( "Shortcut opacity (shadow)" ),
         translate_marker( "The shadow opacity of on-screen keyboard shortcuts, as a percentage." ),
         0, 100, 100
       );

    add( "ANDROID_SHORTCUT_OPACITY_FG", android, translate_marker( "Shortcut opacity (text)" ),
         translate_marker( "The foreground opacity of on-screen keyboard shortcuts, as a percentage." ),
         0, 100, 100
       );

    add( "ANDROID_SHORTCUT_COLOR", android, translate_marker( "Shortcut color" ),
         translate_marker( "The color of on-screen keyboard shortcuts." ),
         0, 15, 15
       );

    add( "ANDROID_SHORTCUT_BORDER", android, translate_marker( "Shortcut border" ),
         translate_marker( "The border of each on-screen keyboard shortcut in pixels. ." ),
         0, 16, 0
       );

    add( "ANDROID_SHORTCUT_WIDTH_MIN", android, translate_marker( "Shortcut width (min)" ),
         translate_marker( "The minimum width of each on-screen keyboard shortcut in pixels. Only relevant when lots of shortcuts are visible at once." ),
         20, 1000, 50
       );

    add( "ANDROID_SHORTCUT_WIDTH_MAX", android, translate_marker( "Shortcut width (max)" ),
         translate_marker( "The maximum width of each on-screen keyboard shortcut in pixels." ),
         50, 1000, 160
       );

    add( "ANDROID_SHORTCUT_HEIGHT", android, translate_marker( "Shortcut height" ),
         translate_marker( "The height of each on-screen keyboard shortcut in pixels." ),
         50, 1000, 130
       );

#endif
}
