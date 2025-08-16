# Copilot Instructions
## When

- Project uses C++23, CMake and catch2
- Prefer modern C++ features, e.g `auto`, structured bindings, `std::optional`, `<range>`, etc.
- `src/` contains headers and source files
- `tests/` contains test files
- When working on an issue:
  - `git switch -c <conventional-commit-type>/<issue-number>-<slugified-title>`
  - Use `git commit` with a conventional commit message
  - Create a test for changes, with comments that link to the github issue
- To build the game and tests:

```sh
cmake --build --parallel 8 --preset linux-full --target cataclysm-bn-tiles cata_test-tiles
```
