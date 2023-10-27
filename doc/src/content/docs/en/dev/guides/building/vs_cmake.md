---
title: CMake + Visual Studio + Vcpkg
---

:::caution

CMake build is work-in-progress.

:::

For official way to build CataclysmBN see [Compiler Support](../../reference/compiler_support.md).

## Prerequisites

- `cmake` >= 3.20.0
- `vcpkg` from [vcpkg.io](https://vcpkg.io/en/getting-started.html)

Note that starting from Visual Studio 2022 version 17.6, `vcpkg` is included with the distribution
and is available in the VS developer command prompt, so you don't have to install it separately.

## Configure

Configuration can be done using one of the presets in `CMakePresets.json`. They will all build the
code into the directory `out/build/<preset>/`.

### Terminal

Ensure that `cmake` can find `vcpkg`. If it can't, it will complain about missing packages. You can
do this in one of the following ways:

- For VS2022 users with preinstalled `vcpkg`, `vcpkg` should already be available if you're running
  the VS developer command prompt and not the plain terminal
- Append `-DVCPKG_ROOT=C:\dev\vcpkg` (or whatever the path is) to any cmake configure commands
- Set the environment variable `VCPKG_ROOT` to the path to the vcpkg checkout
- Add the `VCPKG_ROOT` cache variable in `CMakePresets.json` with the appropriate path (not
  recommended if you plan to work with the code later, git tracks this file)

Run the command

```sh
cmake --list-presets
```

It will show the presets available to you. The list changes based on the environment you are in. If
empty, the environment is not supported.

Run the command

```sh
cmake --preset <preset>
```

It will download all dependencies and generate build files, as long as `vcpkg` installation is
available.

If you're using VS2022, you may need to tell CMake to generate VS2022-compatible project files with
`-G "Visual Studio 17 2022"`, as by default it will try to generate for VS2019.

You can override any option by appending `-Doption=value` to this command, see
[Build options](./cmake.md/#build-options) in CMake guide.

### Visual Studio

Open the game source folder in Visual Studio.

Visual Studio should be able to recognize the folder as a CMake project, and may attempt to start
configuring it, which will most likely fail because it didn't use the proper preset.

The Standard toolbar shows the presets in the `Configuration` drop-down box. Choose the proper one
(should contain `windows` and `msvc`), then from the main menu, select `Project` ->
`Configure Cache`.

## Build

### Terminal

Run the command

- `cmake --build --preset <preset>`

### Visual Studio

From the Standard toolbar's `Build Preset` drop-down menu select the build preset. From the main
menu, select `Build` -> `Build All`.

## Translations

Translations are optional and require `msgfmt` binary from `gettext` package, but `vcpkg` should
install it automatically.

### Terminal

Run the command

- `cmake --build --preset <preset> --target translations_compile`

### Visual Studio

Visual Studio should have built the translations in the previous step. If it did not, refer to the
terminal guide.

## Install

:::caution

Install is still considered WIP and has not been tested much.

:::

### Visual Studio

From the main menu, select `Build` -> `Install CataclysmBN`

### Terminal

Run the command

- `cmake --install out/build/<preset>/ --config RelWithDebInfo`

## Run

### Visual Studio

TBD

The game executable is available at `.\out\build\<preset>\src\Debug\cataclysm-tiles.exe`.

If you run with `TESTS=ON`, the test executable is available at
`.\out\build\<preset>\tests\Debug\cata_test-tiles.exe`.

### Terminal

Run the commands

- `cd out/install/<preset>/`
- `cataclysm` or `cataclysm-tiles.exe`
