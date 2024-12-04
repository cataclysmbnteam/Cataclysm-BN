#!/bin/bash

set -eu
set -o pipefail
set -x

BUILD_PATH=${BUILD_PATH:-"build"}

PLUGIN=$BUILD_PATH/tools/clang-tidy-plugin/libCataAnalyzerPlugin.so

clang-tidy \
  --load="$PLUGIN" \
  --enable-check-profile \
  --store-check-profile=clang-tidy-trace "$@"
