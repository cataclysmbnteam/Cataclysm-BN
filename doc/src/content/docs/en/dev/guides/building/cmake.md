---
title: CMake
---

:::caution

CMake build is work-in-progress.

:::

For official way to build CataclysmBN see [Compiler Support](../../reference/compiler_support.md).

## Prerequisites

You'll need to have these libraries and their development headers installed in order to build
CataclysmBN:

- General
  - `cmake` >= 3.0.0
  - `gcc-libs`
  - `glibc`
  - `zlib`
  - `bzip2`
  - `sqlite3`
- Curses
  - `ncurses`
- Tiles
  - `SDL` >= 2.0.0
  - `SDL_image` >= 2.0.0 (with PNG and JPEG support)
  - `SDL_mixer` >= 2.0.0 (with Ogg Vorbis support)
  - `SDL_ttf` >= 2.0.0
  - `freetype`
- Sound
  - `vorbis`
  - `libbz2`
  - `libz`

In order to compile localization files, you'll also need `gettext` package.

## Build Environment

You can obtain the source code tarball for the latest version from
[git](https://github.com/cataclysmbnteam/Cataclysm-BN).

### UNIX Environment

Obtain packages specified above with your system package manager.

- For Ubuntu-based distros (24.04 onwards):

```sh
sudo apt install git cmake ninja-build mold clang ccache \
libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev \
libfreetype-dev bzip2 zlib1g-dev libvorbis-dev libncurses-dev \
gettext libflac++-dev libsqlite3-dev zlib1g-dev
```

- For Fedora-based distros:

```sh
sudo dnf install git cmake ninja-build mold clang ccache \
SDL2-devel SDL2_image-devel SDL2_ttf-devel SDL2_mixer-devel \
freetype glibc bzip2 zlib-ng libvorbis ncurses gettext flac-devel \
sqlite-devel zlib-devel
```

### Windows Subsystem for Linux (WSL)

Follow the same instructions for `UNIX environment`; it just works (TM)

If you plan on using `tiles`, make sure you have the latest [WSL 2 that supports GUI](https://learn.microsoft.com/en-us/windows/wsl/tutorials/gui-apps).

### Windows Environment (MSYS2)

1. Follow steps from here: https://msys2.github.io/
2. Install CataclysmBN build deps:

```
pacman -S mingw-w64-x86_64-toolchain msys/git \
   	  mingw-w64-x86_64-cmake \
   	  mingw-w64-x86_64-SDL2_{image,mixer,ttf} \
   	  ncurses-devel \
      gettext \
      base-devel
```

This should get your environment set up to build console and tiles version of windows.

:::note

If you're trying to test with Jetbrains CLion, point to the cmake version in the `msys32/mingw32`
path instead of using the built in. This will let cmake detect the installed packages.

:::

### CMake Build

CMake has separate configuration and build steps. Configuration is done using CMake itself, and the
actual build is done using either `make` (for Makefiles generator) or build-system agnostic
`cmake --build .` .

There are two ways to build CataclysmBN with CMake: inside the source tree or outside of it.
Out-of-source builds have the advantage that you can have multiple builds with different options
from one source directory.

:::danger

Inside the source tree build is **NOT** supported.

:::

To build CataclysmBN out of source:

```sh
mkdir build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The above example creates a build directory inside the source directory, but that's not required -
you can just as easily create it in a completely different location.

To install CataclysmBN after building (as root using su or sudo if necessary):

```sh
cmake --install build
```

To change build options, you can either pass the options on the command line:

```sh
cmake .. -DOPTION_NAME=option_value
```

Or use either the `ccmake` or `cmake-gui` front-ends, which display all options and their cached
values on a console and graphical UI, respectively.

```sh
ccmake ..
cmake-gui ..
```

A CMake build with almost all options with build optimizations (ccache, ninja, mold) + tracy
profiler may look like:

```sh
mkdir build
cmake \
  -B build \
  -G Ninja \
  -DCATA_CCACHE=ON \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_INSTALL_PREFIX=$HOME/.local/share \
  -DJSON_FORMAT=ON \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCURSES=OFF \
  -DTILES=ON \
  -DSOUND=ON \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCATA_CLANG_TIDY_PLUGIN=OFF \
  -DLUA=ON \
  -DBACKTRACE=ON \
  -DLINKER=mold \
  -DUSE_XDG_DIR=ON \
  -DUSE_HOME_DIR=OFF \
  -DUSE_PREFIX_DATA_DIR=OFF \
  -DUSE_TRACY=ON \
  -DTRACY_VERSION=master \
  -DTRACY_ON_DEMAND=ON \
  -DTRACY_ONLY_IPV4=ON
cmake --build build
```

This will place the executables into `build/src/`.

## Build for Visual Studio / MSBuild

:::caution

This guide is quite old and requires manual dependency management.

For modern alternative, see [CMake Visual Studio build with vcpkg](./vs_cmake.md)

:::

CMake can generate `.sln` and `.vcxproj` files used either by Visual Studio itself or by MSBuild
command line compiler (if you don't want a full fledged IDE) and have more "native" binaries than
what MSYS/Cygwin can provide.

At the moment only a limited combination of options is supported (tiles only, no localizations, no
backtrace).

Get the tools:

- CMake from the official site - <https://cmake.org/download/>.
- Microsoft compiler - <https://visualstudio.microsoft.com/downloads/?q=build+tools> , choose "Build
  Tools for Visual Studio 2017". When installing chose "Visual C++ Build Tools" options.
  - alternatively, you can get download and install the complete Visual Studio, but that's not
    required.

Get the required libraries:

- [SDL2](https://github.com/libsdl-org/SDL/releases/tag/release-2.28.3) (you need the "(Visual C++
  32/64-bit)" version. Same below)
- [SDL2_ttf](https://github.com/libsdl-org/SDL_ttf)
- [SDL2_image](https://github.com/libsdl-org/SDL_image)
- [SDL2_mixer](https://github.com/libsdl-org/SDL_mixer) (optional, for sound support)
- Unsupported (and unused in the following instructions) optional libs:
  - `ncurses` - ???

Unpack the archives with the libraries.

Open windows command line (or powershell), set the environment variables to point to the libs above
as follows (adjusting the paths as appropriate):

```sh
set SDL2DIR=C:\path\to\SDL2-devel-2.0.9-VC
set SDL2TTFDIR=C:\path\to\SDL2_ttf-devel-2.0.15-VC
set SDL2IMAGEDIR=C:\path\to\SDL2_image-devel-2.0.4-VC
set SDL2MIXERDIR=C:\path\to\SDL2_mixer-devel-2.0.4-VC
```

(for powershell the syntax is `$env:SDL2DIR="C:\path\to\SDL2-devel-2.0.9-VC"`).

Make a build directory and run cmake configuration step

```sh
cd <path to cbn sources>
mkdir build
cmake -B build -DTILES=ON -DLANGUAGES=none -DBACKTRACE=OFF -DSOUND=ON
```

Build!

```
cmake --build build -j 2 -- /p:Configuration=Release
```

The `-j 2` flag controls build parallelism - you can omit it if you wish. The
`/p:Configuration=Release` flag is passed directly to MSBuild and controls optimizations. If you
omit it, the `Debug` configuration would be built instead. For powershell you'll need to have an
extra `--` after the first one.

The resulting files will be put into a `Release` directory inside your source Cataclysm-BN folder.
To make them run you'd need to first move them to the source Cataclysm-BN directory itself (so that
the binary has access to the game data), and second put the required `.dll`s into the same folder -
you can find those inside the directories for dev libraries under `lib/x86/` or `lib/x64/` (you
likely need the `x86` ones even if you're on 64-bit machine).

The copying of dlls is a one-time task, but you'd need to move the binary out of `Release/` each
time it's built. To automate it a bit, you can configure cmake and set the desired binaries
destination directory with `-DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE=` option (and similar for
`CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG`).

Run the game. Should work.

## Build Options

A full list of options supported by CMake, you may either run the `ccmake` or `cmake-gui`
front-ends, or run `cmake` and open the generated CMakeCache.txt from the build directory in a text
editor.

```
cmake -DOPTION_NAME1=option_value1 [-DOPTION_NAME2=option_value2 [...]]
```

### CMake specific options

- CMAKE_BUILD_TYPE=`<build type>`

Selects a specific build configuration when compiling. `release` produces the default, optimized
(-Os) build for regular use. `debug` produces a slower and larger unoptimized (-O0) build with full
debug symbols, which is often needed for obtaining detailed backtraces when reporting bugs.

**NOTE**: By default, CMake will produce `debug` builds unless a different configuration option is
passed in the command line.

- CMAKE_INSTALL_PREFIX=`<full path>`

Installation prefix for binaries, resources, and documentation files.

### CataclysmBN specific options

- CURSES=`<boolean>`

Build curses version.

- TILES=`<boolean>`

Build graphical tileset version.

- SOUND=`<boolean>`

Support for in-game sounds & music.

- USE_HOME_DIR=`<boolean>`

Use user's home directory for save files.

- LANGUAGES=`<str>`

Compile localization files for specified languages. Example:

```
-DLANGUAGES="cs;de;el;es_AR;es_ES"
```

Note that language files are only compiled automatically when building the `RELEASE` build type. For
other build types, you need to add the `translations_compile` target to the `make` command, for
example `make all translations_compile`.

- DYNAMIC_LINKING=`<boolean>`

Use dynamic linking. Or use static to remove MinGW dependency instead.

- CUSTOM LINKER=`<str>`

Choose custom linkers such as [gold], [lld] or [mold].

- Choose ld if you don't use libbacktrace.
- Choose mold if use libbacktrace. It's the fastest linker, outperforming gold by 24x.

[gold]: https://en.wikipedia.org/wiki/Gold_(linker)
[lld]: https://lld.llvm.org
[mold]: https://github.com/rui314/mold

- BACKTRACE=`<boolean>`

On crash, print a backtrace to the console. Defaults to `ON` for debug builds.

- LIBBACKTRACE=`<boolean>`

Print backtrace with [libbacktrace]. This allows lld and mold to print backtrace, and is generally
much faster.

[libbacktrace]: https://github.com/ianlancetaylor/libbacktrace

- USE_TRACY=`<boolean>`

Use tracy profiler. See [Profiling with tracy](../tracy.md) for more information.

- GIT_BINARY=`<str>`

Override default Git binary name or path.

- USE_PREFIX_DATA_DIR=`<boolean>`

Use UNIX system directories for game data in release build.

- USE_XDG_DIR=`<boolean>`

Use XDG directories for save and config files.

- TESTS=`<boolean>`

Whether to build tests.

So a CMake command for building Cataclysm-BN in release mode with tiles and sound support will look
as follows, provided it is run in build directory located in the project.

```sh
cmake ../ -DCMAKE_BUILD_TYPE=Release -DTILES=ON -DSOUND=ON
```
