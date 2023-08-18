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
      - [Global overrides](#global-overrides)
      - [Hooks](#hooks)
      - [Item use function](#item-use-function)
      - [Translation functions](#translation-functions)
  - [C++ layout](#c-layout)
    - [Lua source files](#lua-source-files)
    - [Sol2 source files](#sol2-source-files)
    - [Game source files](#game-source-files)
    - [Adding new type to the doc generator without binding internals](#adding-new-type-to-the-doc-generator-without-binding-internals)
    - [Binding new type to Lua](#binding-new-type-to-lua)
    - [Binding new enum to Lua](#binding-new-enum-to-lua)
    - [Binding new string\_id or int\_id to Lua](#binding-new-string_id-or-int_id-to-lua)


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

You can adjust console log capacity by running `gdebug.set_log_capacity( num )`
(default is 100 entries), or clear it by running `gdebug.clear_lua_log()`.

## Lua hot-reload

To speed up mod development process, BN supports Lua hot-reload functionality.

There is no filesystem watcher, so hot-reload must be triggered manually via a
corresponding `Reload Lua Code` hotkey (unbound by default). The hot-reload can
also be triggered from console window by pressing the corresponding hotkey, or
by running `gdebug.reload_lua_code()` command. Running the command from regular Lua
scripts may have unintended consequences, use at your own risk!

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

As such, we only have 3 scipts to place a mod's Lua code into: `preload.lua`, `finalize.lua` and `main.lua`.
The differences between the 3 and their intended use cases will be explained below.

You can use only one script, two or all three, depending on your needs.

When executing hot-reload, the game repeats the step marked with (R).
That means if you want the code you're working on to be hot-reloadable, put it into `main.lua`.

### preload.lua
This script is supposed to register event hooks and set up definitions that will
then be referred by game JSON loading system (e.g. item use actions).
Note that you can registers them here, and define them in some later stage
(e.g. in `main.lua` to allow hot-reload to affect your hooks).

### finalize.lua
This script is supposed to allow mods to modify definitions loaded from JSON
after copy-from has been resolved, but there is no API for this yet.

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
- Loading packages (or Lua modules) is disabled.
- Your current mod id is stored in `game.current_mod` variable
- Your mod's runtime state should live in `game.mod_runtime[ game.current_mod ]` table.
  You can also interface with other mods if you know their id, by accessing their runtime state
  in a similar way with `game.mod_runtime[ that_other_mod_id ]`
- Changes to global state are not available between scripts.
  This is to prevent accidental collisions between function names and variable names.
  You still can define global variables and functions, but they will be visible to your mod only.

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
game.hooks.<hook_id>        Hooks exposed to Lua scripts, will be called on corresponding events
game.iuse.<iuse_id>         Item use functions that will be recognized by the item factory and called on item use

### Game Bindings
The game exposes various functions, constants and types to Lua.
Functions and constants are organized into "libraries" for organizational purposes.
Types are available globally, and may have member functions and fields.

To see the full list of funcitons, constants and types, run the game with `--lua-doc` command line argument.
This will generate documentation file `lua_doc.md` that will be placed in your `config` folder.

#### Global overrides
Some functions have been globally overriden to improve integration with the game.

Function                | Description
------------------------|-------------
print                   | Print as `INFO LUA` to debug.log (overrides default Lua print)

#### Hooks
To see the list of hooks, check `hooks_doc` section of the autogenerated documentation file.
There, you will see the list of hook ids as well as function signatures that they expect.
You can register new hooks by appending to the hooks table like so:
```lua
-- In preload.lua
local mod = game.mod_runtime[ game.current_mod ]
game.hooks.on_game_save[ #game.hooks.on_game_save + 1 ] = function( ... )
  -- This is essentially a forward declaration.
  -- We declare that the hook exists, it should be called on game_save event,
  -- but we will forward all possible arguments (even if there is none) to,
  -- and return value from, the function that we'll declare later on.
  return mod.my_awesome_hook( ... )
end

-- In main.lua
local mod = game.mod_runtime[ game.current_mod ]
mod.my_awesome_hook = function()
  -- Do actual work here
end
```

#### Item use function
Item use functions use unique id to register themselves in item factory.
On item activation, they receive multiple arguments that will be described in the example below.

```lua
-- In preload.lua
local mod = game.mod_runtime[ game.current_mod ]
game.iuse_functions[ "SMART_HOUSE_REMOTE" ] = function(...)
  -- This is just a forward declaration,
  -- but it will allow us to use SMART_HOUSE_REMOTE iuse in JSONs.
  return mod.my_awesome_iuse_function(...)
end

-- In main.lua
local mod = game.mod_runtime[ game.current_mod ]
mod.my_awesome_iuse_function = function( who, item, pos )
  -- Do actual activation effect here.
  -- `who` is the character that activated the item
  -- `item` is the item itself
  -- `pos` is the position of the item (equal to character pos if character has it on them)
end
```

#### Translation functions
To make the mod translatable to other languages,
get your text via functions bound in `locale` library.
See [TRANSLATING.md](TRANSLATING.md) for detailed explanation of their C++ counterparts.

Usage examples are shown below:

```lua
-- Simple string.
--
-- The "Experimental Lab" text will be extracted from this code by a script,
-- and will be available for translators.
-- When your Lua script runs, this function will search for translation of
-- "Experimental Lab" string and return either translated string, 
-- or the original string if there was no translation found.
local location_name_translated = locale.gettext( "Experimental Lab" )

-- ERROR: you must call `gettext` with a string literal.
-- Calling it like this will make it so "Experimental Lab" is NOT extracted,
-- and translators won't see it when they translate the text.
local location_name_original = "Experimental Lab"
local location_name_translated = locale.gettext( location_name_original )

-- ERROR: don't alias the function under different name.
-- Calling it like this will make it so "Experimental Lab" is NOT extracted,
-- and translators won't see it when they translate the text.
local gettext_alt = locale.gettext
local location_name_translated = gettext_alt( "Experimental Lab" )

-- This, however, is fine.
local gettext = locale.gettext
local location_name_translated = gettext( "Experimental Lab" )

-- String with possible plural form.
-- Many languages have more than 2 plural forms with complex rules related to which one to use.
local item_display_name = locale.vgettext( "X-37 Prototype", "X-37 Prototypes", num_of_prototypes )

-- String with context
local text_1 = locale.pgettext("the one made of metal", "Spring")
local text_2 = locale.pgettext("the one that makes water", "Spring")
local text_3 = locale.pgettext("time of the year", "Spring")

-- String with both context and plural forms.
local item_display_name = locale.vpgettext("the one made of metal", "Spring", "Springs", num_of_springs)

--[[
  When some text is tricky and requires explanation,
  it's common to place a special comment denoted with `~` to help translators.
  The comment MUST BE right above the function call.
]]

--~ This comment is good and will be visible for translators.
local ok = locale.gettext("Confusing text that needs explanation.")

--~ ERROR: This comment is too far from gettext call and won't be extracted!
local not_ok = locale.
                gettext("Confusing text that needs explanation.")

local not_ok = locale.gettext(
                  --~ ERROR: This comment is in wrong place and won't be extracted!
                  "Confusing text that needs explanation."
                )

--[[~
  ERROR: Multiline Lua comments can't be used as translator comments!
  This comment won't be extracted!
]] 
local ok = locale.gettext("Confusing text that needs explanation.")

--~ If you need a multiline translator comment,
--~ just use 2 or more single-line comments.
--~ They'll be concatenated and shown as a single multi-line comment.
local ok = locale.gettext("Confusing text that needs explanation.")

```

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
* `catalua_bindings*` - Game Lua bindings live here.
* `catalua_console.h`(`.cpp`) - Ingame Lua console.
* `catalua_impl.h`(`.cpp`) - Implementation details for `catalua.h`(`.cpp`).
* `catalua_iuse_actor.h`(`.cpp`) - Lua-driven `iuse_actor`.
* `catalua_log.h`(`.cpp`) - In-memory logging for the console.
* `catalua_luna.h` - Usertype registration interface with automatic doc generation, aka `luna`.
* `catalua_luna_doc.h` - List of types registration through `luna` or exposed to its doc generator.
* `catalua_readonly.h`(`.cpp`) - Functions for marking Lua tables as read-only.
* `catalua_serde.h`(`.cpp`) - Lua table to/from JSON (de-)serialization.
* `catalua_type_operators.h` - Macro that helps with implementing bindings for string_ids


### Adding new type to the doc generator without binding internals
If a C++ type has not been registered in the doc generator,
it will show up as `<cppval: **gibberish** >`.
To mitigate this problem, you can add `LUNA_VAL( your_type, "YourType" )` in `catalua_luna_doc.h`,
and the generator will use `YourType` string for argument type.


### Binding new type to Lua
First, we need to register the new type with the bindings system.
It needs to be done for many reasons, including so that the doc generator understands it,
and the runtime can deserialize from JSON any Lua table that contains that type.
If you don't you'll get a compile error saying
`Type must implement luna_traits<T>`.

1. In `catala_luna_doc.h`, add declaration for your type.
   For example, if we're binding an imaginary `horde` type (which is a `struct`),
   it will be a single line near the top of the file:
   ```c++
   struct horde;
   ```
   Complex templated types may need to actually pull in the relevant header,
   but please avoid it as it heavily impacts compilation times.

2. In the same file, register your type with the doc generator.
   Continuing with the `horde` example, it's done like this:
   ```c++
   LUNA_VAL( horde, "Horde" );
   ```
   While C++ types use all kinds of style for their names,
   on Lua side they all should be in `CamelCase`.

Now we can actually get to the details.
The bindings are implemented in `catalua_bindings*.cpp` files.
They are spread out into multiple `.cpp` files to speed up compilation
and make it easy to navigate, so you can put yours into any existing
`catalua_bindings*.cpp` file or make your own similar file.
They are also spread out into functions, for the same reasons.
Let's register our `horde` type, and put it in a new file and a new function:
1. Add a new function declaration in `catalua_bindings.h`:
   ```c++
   void reg_horde( sol::state &lua );
   ```
2. Call the function in `reg_all_bindings` in `catalua_bindings.cpp`:
   ```c++
   reg_horde( lua );
   ```
3. Make a new file, `catalua_bindings_horde.cpp`, with the following contents:
    ```c++
    #ifdef LUA
    #include "catalua_bindings.h"

    #include "horde.h" // Replace with the header where your type is defined

    void cata::detail::reg_horde( sol::state &lua )
    {
        sol::usertype<horde> ut =
            luna::new_usertype<horde>(
                lua,
                luna::no_bases,
                luna::constructors <
                    // Define your actual constructors here
                    horde(),
                    horde( const point & ),
                    horde( int, int )
                    > ()
                );
        
        // Register all needed members
        luna::set( ut, "pos", &horde::pos );
        luna::set( ut, "size", &horde::size );

        // Register all needed methods
        luna::set_fx( ut, "move_to", &horde::move_to );
        luna::set_fx( ut, "update", &horde::update );
        luna::set_fx( ut, "get_printable_name", &horde::get_printable_name );

        // Add (de-)serialization functions so we can carry
        // our horde over the save/load boundary
        reg_serde_functions( ut );

        // Add more stuff like arithmetic operators, to_string operator, etc.
    }
    ```
1. That's it. Your type is now visible in Lua under name `Horde`,
   and you can use the binded methods and members.


### Binding new enum to Lua
Binding enums is similar to binding types.
Let's bind an imaginary `horde_type` enum here:
1. If enum does not have an explicitly defined container
   (the `: type` part after `enum name` in the header where it's defined),
   you'll have to specify the container first, for example:
   ```diff
     // hordes.h
   - enum class horde_type {
   + enum class horde_type : int {
       animals,
       robots,
       zombies
     }
   ```
2. Add the declaration to `catalua_luna_doc.h`
   ```c++
   struct horde_type : int;
   ```
3. Register it in `catalua_luna_doc.h` with
   ```c++
   LUNA_ENUM( horde_type, "HordeType" )
   ```
4. Ensure the enum implements the automatic conversion
   to/from `std::string`, see `enum_conversions.h` for details.
   Some enums will already have it, but most won't.
   Usually it's just a matter of specializing `enum_traits<T>` for your enum `T`
   in the header, then defining `io::enum_to_string<T>` in the `.cpp`
   file with enum -> string conversion.
5. Bind enum fields in `reg_enums` function in `catalua_bindings.cpp`:
   ```c++
   reg_enum<horde_type>( lua );
   ```
   This uses the automatic convertion from step 4,
   so we have equal names between JSON and Lua.



### Binding new string_id<T> or int_id<T> to Lua
Binding these can be done separately from binding `T` itself.
1. Register your type `T` with the doc generator if you haven't already
   (see [relevant docs](#adding-new-type-to-the-doc-generator-without-binding-internals)).
2. Replace `LUNA_VAL` from step 1 with `LUNA_ID`.
3. Ensure your type `T` implements operators `<` and `==`.
   It's usually easy implement them manually, and can be done semi-automatically with macro
   `LUA_TYPE_OPS` found in `catalua_type_operators.h`.
4. In `catalua_bindings_ids.cpp`, add the header where your type T is defined:
   ```c++
   #include "your_type_definition.h"
   ```
5. In `reg_game_ids` function, register it like so:
   ```c++
   reg_id<T, true>( lua );
   ```
  That `true` can be replaced with `false` if you only want to bind
  `string_id<T>` and don't care about (or can't implement) `int_id<T>`.

  You may get linker errors at this stage, e.g. about `is_valid()` or
  `NULL_ID()` methods, which are for various reasons not implemented
  forall string or int ids. In this case, you'll have to define these manually,
  see relevant docs on `string_id` and `int_id` for more info.

And that's it. Now, your type `T` will show up in Lua with `Raw` postfix, 
`string_id<T>` will have `Id` postfix, and `int_id<T>` will have `IntId` postfix.
As example, for `LUNA_ID( horde, "Horde" )`, we'll get:
* `horde` -> `HordeRaw`
* `string_id<horde>` -> `HordeId`
* `int_id<horde>` -> `HordeIntId`
All type conversions between the 3 are implemented automatically by the system.
Actual fields and methods of `T` can be binded to Lua same way as usual.
