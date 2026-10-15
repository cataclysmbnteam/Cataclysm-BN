---
title: Nix
---

This repo ships a flake (`flake.nix`) that builds the game through the same CMake setup
described in [CMake](./cmake.md). Nix provides the dependencies and the install prefix;
nothing about the build itself is reimplemented.

Requires Nix with flakes enabled. Everything else, toolchain (clang 22), SDL3 stack, and
shadercross, comes from nixpkgs, so there is no system setup beyond Nix itself.

```sh
nix run .#tiles      # SDL3 + sound game (default)
nix build .#curses   # ncurses text build
nix build .#cata-test
nix flake check      # calendar smoke test + --check-mods
nix develop          # shell with cmake, ninja, git, nixfmt
```

The game revision is the checkout itself (`src = self`), so the flake tracks whatever the
working tree has. The binary banner combines the current stable version with the flake
revision; flake sources carry no `.git`, so `nix/package.nix` seeds `src/version.h` and
`VERSION.txt` from it at configure time. `flake.nix` holds the exact form.

`gfx/` ships pre-composed, so the game package already bundles every tileset.

## Packages

- `tiles`: SDL3 + sound game, the default (`nix run .#tiles` plays it).
- `curses`: text build, no sound.
- `cata-test`: Catch2 test binary.

`nix/package.nix` passes `tiles`, `sound`, and `useXdgDir` straight through to the
matching CMake options; `buildTests` selects the test binary. The toolchain is LLVM 22,
since the docs require clang >= 22.

## Checks

`nix flake check` runs the `[calendar]` smoke test through the Catch2 binary and
`--check-mods` through the game binary, the same content gate as
`./build-scripts/lint-json.sh`. The test binary resolves its data relative to the working
directory, so the check links `./data`, `./gfx`, and `./lang/mo` out of the installed
package. Headless runs set `CATA_TEST_COMPUTE_ACCELERATION=cpu` because the sandbox has
no GPU, not even software Vulkan.

## Requirement differences from other guides

- System SDL3 from nixpkgs. The vendored `BUILD_SDL3` fetch needs network and cannot
  work in the sandbox, so it stays off. The curses build still needs system SDL3 for
  platform services.
- `shadercross` from nixpkgs (`BUILD_SHADERCROSS=OFF`) for the same reason.
- No ccache: nothing persists between sandbox builds.
- Default linker instead of mold.
- `libbacktrace` from nixpkgs with `LIBBACKTRACE=ON`, same as CI, so crash traces keep
  line info.
