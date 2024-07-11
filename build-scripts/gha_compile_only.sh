#!/bin/bash

# Script made specifically for compiling without running tests on GitHub Actions

echo "Using bash version $BASH_VERSION"
set -exo pipefail

num_jobs=3

# We might need binaries installed via pip, so ensure that our personal bin dir is on the PATH
export PATH=$HOME/.local/bin:$PATH

if [ -n "$TEST_STAGE" ]
then
    build-scripts/lint-json.sh
    make style-all-json-parallel RELEASE=1

    tools/dialogue_validator.py data/json/npcs/* data/json/npcs/*/* data/json/npcs/*/*/*
elif [ -n "$JUST_JSON" ]
then
    echo "Early exit on just-json change"
    exit 0
fi

ccache --zero-stats
# Increase cache size because debug builds generate large object files
ccache -M 5G
ccache --show-stats

if [ "$CMAKE" = "1" ]
then
    if [ "$RELEASE" = "1" ]
    then
        build_type=MinSizeRel
    else
        build_type=Debug
    fi

    mkdir build
    cd build
    cmake \
        -DBACKTRACE=ON \
        ${COMPILER:+-DCMAKE_CXX_COMPILER=$COMPILER} \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_BUILD_TYPE="$build_type" \
        -DTILES=${TILES:-0} \
        -DSOUND=${SOUND:-0} \
        -DLUA=${LUA:-0} \
        ..

    make -j$num_jobs
else
    if [ "$OS" == "macos-12" ]
    then
        export NATIVE=osx
        # if OSX_MIN we specify here is lower than 11 then linker is going
        # to throw warnings because uncaught_exceptions, SDL and gettext libraries installed from
        # Homebrew are built with minimum target osx version 11
        export OSX_MIN=11
    else
        export BACKTRACE=1
    fi
    make -j "$num_jobs" RELEASE=1 CCACHE=1 CROSS="$CROSS_COMPILATION" LINTJSON=0 FRAMEWORK=1

    # For CI on macOS, patch the test binary so it can find SDL2 libraries.
    if [[ ! -z "$OS" && "$OS" = "macos-12" ]]
    then
        file tests/cata_test
        install_name_tool -add_rpath $HOME/Library/Frameworks tests/cata_test
    fi
fi

# vim:tw=0
