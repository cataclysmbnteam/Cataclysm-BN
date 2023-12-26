<!--
HOW TO USE: Under each "## Heading" below, enter information relevant to your pull request.
Leave the headings unless they don't apply to your PR.

NOTE: Please grant permission for repository maintainers to edit your PR.  It is EXTREMELY common for PRs to be held up due to trivial changes being requested and the author being unavailable to make them.  In web UI, you can do it by clicking the "Allow edits and access to secrets by maintainers" checkbox next to "Create Pull Request" button at the bottom of the editor, or by clicking the same checkbox in the sidebar after PR has been created.

CODE STYLE: The game uses automatic code formatting tools to keep code style consistent.  If your PR does not adhere to the style, the autofix.ci app will format the code for you and push the changes as a new commit.  You can also format the code yourself before committing it, it's faster that way and avoids the hurdle of keeping your branch up to date.  See relevant guides for more information: https://docs.cataclysmbn.org/en/contribute/contributing/#code-style

WARNING: If autofix.ci app did the formatting for you, YOU MUST DO EITHER OF THE FOLLOWING:
- Run `git pull` to merge the automated commit into your local PR branch.
- Format your code locally, and force push to your PR branch. 
If you don't do this, your following work will be based on the old commit, and may cause MERGE CONFLICT.
If you use GitHub's web editor to edit files, you shouldn't need to do this as the web editor works directly on the remote branch.

PR TITLE: Please follow Conventional Commits: https://www.conventionalcommits.org
This makes it clear at a glance what the PR is about.
For example:
    feat(content, mods/DinoMod): new dinosaur species
For more info on which categories are available, see: https://docs.cataclysmbn.org/en/contribute/changelog_guidelines/
If the PR is a port or adaptation of DDA content, please indicate it by adding "port" in PR title, like:
    feat(port): <feature name> from DDA
-->

## Purpose of change

<!-- 
With a few sentences, describe your reasons for making this change.  If it relates to an existing issue, you can link it with a # followed by the Github issue number, like #1234.

If your pull request *fully* resolves an issue, include the word "Fix" or "Fixes" before the issue number, like: "Fixes #1234".  This will make GitHub automatically close the issue once the PR is merged.  For multiple issues, repeat 'Fixes' multiple times: "Fixes #1234, Fixes #5678".

If there is no related issue, explain here what issue, feature, or other concern you are addressing.  If this is a bugfix, include steps to reproduce the original bug, so your fix can be verified.
-->

## Describe the solution

<!--
How does the feature work, or how does this fix a bug?  The easier you make your solution to understand, the faster it can get merged.

If this is a port or adaptation of DDA content, provide the link to the original PR (or PRs, if there were multiple) and explain what additional changes, if any, you made to the behavior.

Remember to attribute the original author(s): if you've just copied over the changes, add "Co-Authored-By: Author Name <author_email@example.com>" to the commit message (not the PR description!).  If you've cherry-picked the commits, which is the recommended way of porting, git should preserve the authorship information for you.
-->

## Describe alternatives you've considered

<!-- Explain any alternative solutions, different approaches, or possibilities you've considered using to solve the same problem. -->

## Testing

<!-- Describe what steps you took to test that this PR resolved the bug or added the feature, and what tests you performed to make sure it didn't cause any regressions.  Also include testing suggestions for reviewers and maintainers. -->

## Additional context

<!-- Add any other context (such as mock-ups, proof of concepts or screenshots) about the feature or bugfix here. -->

## Checklist

<!--
Certain common types of PRs may need additional code or documentation changes that are easy to forget about or may not be obvious if you're a new contributor.  The checklists below should help you track down what else may need to be done.

Please uncomment any relevant checklists, follow their steps and tick the checkboxes once you're done.  If your PR does not fall under these categories, you can ignore these lists.  If you have any questions or advice on how to improve these, feel free to contact us on our Discord server.  

If this is a C++ PR that modifies JSON loading or behavior:
- [ ] Document the changes in the appropriate location in the `doc/` folder.
- [ ] If documentation for this feature does not exist, please write it or at least note its lack in PR description.
- [ ] New localizable fields need to be added to the `lang/bn_extract_json_strings.sh` script if it does not support them yet.
- [ ] If applicable, add checks on game load that would validate the loaded data.
- [ ] If it modifies format of save files, please add migration from the old format.

If this is a PR that modifies build process or code organization:
- [ ] Please document the changes in the appropriate location in the `doc/` folder.
- [ ] If documentation for this feature or process does not exist, please write it.
- [ ] If the change alters versions of software required to build or work with the game, please document it.

If this is a PR that removes JSON entities:
- [ ] The removed JSON entities have new entries in `data/json/obsoletion/` folder or use some other migration process for existing saves.
-->
