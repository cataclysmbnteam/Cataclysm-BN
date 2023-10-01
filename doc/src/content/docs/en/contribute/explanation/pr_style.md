---
title: Pull Request Best Practices
---

If you file a PR but you're still working on it, please mark it as
[draft](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/about-pull-requests#draft-pull-requests).

![image](https://github.com/cataclysmbnteam/Cataclysm-BN/assets/54838975/aacc6d9e-e4b5-4290-9553-51e71be19c2c)

This can help speed up our review process by allowing us to only review the things that are ready
for it, and will prevent anything that isn't completely ready from being merged in.

It is not required to solve or reference an open issue to file a PR, however, if you do so, you need
to explain the problem your PR is solving in full detail.

## All PRs should have a `"Summary"` line

Summary is a one-line description of your change that will be extracted and added to the
[project changelog](../../game/changelog.md)

The format is: `SUMMARY: Category "description"`

The categories to choose from are: Features, Content, Interface, Mods, Balance, Bugfixes,
Performance, Infrastructure, Build, I18N.

Example: `SUMMARY: Content "Adds new mutation category 'Mouse'"`

See the [Changelog Guidelines](../reference/changelog_guidelines.md) for explanations of the
categories.

## Closing issues using keywords

[Github has a feature][keyword] that allows you to close issues using keywords in your PR
description.

If your PR closes (or 'fix') an existing issue, please include this somewhere in the description:

```md
- {keyword} #{issue}
```

for example: `- fixed #12345`

`{keyword}` must be one of the following:

- `close`, `closes`, `closed`
- `fix`, `fixes`, `fixed`
- `resolve`, `resolves`, `resolved`

This will link the PR to the issue, and close it when the PR is merged, for example:

![image](https://github.com/cataclysmbnteam/Cataclysm-BN/assets/54838975/fcb35752-1f06-4001-a7ea-b508c582afd4)

[keyword]: https://docs.github.com/en/issues/tracking-your-work-with-issues/linking-a-pull-request-to-an-issue#linking-a-pull-request-to-an-issue-using-a-keyword
