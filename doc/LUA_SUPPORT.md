# LUA SUPPORT

Use the `Home` key to return to the top.

- [LUA SUPPORT](#lua-support)
  - [Introduction](#introduction)
  - [Useful links](#useful-links)
  - [Example mods](#example-mods)
  - [Ingame Lua console](#ingame-lua-console)
  - [Lua hot-reload](#lua-hot-reload)
  - [Game data loading](#game-data-loading)
    - [preload.lua](#preloadlua)
    - [finalize.lua](#finalizelua)
    - [main.lua](#mainlua)
  - [Lua API details](#lua-api-details)
    - [Lua libraries and functions](#lua-libraries-and-functions)
    - [Global state](#global-state)
    - [Game Bindings](#game-bindings)
      - [Global functions](#global-functions)
      - [Hooks](#hooks)
      - [Item use actor](#item-use-actor)
      - [Classes](#classes)
  - [C++ layout](#c-layout)
    - [Lua source files](#lua-source-files)
    - [Sol2 source files](#sol2-source-files)
    - [Game source files](#game-source-files)


## Introduction

This document describes implementation details behind Lua integration in Cataclysm: Bright Nights.

BN uses Lua 5.3.6 to run scripts and relies on sol2 v3.3.0 for bindings on C++ side.

## Useful links

Lua 5.3 Reference Manual: https://www.lua.org/manual/5.3/

Sol2 documentation: https://sol2.readthedocs.io/en/latest/

Programming in Lua (first edition): https://www.lua.org/pil/contents.html

## Example mods

There are a couple heavily-commented example mods in `data/mods/` that make use
of Lua API described here:
* `smart_house_remotes` - Add remotes for controlling garage doors and window curtains.
* `saveload_lua_test` - Mod for testing Lua save/load API.

## Ingame Lua console

In-game Lua console is available through the debug menu or via `Lua Console`
hotkey (unbound by default).

It is rather simple, but is capable of keeping input history, showing output and
errors from Lua scripts as well as running Lua snippets and printing the
returned values.

You can adjust console log capacity by running `set_log_capacity( num )`
(default is 100 entries), or clear it by running `clear_lua_log()`.

TODO: put these functions into some namespace

## Lua hot-reload

To speed up mod development process, BN supports Lua hot-reload functionality.

There is no filesystem watcher, so hot-reload must be triggered manually via a
corresponding `Reload Lua Code` hotkey (unbound by default). The hot-reload can
also be triggered from console window by pressing the corresponding hotkey, or
by running `reload_lua_code()` command. Running the command from regular Lua
scripts may have unintended consequences, use at your own risk!

TODO: put this function into some namespace

Note that not all code can be hot-reloaded, it'll be explained in later sections.

## Game data loading

When a world is being loaded, game does it in roughly these steps:

1. Initializes world-related internal state, sets the world as active
2. Loads world's artifact item types (artifacts are hacky and will likely be removed soon in favor of relics + Lua)
3. Retrieves list of mods used by the world
4. Loads the mods according to the list
5. Initializes avatar-related internal state
6. Loads from save dir actual overmap data, avatar data and reality bubble data

What we care about here is the mod loading stage. It has a number of sub-steps:
1. Loading function receives list of world mods
2. It discards the missing ones and prints debug message for each
3. It checks remaining mods on the list, and throws error if a mod needs Lua, but the game build does NOT support Lua
4. It also throws a warning if game's Lua API version differs from the one used by the mod
5. For every mod on the list that uses Lua, it runs the mod's `preload.lua` script (if present)
6. It goes over all mods in same order as in the list, and loads JSON definitions from each mod's folder
7. It finalizes loaded data (resolves copy-from, prepares some types with complex state for use)
8. For every mod on the list that uses Lua, it runs the mod's `finalize.lua` script (if present)
9. It checks consistency of loaded data (validates values, warns about iffy combinations of values, etc.)
10. (R) For every mod on the list that uses Lua, it runs the mod's `main.lua` script (if present)

As such, we only have 3 scipts to place mod's Lua code into: `preload.lua`, `finalize.lua` and `main.lua`.
The differences and intended use case will be explained below.

You can use only one script, two or all three, depending on your needs.

When executing hot-reload, the game repeats the step marked with (R), so if you
want the code you're working on to be hot-reloadable, put it into `main.lua`.

### preload.lua
This script is supposed to register event hooks and set up definitions that will
then be referred by game JSON loading system (e.g. item use actions).

### finalize.lua
This script is supposed to allow mods to modify definitions loaded from JSON
after copy-from has been resolved, but for now there is no API for this.

TODO: api for finalization

### main.lua
This script is supposed to implement the main logic of the mod.
This includes, but not limited, to:
1. Mod runtime state
2. Mod initialization on game start
3. Mod save/load code, if required
4. Implementation of hooks that were set up in `preload.lua`

## Lua API details

While you can do a lot of interesting stuff with vanilla Lua, the integration
imposes some limits to prevent potential bugs:
- Loading packages (or Lua modules) is disabled. TODO: allow it on data loading
  stage.
- Changes to global state are not available between scripts. If you want mod
  interoperability, use funtions and tables in `game` global table.
- Your mod's runtime state should live in `game.mod_runtime[ game.current_mod ]`
  table. You can also access another mod's state this way if you know its id.

### Lua libraries and functions
When script is called, it comes with some standard Lua libraries pre-loaded:

Library       | Description
--------------|------------
 `base`       | print, assert, and other base functions
 `debug`      | the debug library
 `io`         | input/output library
 `math`       | all things math
 `os`         | functionality from the OS
 `string`     | string library
 `table`      | the table manipulator and observer functions

See `Standard Libraries` section in Lua manual for details.

TODO: restrict some of these or outright get rid of them.

Some of the functions here are overloaded by BN, see [Cata
Bindings](#cata-bindings) for details.

### Global state
Most of necessary data and game runtime state is available through global `game` table.
It has the following members:

game.current_mod            Id of mod that's being loaded (available only when script is executed)
game.active_mods            List of active world mods, in load order
game.mod_runtime.<mod_id>   Runtime data for mods (each mod gets its own table named after its id)
game.mod_storage.<mod_id>   Per-mod storage that gets automatically saved/loaded on game save/load.
game.cata_internal          For internal game purposes, please don't use this

### Game Bindings
TODO: automatic documentation

The game exposes various global functions and classes to Lua.

#### Global functions

Function                | Description
------------------------|-------------
print                   | Print as `INFO LUA` to debug.log (overrides default Lua print)
log_info                | Print as `INFO LUA` to debug.log
log_warn                | Print as `WARNING LUA` to debug.log
log_error               | Print as `ERROR LUA` to debug.log
debugmsg                | Show the fabled "red text message" and also print as `ERROR DEBUGMSG` to debug.log

#### Hooks
TODO

#### Item use actor
TODO

#### Classes
- Avatar
- Character
- Creature
- Monster
- Npc
- Player
- Point

## C++ layout
Lua build can be enabled by passing `LUA=1` to the Makefile, or enabling `LUA` build switch in CMake builds.
Both msvc and android for simplicity always build with Lua **enabled**. 

### Lua source files
To simplify build setup and improve portability we bundle `Lua 5.3.6` source
code in `src/lua/` directory and have the build systems compile it and link into
the game executable and library for tests.

### Sol2 source files
Sol2 makes it easy to bundle, we have `sol2 v3.3.0` single-header amalgamated
version in `src/sol/` and just include it as needed. The header is quite large,
so the less source files include it the better.
* `sol/config.hpp` - Configuration header, we have a few options defined there
* `sol/forward.hpp` - Forward declarations, a lightweight header that should be
  included in game headers instead of `sol/sol.hpp`
* `sol/sol.hpp` - Main sol2 header file, quite large, avoid including in game
  headers

### Game source files
All Lua-related game source files have the `catalua` prefix.

If you want to add new bindings, consider looking at existing examples in
`src/catalua_bindings.cpp` and reading relevant part of Sol2 docs.

* `catalua.h` (and `catalua.cpp`) - Main Lua interface. It's the only header
  most of the codebase will have to include, and it provides a public interface
  that works in both `LUA=1` and `LUA=0` builds ( in builds without Lua, most of
  the functions there are no-op ).
* `catalua_sol.h` and `catalua_sol_fwd.h` - Wrappers for `sol/sol.hpp` and
  `sol/forward.hpp` with custom pragmas to make them compile.
* `catalua_impl.h`(`.cpp`) - Implementation details for `catalua.h`(`.cpp`).
* `catalua_bindings.h`(`.cpp`) - Game Lua bindings live here.
* `catalua_console.h`(`.cpp`) - Ingame Lua console.
* `catalua_log.h`(`.cpp`) - In-memory logging for the console.
* `catalua_serde.h`(`.cpp`) - Lua table <-> JSON convertion.
* `catalua_iuse_actor.h`(`.cpp`) - Lua-driven `iuse_actor`
