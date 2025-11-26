---
title: Formatting & Linting
---

This guide explains how to format and lint code in Cataclysm: Bright Nights.

## Quick Reference

| File Type       | Tool           | Command                                            |
| --------------- | -------------- | -------------------------------------------------- |
| C++ (`.cpp/.h`) | astyle         | `make astyle`                                      |
| JSON            | json_formatter | `cmake --build build --target style-json-parallel` |
| Markdown        | deno fmt       | `deno fmt`                                         |
| TypeScript      | deno fmt       | `deno fmt`                                         |
| Lua             | dprint         | `deno task dprint fmt`                             |

## Automated Formatting

Pull requests are automatically formatted by [autofix.ci](https://autofix.ci). If your code has
style violations, a commit will be pushed to fix them.

> [!TIP]
> To avoid merge conflicts after autofix commits, either:
>
> 1. Run `git pull` to merge the autofix commit, then continue working
> 2. Format locally before pushing, then `git push --force` if needed

## C++ Formatting

C++ files are formatted with [astyle](http://astyle.sourceforge.net/).

```sh
# Format all C++ files
make astyle

# Install astyle (Ubuntu/Debian)
sudo apt install astyle

# Install astyle (Fedora)
sudo dnf install astyle

# Install astyle (macOS)
brew install astyle
```

The style configuration is in `.astylerc` at the repository root.

## JSON Formatting

JSON files are formatted with `json_formatter`, a custom tool built from the project source.

### Using CMake (Recommended)

```sh
# Configure with JSON_FORMAT enabled
cmake -B build -DJSON_FORMAT=ON

# Format all JSON files in parallel
cmake --build build --target style-json-parallel

# Format all JSON files sequentially (slower, but useful for debugging)
cmake --build build --target style-json
```

### Using Makefile (Legacy)

```sh
make style-all-json-parallel RELEASE=1
```

> [!NOTE]
> The `data/names/` directory is excluded from formatting because name files have special formatting
> requirements.

### JSON Syntax Validation

Before formatting, you can validate JSON syntax:

```sh
build-scripts/lint-json.sh
```

This runs Python's `json.tool` on all JSON files to catch syntax errors.

## Markdown & TypeScript Formatting

Markdown and TypeScript files are formatted with [Deno](https://deno.land/).

```sh
# Install Deno
curl -fsSL https://deno.land/install.sh | sh

# Format Markdown and TypeScript
deno fmt
```

## Lua Formatting

Lua files are formatted with [dprint](https://dprint.dev/) via Deno.

```sh
# Format Lua files
deno task dprint fmt
```

## Dialogue Validation

NPC dialogue files have additional validation:

```sh
tools/dialogue_validator.py data/json/npcs/* data/json/npcs/*/* data/json/npcs/*/*/*
```

## Pre-commit Workflow

Before committing, run these checks:

```sh
# 1. Format C++
make astyle

# 2. Format JSON (requires CMake build with JSON_FORMAT=ON)
cmake --build build --target style-json-parallel

# 3. Format Markdown/TypeScript
deno fmt

# 4. Format Lua
deno task dprint fmt
```

## CI Integration

The CI pipeline runs these checks automatically:

1. **JSON syntax validation** - `build-scripts/lint-json.sh`
2. **JSON formatting** - `cmake --build build --target style-json-parallel`
3. **Dialogue validation** - `tools/dialogue_validator.py`

If any check fails, the build will fail. Use the commands above to fix issues locally before
pushing.

## Editor Integration

### VS Code

Install these extensions for automatic formatting:

- **C++**: [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) with
  astyle integration
- **Deno**: [Deno](https://marketplace.visualstudio.com/items?itemName=denoland.vscode-deno) for
  Markdown/TypeScript

### Vim/Neovim

Add to your config:

```vim
" Format C++ with astyle on save
autocmd BufWritePre *.cpp,*.h !astyle --options=.astylerc %

" Format with deno
autocmd BufWritePre *.md,*.ts !deno fmt %
```

## Troubleshooting

### "json_formatter not found"

Make sure you configured CMake with `-DJSON_FORMAT=ON`:

```sh
cmake -B build -DJSON_FORMAT=ON
cmake --build build --target json_formatter
```

### "style-json-parallel target not found"

This target is only available on Unix-like systems (Linux, macOS). On Windows, use WSL or format
JSON files individually.

### astyle produces different results

Make sure you're using the `.astylerc` from the repository root:

```sh
astyle --options=.astylerc src/*.cpp
```
