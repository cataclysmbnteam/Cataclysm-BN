#!/bin/bash

if [ -n "${CODE_COVERAGE}" ]; then
  pip install --user pyyaml cpp-coveralls;
  export CXXFLAGS=--coverage;
  export LDFLAGS=--coverage;
fi

# Influenced by https://github.com/zer0main/battleship/blob/master/build/windows/requirements.sh
if [ -n "${MXE_TARGET}" ]; then
  echo "deb http://pkg.mxe.cc/repos/apt/debian wheezy main" \
    | sudo tee /etc/apt/sources.list.d/mxeapt.list
  sudo apt-key adv --keyserver x-hkp://keys.gnupg.net \
    --recv-keys D43A795B73B16ABE9643FE1AFD8FFF16DB45C6AB
  sudo apt-get update

  MXE2_TARGET=$(echo "$MXE_TARGET" | sed 's/_/-/g')
  export MXE_DIR=/usr/lib/mxe/usr/bin
  sudo apt-get --yes install mxe-${MXE2_TARGET}-gcc mxe-${MXE2_TARGET}-gettext mxe-${MXE2_TARGET}-glib \
    mxe-${MXE2_TARGET}-sdl2 mxe-${MXE2_TARGET}-sdl2-gfx mxe-${MXE2_TARGET}-sdl2-image mxe-${MXE2_TARGET}-sdl2-ttf \
    mxe-${MXE2_TARGET}-sdl2-mixer \
    mxe-${MXE2_TARGET}-freetype mxe-${MXE2_TARGET}-lua
  export CROSS_COMPILATION='${MXE_DIR}/${MXE_TARGET}-'
  export CCACHE=1
fi
