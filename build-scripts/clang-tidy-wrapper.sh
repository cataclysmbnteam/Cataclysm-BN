#!/bin/bash

set -eu
set -o pipefail

plugin=build/tools/clang-tidy-plugin/libCataAnalyzerPlugin.so

clang-tidy \
  --load $plugin \
  --config-file=build-scripts/.clang-tidy \
  --enable-check-profile \
  --store-check-profile=clang-tidy-trace "$@"
