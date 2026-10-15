# Build Cataclysm: Bright Nights with the repo's CMake setup (see
# docs/en/dev/guides/building/cmake.md). CMake stays the build system;
# Nix just hands it dependencies and the install prefix.
#
# Notes for maintainers:
#   * gfx/ ships pre-composed, so there is no tileset pipeline here.
#     The game package already bundles every tileset, like a release.
#   * CMake generates src/version.h and VERSION.txt from git at configure
#     time. Nix sources carry no .git, so preConfigure seeds both from
#     the `version` and `rev` arguments instead.
#   * Docs require clang >= 22, so callers pass llvmPackages_22.stdenv.
#     Shadercross comes from nixpkgs (BUILD_SHADERCROSS=OFF) instead of
#     building from source, which would need network at build time.
{
  lib,
  stdenv,
  cmake,
  ninja,
  pkg-config,
  gettext,
  sqlite,
  ncurses,
  zlib,
  bzip2,
  libbacktrace,
  libvorbis,
  freetype,
  sdl3,
  sdl3-image,
  sdl3-ttf,
  sdl3-mixer,
  sdl3-shadercross,
  # SDL3 graphical build; ncurses text build when false
  tiles ? true,
  # sound via SDL3_mixer; requires tiles
  sound ? true,
  # user files in $XDG_DATA_HOME instead of ~/.cataclysm-bn
  useXdgDir ? false,
  # build the cata_test-tiles Catch2 binary instead of just the game
  buildTests ? false,
  version,
  # Full git revision of `src`, used for the version banner and the
  # commit link in VERSION.txt.
  rev ? "unknown",
  src,
}:

assert lib.assertMsg (tiles || !sound) "cataclysm-bn: sound requires tiles";
assert lib.assertMsg (
  !buildTests || (tiles && sound)
) "cataclysm-bn: tests need the tiles+sound variant (game-state init)";

let
  inherit (lib) optionals;
  gameDeps = [
    sqlite
    zlib
    bzip2
    freetype
    # Crash traces with line info, same as CI. Without the system
    # copy CMake fetches libbacktrace from GitHub, which cannot work
    # in the sandbox.
    libbacktrace
  ]
  ++ optionals tiles [
    sdl3
    sdl3-image
    sdl3-ttf
  ]
  ++ optionals sound [
    sdl3-mixer
    libvorbis
  ]
  # CATA_SDL forces on for every variant, even curses, for GPU platform
  # services; CMake fetches SDL3 from GitHub when the system copy is
  # missing, which cannot work in the sandbox. The tiles list already
  # carries it, so curses needs it here.
  ++ optionals (!tiles) [
    ncurses
    sdl3
  ];

  targetName =
    if buildTests then
      "cata_test-tiles"
    else if tiles then
      "cataclysm-bn-tiles"
    else
      "cataclysm-bn";
in
stdenv.mkDerivation (
  {
    pname = "cataclysm-bn" + lib.optionalString buildTests "-test";
    inherit version src;

    nativeBuildInputs = [
      cmake
      ninja
      pkg-config
      gettext
      # Provides `shadercross` on PATH so BUILD_SHADERCROSS can stay OFF.
      sdl3-shadercross
    ];

    buildInputs = gameDeps;

    strictDeps = true;

    # RelWithDebInfo takes the release path in CMakeLists.txt (PREFIX data
    # layout under share/cataclysm-bn); Debug would drop into dev mode
    # with binaries in the source dir and no install rules.
    cmakeBuildType = "RelWithDebInfo";

    # The test binary is the deliverable here, not a build-time suite run:
    # doCheck flips the cmake hook's -DBUILD_TESTING to ON (it forces OFF
    # otherwise, which silently drops the test target), while the no-op
    # checkPhase keeps the suite from running in the sandbox. The calendar
    # suite runs later as a flake check instead.
    doCheck = buildTests;
    checkPhase = ":";

    cmakeFlags = [
      "-DTILES=${if tiles then "ON" else "OFF"}"
      "-DCURSES=${if tiles then "OFF" else "ON"}"
      "-DSOUND=${if sound then "ON" else "OFF"}"
      "-DTESTS=${if buildTests then "ON" else "OFF"}"
      "-DUSE_PREFIX_DATA_DIR=ON"
      "-DBUILD_SHADERCROSS=OFF"
      "-DCATA_CCACHE=OFF"
      "-DLIBBACKTRACE=ON"
      "-DJSON_FORMAT=OFF"
    ]
    ++ optionals useXdgDir [ "-DUSE_XDG_DIR=ON" ];

    # Seed the two files CMake only generates when git_describe finds a
    # repo. Nix sources carry no .git, so both are missing otherwise and
    # the install rule aborts on VERSION.txt.
    preConfigure = ''
      cat > src/version.h <<EOF
      // NOLINT(cata-header-guard)
      #define VERSION "${version}"
      #define BUILD_TIMESTAMP "nix-build"
      EOF
      printf 'build type: RelWithDebInfo\nbuild number: nix\ncommit sha: %s\ncommit url: https://github.com/cataclysmbn/Cataclysm-BN/commit/%s\n' '${rev}' '${rev}' > VERSION.txt
    '';

    # The game installs no desktop entry itself (those CMake messages are
    # leftovers nothing sets), so ship the repo's files with Exec pointing
    # at the store binary. Tiles only: the shipped entry launches the
    # graphical build.
    postInstall = lib.optionalString (tiles && !buildTests) ''
      install -Dm644 ${src}/data/XDG/org.cataclysmbn.CataclysmBN.svg $out/share/icons/hicolor/scalable/apps/org.cataclysmbn.CataclysmBN.svg
      install -Dm644 ${src}/data/XDG/org.cataclysmbn.CataclysmBN.metainfo.xml $out/share/metainfo/org.cataclysmbn.CataclysmBN.metainfo.xml
      install -Dm644 ${src}/data/XDG/org.cataclysmbn.CataclysmBN.desktop $out/share/applications/org.cataclysmbn.CataclysmBN.desktop
      substituteInPlace $out/share/applications/org.cataclysmbn.CataclysmBN.desktop \
        --replace-fail "Exec=cataclysm-bn-tiles" "Exec=$out/bin/cataclysm-bn-tiles"
    '';

    meta = {
      description = "Cataclysm: Bright Nights ${targetName} (${version})";
      homepage = "https://github.com/cataclysmbn/Cataclysm-BN";
      license = lib.licenses.cc-by-sa-30;
      platforms = lib.platforms.linux;
      mainProgram = targetName;
    };
  }
  // lib.optionalAttrs buildTests {
    # The test package ships only the Catch2 binary, not a second copy of
    # all game data (tests/CMakeLists.txt has no install rule for it, so
    # fish it out of the build tree). The calendar check reads data from
    # the tiles package instead.
    installPhase = ''
      runHook preInstall
      testBin=$(find "$NIX_BUILD_TOP" -name cata_test-tiles -type f | head -n 1)
      test -n "$testBin" || { echo "cata_test-tiles not found in build tree" >&2; exit 1; }
      install -Dm755 "$testBin" $out/bin/cata_test-tiles
      runHook postInstall
    '';
  }
)
