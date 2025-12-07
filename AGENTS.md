# Cataclysm: Bright Nights - Agent Guidelines

Repository for post-apocalyptic sci-fi survival roguelike.

- **Tech Stack:** C++23, CMake, Catch2
- **Platforms:** Linux, Windows, macOS, Android

## Directory Structure

```
src/              Core game logic (C++ source)
tests/            Unit and integration tests
data/json/        Game content (items, monsters, etc.)
gfx/              Tilesets and graphics
lang/             Localization files
docs/             Project documentation
```

## Build & Test

```sh
# Configure and build
cmake --preset linux-full
cmake --build --preset linux-full --target cataclysm-bn-tiles cata_test-tiles

# Run tests
./out/build/linux-full/tests/cata_test-tiles "test filter"
```

See [build docs](./docs/en/dev/guides/building/cmake.md) for details.

## Development Workflow

1. **Always** get issue details via GitHub MCP tools
2. Create branches per issue from `main`: `git switch --create <type>/<issue-number>/<description>`
   - Types: conventional commits (feat, fix, docs, style, refactor, test, chore) depending on issue type
3. Make changes following [style guide](./docs/en/dev/explanation/code_style.md)
   - Prefer modern C++ (auto, `trailing return`, `<ranges>`, `std::optional`, smart pointers, etc)
4. Add/update tests in `tests/` (use Catch2: `TEST_CASE`, `REQUIRE`, `CHECK`)
5. Build and test before committing
6. Commit with [conventional commits](./docs/en/contribute/changelog_guidelines.md)
   - messages must be terse and straight to the point
   - DO NOT add message body or footer unless absolutely necessary
   - use atomic commits
7. Submit PR using [PR template](./.github/pull_request_template.md)
   - **Be terse. Never waste reviewer time. No fluff or meta-commentary.**

## Guidelines

- Legacy codebase
  - Search for patterns, read tests, consult docs before making assumptions.
  - Boyscout rule: refactor parts you touch.

## Common Tasks

**JSON content:** Edit `data/json/`, validate with `build-scripts/lint-json.sh`
**Game logic:** Modify `src/*.cpp`, run affected tests
**Localization:** Use `_("text")` for translatable strings

## Code Review (GitHub Copilot)

- Comment only on objective, obvious mistakes
- All comments must be direct, actionable, PR-specific
- No summaries, assessments, or meta-commentary

### Using `suggestion` Blocks

- Use **ONLY** when change is completely self-contained (no new variables/functions, single file):
- **DON'T** use for multi-file changes or changes needing external context. Use regular code blocks instead.

## Security

- Avoid unbounded loops/recursion
- Validate JSON input, check array bounds
- Use smart pointers, be cautious with save file deserialization

## Resources

- [Dev docs](./docs/en/dev/)
