#!/bin/bash

# Shell script intended for clang-tidy check

num_jobs=${num_jobs:-3}

echo "Using bash version $BASH_VERSION with $num_jobs jobs"
set -exo pipefail


# We might need binaries installed via pip, so ensure that our personal bin dir is on the PATH
export PATH=$HOME/.local/bin:$PATH

if [ "$RELEASE" = "1" ]
then
    build_type=MinSizeRel
else
    build_type=Debug
fi

cmake_extra_opts=()

if [ "$CATA_CLANG_TIDY" = "plugin" ]
then
    cmake_extra_opts+=("-DCATA_CLANG_TIDY_PLUGIN=ON")
    # Need to specify the particular LLVM / Clang versions to use, lest it
    # use the older LLVM that comes by default on Ubuntu.
    cmake_extra_opts+=("-DLLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm")
    cmake_extra_opts+=("-DClang_DIR=/usr/lib/llvm-16/lib/cmake/clang")
fi

mkdir -p build
cd build
cmake \
    -DBACKTRACE=ON \
    ${COMPILER:+-DCMAKE_CXX_COMPILER=$COMPILER} \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_BUILD_TYPE="$build_type" \
    -DTILES=${TILES:-0} \
    -DSOUND=${SOUND:-0} \
    "${cmake_extra_opts[@]}" \
    ..

if [ "$CATA_CLANG_TIDY" = "plugin" ]
then
    make -j$num_jobs CataAnalyzerPlugin
    if ! which FileCheck
    then
        echo "Missing FileCheck"
        exit 1
    fi
    CATA_CLANG_TIDY=clang-tidy
    lit -v tools/clang-tidy-plugin/test
fi

"$CATA_CLANG_TIDY" --version

# Show compiler C++ header search path
${COMPILER:-clang++} -v -x c++ /dev/null -c
# And the same for clang-tidy
"$CATA_CLANG_TIDY" ../src/version.cpp -- -v

cd ..
ln -s build/compile_commands.json

# We want to only analyze all files that changed in the PR,
# because it takes a long time to analyze all files on GitHub Actions.
set +x

all_cpp_files="$(jq -r '.[].file' build/compile_commands.json)"
if [ "$TIDY" == "all" ]
then
    echo "Analyzing all files"
    tidyable_cpp_files=$all_cpp_files
else
    make \
        -j $num_jobs \
        ${COMPILER:+COMPILER=$COMPILER} \
        TILES=${TILES:-0} \
        SOUND=${SOUND:-0} \
        includes

    tidyable_cpp_files="$( \
        ( test -f ./files_changed && build-scripts/get_affected_files.py ./files_changed ) || \
        echo unknown )"

    if [ "tidyable_cpp_files" == "unknown" ]
    then
        echo "Unable to determine affected files"
    else
        echo "Affected files:"
        echo "$tidyable_cpp_files"
    fi
fi


# We might need to analyze only a subset of the files if they have been split
# into multiple jobs for efficiency. The paths from `compile_commands.json` can
# be absolute but the paths from `get_affected_files.py` are relative, so both
# formats are matched. Exit code 1 from grep (meaning no match) is ignored in
# case one subset contains no file to analyze.
case "$CATA_CLANG_TIDY_SUBSET" in
    ( src )
        tidyable_cpp_files=$(printf '%s\n' "$tidyable_cpp_files" | grep -E '(^|/)src/' || [[ $? == 1 ]])
        ;;
    ( other )
        tidyable_cpp_files=$(printf '%s\n' "$tidyable_cpp_files" | grep -Ev '(^|/)src/' || [[ $? == 1 ]])
        ;;
esac

printf "Subset to analyze: '%s'\n" "$CATA_CLANG_TIDY_SUBSET"
printf "Files to analyze: '%s'\n" "$tidyable_cpp_files"

function analyze_files_in_random_order
{
    echo "$1" | shuf | xargs -P "$num_jobs" -n 1 ./build-scripts/clang-tidy-wrapper.sh -quiet
}

# fail fast if no files to analyze
if [ -z "$tidyable_cpp_files" ]
then
    echo "No files to analyze"
else
  echo "Analyzing affected files"
  analyze_files_in_random_order "$tidyable_cpp_files"
fi
