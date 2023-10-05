#!/bin/bash

set -eu
set -o pipefail

plugin=build/tools/clang-tidy-plugin/libCataAnalyzerPlugin.so

LD_PRELOAD=$plugin clang-tidy \
  --load $plugin \
  --enable-check-profile \
  --store-check-profile=clang-tidy-trace "$@"
