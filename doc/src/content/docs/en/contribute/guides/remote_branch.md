---
title: Using remote tracking branches
---

Remote tracking branches allow you to easily stay in touch with this repository's `upload` branch,
as they automatically know which remote branch to get changes from.

```sh
$ git branch -vv
* upload      xxxx [origin/upload] ....
  new_feature xxxx ....
```

Here you can see we have two branches; `upload` which is tracking `origin/upload`, and `new_feature`
which isn't tracking any branch. In practice, what this means is that git won't know where to get
changes from.

```sh
$ git checkout new_feature
Switched to branch 'new_feature'
$ git pull
There is no tracking information for the current branch.
Please specify which branch you want to merge with.
```

In order to easily pull changes from `upstream/upload` into the `new_feature` branch, we can tell
git which branch it should track. (You can even do this for your local upload branch.)

```sh
$ git branch -u upstream/upload new_feature
Branch new_feature set up to track remote branch upload from upstream.
$ git pull
Updating xxxx..xxxx
....
```

You can also set the tracking information at the same time as creating the branch.

```sh
$ git branch new_feature_2 --track upstream/upload
Branch new_feature_2 set up to track remote branch upload from upstream.
```

> **Note**: Although this makes it easier to pull from `upstream/upload`, it doesn't change anything
> with regards to pushing. `git push` fails because you don't have permission to push to
> `upstream/upload`.

```sh
$ git push
error: The requested URL returned error: 403 while accessing https://github.com/cataclysmbnteam/Cataclysm-BN.git
fatal: HTTP request failed
$ git push origin
....
To https://github.com/YOUR_USERNAME/Cataclysm-BN.git
xxxx..xxxx  new_feature -> new_feature
```
