#!/bin/bash

# Build script intended for use in Github workflow

set -exo pipefail

num_jobs=3

function run_tests
{
    # The grep suppresses lines that begin with "0.0## s:", which are timing lines for tests with a very short duration.
    $WINE "$@" -d yes --use-colour yes --rng-seed time --error-format=github-action | grep -Ev "^0\.0[0-9]{2} s:"
}

# We might need binaries installed via pip, so ensure that our personal bin dir is on the PATH
export PATH=$HOME/.local/bin:$PATH

if [ -n "$DEPLOY" ]
then
    : # No-op, for now
elif [ -n "$TEST_STAGE" ]
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
    bin_path="./"
    if [ "$RELEASE" = "1" ]
    then
        build_type=MinSizeRel
        bin_path="build/tests/"
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
        -DLIBBACKTRACE=${LIBBACKTRACE:-0} \
        ..

    # Regular build
    make -j$num_jobs translations_compile
    make -j$num_jobs
    cd ..
    # Run regular tests
    [ -f "${bin_path}cata_test" ] && run_tests "${bin_path}cata_test"
    [ -f "${bin_path}cata_test-tiles" ] && run_tests "${bin_path}cata_test-tiles"
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
    make -j "$num_jobs" RELEASE=1 CCACHE=1 CROSS="$CROSS_COMPILATION" LANGUAGES="all" LINTJSON=0 LIBBACKTRACE="$LIBBACKTRACE"

    export UBSAN_OPTIONS=print_stacktrace=1
    if [ "$OS" == "macos-12" ]
    then
        run_tests ./tests/cata_test
    else
        run_tests ./tests/cata_test &
        pids[0]=$!
        if [ -n "$MODS" ]
        then
            run_tests ./tests/cata_test --user-dir=modded $MODS 2>&1 | sed 's/^/MOD> /' &
            pids[1]=$!
        fi
        for pid in ${pids[@]}; do
            wait $pid
        done
    fi

    if [ -n "$TEST_STAGE" ]
    then
        # Run the tests one more time, without actually running any tests, just to verify that all
        # the mod data can be successfully loaded

        # Use a blacklist of mods that currently fail to load cleanly.  Hopefully this list will
        # shrink over time.
        blacklist=build-scripts/mod_test_blacklist
        if [ "$LUA" == "1" ]
        then
            do_lua="1"
        else
            do_lua="0"
        fi
        mods="$(./build-scripts/get_all_mods.py $blacklist $do_lua)"
        run_tests ./tests/cata_test --user-dir=all_modded --mods="$mods" '~*'
    fi
fi
ccache --show-stats
# Shrink the ccache back down to 2GB in preperation for pushing to shared storage.
ccache -M 2G
ccache -c

# vim:tw=0
