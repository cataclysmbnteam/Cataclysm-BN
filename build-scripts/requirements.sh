#!/bin/bash

set -e
set -x

if [[ "$LIBBACKTRACE" == "1" ]]; then
    git clone https://github.com/ianlancetaylor/libbacktrace.git
    (
        cd libbacktrace
        git checkout 4d2dd0b172f2c9192f83ba93425f868f2a13c553
        ./configure
        make -j$(nproc)
        sudo make install
    )
fi

if [ -n "$CATA_CLANG_TIDY" ]; then
  pip install --user wheel --upgrade
  pip install --user 'lit==0.11.1' 'click==7.1.2'
fi

if [ -n "$LANGUAGES" ]; then
  pip install --user polib luaparser
fi

# Influenced by https://github.com/zer0main/battleship/blob/master/build/windows/requirements.sh
if [ -n "${MXE_TARGET}" ]; then
  sudo dpkg --add-architecture i386
  sudo apt update
  sudo apt-get --yes install wine wine32

  set +e
  retry=0
  until [[ "$retry" -ge 5 ]]; do
    curl -L -o mxe-x86_64.tar.xz https://github.com/BrettDong/MXE-GCC/releases/download/mxe-sdl-2-0-20/mxe-x86_64.tar.xz && curl -L -o mxe-x86_64.tar.xz.sha256 https://github.com/BrettDong/MXE-GCC/releases/download/mxe-sdl-2-0-20/mxe-x86_64.tar.xz.sha256 && shasum -a 256 -c ./mxe-x86_64.tar.xz.sha256 && break
    retry=$((retry+1))
    rm -f mxe-i686.tar.xz mxe-i686.tar.xz.sha256
    sleep 10
  done
  if [[ "$retry" -ge 5 ]]; then
    echo "Error downloading or checksum failed for MXE i686"
    exit 1
  fi
  set -e
  sudo tar xJf mxe-i686.tar.xz -C /opt

  export MXE_DIR=/opt/mxe
  export CROSS_COMPILATION="${MXE_DIR}/usr/bin/${MXE_TARGET}-"
  # Need to overwrite CXX to make the Makefile $CROSS logic work right.
  export CXX="$COMPILER"
  export CCACHE=1

  set +e
  retry=0
  until [[ "$retry" -ge 5 ]]; do
    curl -L -o SDL2-devel-2.26.2-mingw.tar.gz https://github.com/libsdl-org/SDL/releases/download/release-2.26.2/SDL2-devel-2.26.2-mingw.tar.gz && shasum -a 256 -c ./build-scripts/SDL2-devel-2.26.2-mingw.tar.gz.sha256 && break
    retry=$((retry+1))
    rm -f SDL2-devel-2.26.2-mingw.tar.gz
    sleep 10
  done
  if [[ "$retry" -ge 5 ]]; then
    echo "Error downloading or checksum failed for SDL2-devel-2.26.2-mingw.tar.gz"
    exit 1
  fi
  set -e
  sudo tar -xzf SDL2-devel-2.26.2-mingw.tar.gz -C ${MXE_DIR}/usr/${MXE_TARGET} --strip-components=2 SDL2-2.26.2/x86_64-w64-mingw32

  if grep -q "x86_64" <<< "$MXE_TARGET"; then
    curl -L -o libbacktrace-x86_64-w64-mingw32.tar.gz https://github.com/Qrox/libbacktrace/releases/download/2020-01-03/libbacktrace-x86_64-w64-mingw32.tar.gz
    if ! shasum -a 256 -c ./build-scripts/libbacktrace-x86_64-w64-mingw32-sha256; then
      echo "Checksum failed for libbacktrace-x86_64-w64-mingw32.tar.gz"
      exit 1
    fi
    sudo tar -xzf libbacktrace-x86_64-w64-mingw32.tar.gz --exclude=LICENSE -C ${MXE_DIR}/usr/${MXE_TARGET}
  else
    curl -L -o libbacktrace-i686-w64-mingw32.tar.gz https://github.com/Qrox/libbacktrace/releases/download/2020-01-03/libbacktrace-i686-w64-mingw32.tar.gz
    if ! shasum -a 256 -c ./build-scripts/libbacktrace-i686-w64-mingw32-sha256; then
      echo "Checksum failed for libbacktrace-i686-w64-mingw32.tar.gz"
      exit 1
    fi
    sudo tar -xzf libbacktrace-i686-w64-mingw32.tar.gz --exclude=LICENSE -C ${MXE_DIR}/usr/${MXE_TARGET}
  fi
fi

set +x
