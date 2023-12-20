---
title: Changelog Guidelines
---

PR title follows [Convensional Commits](https://www.conventionalcommits.org/en/v1.0.0/) for easier
changelog generation. The format is one of:

```
<Category>: <PR subject>
<Category>(<Scope>, <Scope>, ...): <PR subject>
```

For Example, a PR title can be:

```
feat: add new mutation
feat(content, port): port mutation description from DDA
```

## Category

The category is the first word in the PR title. They specify the type of change being made. When in
doubt, use `feat` for new features, and `fix` for bugfixes. Here are some frequently used
categories:

### `feat`: Features

New features, additions, or balance changes.

### `fix`: Bugfixes

Anything that fixes a bug or makes the game more stable.

### `refactor`: Infrastructure

Make development easier without changing its behavior. For example:

- `C++` refactorings and overhaul
- `Json` reorganizations
- `docs/`, `.github/` and repository changes
- other development tools

### `build`: Build

Improve build process:

- more robust
- easier to use
- faster compile time

### Others

- `docs`: Documentation changes
- `style`: Code style changes (whitespace, formatting, etc), usu. fixing JSON formatting.
- `perf`: Performance Improvements
- `test`: Adding missing tests or correcting existing tests
- `ci`: Changes to CI process
- `chore`: Other changes that don't fit into any of the above categories
- `revert`: Reverts a previous commit

## Scopes

1. Use them inside parentheses after the category to further narrow the scope of your PR.
2. There are no limits to number of scopes.
3. They are optional, but recommended.
4. these are only guidelines, not rules. choose the best one for your PR freely!

### `<None>`: Player/Worldwide Features

Changes related to player:

- player can do something new (e.g: mutations, skills)
- something new can happen to the player (e.g: new disease)

Example PR title:

```
feat: strength training activity
feat: mutation system overhaul
```

### `content`: Contents

New contents like:

- new monsters
- new map areas
- new items
- new vehicles
- new doohickeys

Example PR title:

```
feat(content): semi-plausible smokeless gunpowder recipe
feat(content, port): game store
```

### `UI`: Interfaces

UI/UX changes like:

- adding/adjusting menus
- change shortcuts
- streamlining workflows
- quality of life improvements

Example PR title:

```
feat(UI): More info about items dropped with bags
feat(UI): overhaul encumbrance UI
```

### `i18n`: Internationalization

Improve translation and other languages support.

```
fix(UI, i18n): recipe names not translated unless learned
```

### `mods/<MOD_ID>`: Mods

- changes contained within a mod
- extends what is capable within a mod

Example PR title:

```
feat(mods/magiclysm, content): add missing owlbear pelts recipe
fix(mods/no_hope): No Hope doesn't make the world freezing
```

### `balance`: Balance Changes

Changes to game balance.

Example PR title:

```
feat(balance): Give moose pelts instead of hides
```

### `port`: Ports from DDA or other forks

Example PR title:

```
feat(content, port): game shop
```
