#pragma once
#ifndef CATA_SRC_CACHED_OPTIONS_H
#define CATA_SRC_CACHED_OPTIONS_H

// A collection of options which are accessed frequently enough that we don't
// want to pay the overhead of a string lookup each time one is tested.
// They should be updated when the corresponding option is changed (in options.cpp).

/**
 * Set to true when running in test mode (e.g. unit tests, checking mods).
 * Does not correspond to any game option, but still requires
 * caching due to heavy usage.
 */
extern bool test_mode;

/**
 * Extended debugging mode, can be toggled during game.
 * If enabled some debug messages in the normal player message log are shown,
 * and other windows might have verbose display (e.g. vehicle window).
 */
extern bool debug_mode;

/**
 * Report unused JSON fields in regular (non-test) mode.
 */
extern bool json_report_unused_fields;

/**
 * Report extra problems in JSONs.
 * Because either @ref test_mode or @ref json_report_unused_fields is set.
 */
extern bool json_report_strict;

/**
 * Use tiles for display. Always false for ncurses build,
 * but can be toggled in sdl build.
 */
extern bool use_tiles;

/**
 * Use tiles for 'm'ap display. Always false for ncurses build,
 * but can be toggled in sdl build.
 */
extern bool use_tiles_overmap;

/** Flow direction for the message log in the sidebar. */
extern bool log_from_top;
extern int message_ttl;
extern int message_cooldown;

/**
 * Circular distances.
 * If true, calculate distance in a realistic way [sqrt(dX^2 + dY^2)].
 * If false, use roguelike distance [maximum of dX and dY].
 */
extern bool trigdist;

/** 3D FoV enabled/disabled. */
extern bool fov_3d;

/** 3D FoV range, in Z levels, in both directions. */
extern int fov_3d_z_range;

/** Using isometric tileset. */
extern bool tile_iso;

/** Static z level effect. */
extern bool static_z_effect;

/**
 * Whether to show the pixel minimap. Always false for ncurses build,
 * but can be toggled during game in sdl build.
 */
extern bool pixel_minimap_option;

/**
 * Items on the map with at most this distance to the player are considered
 * available for crafting, see inventory::form_from_map
*/
extern int PICKUP_RANGE;

/**
 * If true, disables all debug messages. Only used for debugging "weird" saves.
 */
extern bool dont_debugmsg;

#endif // CATA_SRC_CACHED_OPTIONS_H
