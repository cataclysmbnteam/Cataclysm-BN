---
title: Contributing
---

:::tip{title="Opening new issue"}

Check [how to open a issue](./issues.md).

:::

# Want to help?

Help is appreciated, especially with:

- Reporting bugs. Including ones inherited from DDA.
- Identifying problems that aren't bugs. Misleading descriptions, values that are clearly off
  compared to similar cases, grammar mistakes, UI wonkiness that has an obvious solution.
- Making useless things useful or putting them on a blacklist. Adding deconstruction recipes for
  things that should have them but don't, replacing completely redundant items with their generic
  versions (say, "tiny marked bottle" with just "tiny bottle") in spawn lists.
- Tileset work. I'm occasionally adding new objects, like the new electric grid elements, and they
  could use new tiles.
- Balance analysis. Those should be rather in depth or "obviously correct". Obviously correct would
  be things like: "weapon x has strictly better stats than y, but y requires rarer components and
  has otherwise identical requirements".
- Identifying performance bottlenecks with a profiler.
- Code quality help.

## How-to

Contributing to Cataclysm: Bright Nights is easy:

1. Fork the repository here on GitHub.
2. Make your changes.
3. Send us a pull request.

:::note{title="License"}

Cataclysm: Bright Nights is released under the Creative Commons Attribution ShareAlike 3.0 license.
The code and content of the game is free to use, modify, and redistribute for any purpose
whatsoever. See http://creativecommons.org/licenses/by-sa/3.0/ for details. This means any
contribution you make to the project will also be covered by the same license, and this license is
irrevocable.

:::

## Guidelines

There are a couple of guidelines we suggest sticking to:

- Add this repository as an `upstream` remote.
- Keep your `upload` branch clean. This means you can easily pull changes made to this repository
  into yours.
- Create a new branch for each new feature or set of related bug fixes.
- Never merge from your local branches into your `upload` branch. Only update that by pulling from
  `upstream/upload`.

## Code Style

### C++

Code style is enforced across the codebase by `astyle`. See
[CODE_STYLE](../../dev/explanation/code_style.md) for details.

### JSON

JSON files are formatted using custom formatter available in `tools/format`. Visit
[JSON Style Guide](../../mod/json/explanation/json_style.md) for details.

### Markdown

Markdown files such as `doc/` are formatted using [`deno`](https://deno.com)'s built-in formatter.
Run [`deno fmt`](https://deno.land/manual/tools/formatter) anywhere to format markdown files. On
VSCode, you can set following configuration to auto-format markdown files on save:

```json
// .vscode/settings.json
{
  "[markdown]": {
    "editor.formatOnSave": true,
    "editor.defaultFormatter": "denoland.vscode-deno"
  }
}
```

## Translations

The translation of Cataclysm: BN is done using Transifex. Look at the
[translation project](https://app.transifex.com/bn-team/cataclysm-bright-nights/) for an up-to-date
list of supported languages.

For more information:

- [For translators](../../i18n/tutorial/transifex)
- [For developers](../../i18n/reference/translation)
- [For maintainers](../../i18n/guides/maintain)

## Example Workflow

### Setup your environment

_(This only needs to be done once.)_

1. Fork this repository here on GitHub.

1. Clone your fork locally.

```sh
$ git clone https://github.com/YOUR_USERNAME/Cataclysm-BN.git
# Clones your fork of the repository into the current directory in terminal
```

3. Set commit message template.

```sh
$ git config --local commit.template .gitmessage
```

4. Add this repository as a remote.

```sh
$ cd Cataclysm-BN
# Changes the active directory in the prompt to the newly cloned "Cataclysm-BN" directory
$ git remote add -f upstream https://github.com/cataclysmbnteam/Cataclysm-BN.git
# Assigns the original repository to a remote called "upstream"
```

For further details about commit message guidelines please visit:

- [codeinthehole.com](https://codeinthehole.com/tips/a-useful-template-for-commit-messages/)
- [chris.beams.io](https://chris.beams.io/posts/git-commit/)
- [help.github.com](https://help.github.com/articles/closing-issues-using-keywords/)

### Update your `upload` branch

1. Make sure you have your `upload` branch checked out.

```sh
$ git checkout upload
```

2. Pull the changes from the `upstream/upload` branch.

```sh
$ git pull --ff-only upstream upload
# gets changes from "upload" branch on the "upstream" remote
```

> **Note** If this gives you an error, it means you have committed directly to your local `upload`
> branch.
> [Click here for instructions on how to fix this issue](#why-does-git-pull---ff-only-result-in-an-error).

### Make your changes

0. Update your `upload` branch, if you haven't already.

1. For each new feature or bug fix, create a new branch.

```sh
$ git branch new_feature
# Creates a new branch called "new_feature"
$ git checkout new_feature
# Makes "new_feature" the active branch
```

2. Once you've committed some changes locally, you need to push them to your fork here on GitHub.

```sh
$ git push origin new_feature
# origin was automatically set to point to your fork when you cloned it
```

3. Once you're finished working on your branch, and have committed and pushed all your changes,
   submit a pull request from your `new_feature` branch to this repository's `upload` branch.

:::note

any new commits to the `new_feature` branch on GitHub will automatically be included in the pull
request, so make sure to only commit related changes to the same branch.

:::

### issue

and `{issue}` is the number of the issue you're closing after PR gets merged.

This would automatically close the issue when the PR is pulled in, and allows merges to work
slightly faster.

### closing multiple issues at once

```md
- {keyword} #{issue}, {keyword} #{issue}
```

See https://help.github.com/articles/closing-issues-using-keywords for more.

## Tooling support

Various tools are available to help you keep your contributions conforming to the appropriate style.
See [DEVELOPER_TOOLING](../../dev/reference/tooling.md) for more details.

## Advanced Techniques

Check out the [guides](../guides/remote_branch.md) for more advanced techniques. These guidelines
aren't essential, but they can make keeping things in order much easier.

## Frequently Asked Questions

### Why does `git pull --ff-only` result in an error?

If `git pull --ff-only` shows an error, it means that you've committed directly to your local
`upload` branch. To fix this, we create a new branch with these commits, find the point at which we
diverged from `upstream/upload`, and then reset `upload` to that point.

```sh
$ git pull --ff-only upstream upload
From https://github.com/cataclysmbnteam/Cataclysm-BN
 * branch            upload     -> FETCH_HEAD
fatal: Not possible to fast-forward, aborting.
$ git branch new_branch upload          # mark the current commit with a tmp branch
$ git merge-base upload upstream/upload
cc31d0... # the last commit before we committed directly to upload
$ git reset --hard cc31d0....
HEAD is now at cc31d0... ...
```

Now that `upload` has been cleaned up, we can easily pull from `upstream/upload`, and then continue
working on `new_branch`.

```sh
$ git pull --ff-only upstream upload
# gets changes from the "upstream" remote for the matching branch, in this case "upload"
$ git checkout new_branch
```

For more frequently asked questions, see the [developer FAQ](../../dev/reference/faq.md).
