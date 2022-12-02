# LUA SUPPORT

Use the `Home` key to return to the top.

- [LUA SUPPORT](#lua-support)
  - [Introduction](#introduction)
  - [Useful links](#useful-links)
    - [Data loading](#data-loading)
  - [Lua layout](#lua-layout)
    - [Lua libraries and functions](#lua-libraries-and-functions)
    - [Game Bindings](#game-bindings)
      - [Global functions](#global-functions)
      - [Classes](#classes)
    - [Global state](#global-state)
  - [C++ layout](#c-layout)
    - [Lua sources](#lua-sources)
    - [Sol2 sources](#sol2-sources)
    - [Game bindings sources](#game-bindings-sources)


## Introduction

This document describes implementation details behind Lua integration in Cataclysm: Bright Nights.

BN uses Lua 5.3.6 to run scripts and relies on sol2 v3.3.0 for bindings on C++ side.

## Useful links

Lua 5.3 Reference Manual: https://www.lua.org/manual/5.3/

Sol2 documentation: https://sol2.readthedocs.io/en/latest/

Programming in Lua (first edition): https://www.lua.org/pil/contents.html

### Data loading

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

As such, we only have 2 opportunities for running Lua scripts on a mod: the `preload.lua` and the `finalize.lua`.

You can use either or both, depending on your needs.

## Lua layout

While you can do a lot of interesting stuff with vanilla Lua, the integration imposes some limits to prevent potential bugs:
- Loading packages (or Lua modules) is disabled. TODO: allow it on data loading stage.
- Changes to global state are not available between mods. If you want mod interoperability, use funtions and tables in `game` global table.
- Your mod's runtime state should live in `game.mod_runtime[ game.current_mod ]` table. You can also access another mod's state this way if you know its id.

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

Some of the functions here are overloaded by BN, see [Cata Bindings](#cata-bindings) for details.

### Game Bindings
The game exposes various global functions and classes to Lua.

#### Global functions

Function                | Description
------------------------|-------------
print                   | Print as `INFO LUA` to debug.log (overrides default Lua print)
log_info                | Print as `INFO LUA` to debug.log
log_warn                | Print as `WARNING LUA` to debug.log
log_error               | Print as `ERROR LUA` to debug.log
debugmsg                | Show the fabled "red text message" and also print as `ERROR DEBUGMSG` to debug.log

#### Classes
TODO: automatic documentation

- Avatar
- Character
- Creature
- Monster
- Npc
- Player
- Point

### Global state
Most of game data is available through global `game` table.
It has the following members:

game.current_mod            Id of mod that's being loaded
game.active_mods            List of active world mods, in load order
game.mod_runtime            Runtime data for mods

## C++ layout
Lua build can be enabled by passing `LUA=1` to the Makefile.
TODO: Cmake builds
TODO: msvc builds

### Lua sources
To simplify build setup and improve portability we bundle `Lua 5.3.6` source code in `src/lua/` directory and have the main Makefile compile it and link into the game executable and library for tests.

### Sol2 sources
Sol2 makes it easy to bundle, we have `sol2 v3.3.0` single-header amalgamated version in `src/sol/` and just include it as needed. The header is quite large, so the less source files include it the better.
* `sol/config.hpp` - Configuration header, we have a few options defined there
* `sol/forward.hpp` - Forward declarations, a lightweight header that should be included in game headers instead of `sol/sol.hpp`
* `sol/sol.hpp` - Main sol2 header file, quite large, avoid including in game headers

### Game bindings sources
If you want to add new bindings, consider looking at existing examples in `src/catalua_bindings.cpp` and reading relevant part of Sol2 docs.

* `catalua_sol.h` and `catalua_sol_fwd.h` - Wrappers for `sol/sol.hpp` and `sol/forward.hpp` with custom pragmas to make them build
* `catalua.h` and `catalua.cpp` - Main Lua interface. It's the only header most of the codebase will have to include, and it provides a public interface that works in both `LUA=1` and `LUA=0` builds ( in builds without Lua, the functions are no-op ).
* `catalua_impl.h` and `catalua_impl.cpp` - Implementation details for `catalua.h` and `catalua.cpp`. Shouldn't be included by outside code.
* `catalua_bindings.h` and `catalua_bindings.cpp` - Lua C++ bindings live here.
