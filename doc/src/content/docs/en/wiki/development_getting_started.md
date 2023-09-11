---
title: How to work on cataclysmbnteam/Cataclysm-BN
---

So you'd like to start making code changes and pull requests on this repository.

One important thing to note is that the "experimental" branch of this repository is actually where
the changes occur. The master branch of this repo simply tracks upstream.

1. Fork this repository. Due to how Github works you must not have an existing fork of the upstream
   repo (I.E. https://github.com/cataclysmbnteam/Cataclysm-BN/).
2. Create a branch on your fork.

```
git checkout -b $branch_name
git push --set-upstream origin $branch_name
```

3. Set the newly created branch track the experimental branch.

```
git branch $branch_name --set-upstream-to experimental
```

4. Finally pull the changes from experimental

```
git pull
```

5. Congratulations! You're good to go! You can now start committing and pushing changes to your
   fork. When you're done you can create pull request for the Coolthulhu repo.
