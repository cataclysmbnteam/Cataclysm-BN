# Cataclysm: Bright Nights

<header align="center">
  <a><img src="doc/src/content/docs/en/contribute/img/readme-title.png" title="screenshots of (clockwise from upper-right: Chaosvolt (x2), ExecutorBill, scarf005"></a>

[![en][icon-en]][en] [![ko][icon-ko]][ko]

</header>

[en]: ./README.md
[icon-en]: https://img.shields.io/badge/lang-en-red?style=flat-square
[ko]: ./README.ko.md
[icon-ko]: https://img.shields.io/badge/lang-ko-orange?style=flat-square

Cataclysm: Bright Nights is a roguelike with sci-fi elements set in a post-apocalyptic world.

While some have described it as a "zombie game", there is far more to Cataclysm than that. Struggle
to survive in a harsh, persistent, procedurally generated world. Scavenge the remnants of a dead
civilization for food, equipment, or, if you are lucky, a vehicle with a full tank of gas to get you
the hell out of there.

Fight to defeat or escape from a wide variety of powerful monstrosities, from zombies to giant
insects to killer robots and things far stranger and deadlier, and against the others like yourself,
who want what you have.

Find a way to stop the Cataclysm ... or become one of its strongest monsters.

> Cataclysm: Bright Nights is a fork of Cataclysm: Dark Days Ahead.
> [see the differences from its ancestor.](https://docs.cataclysmbn.org/en/game/changelog/).

## Downloads

### Executables

[![Stable][stable-releases-badge]][stable-releases] [![Recent][all-releases-badge]][all-releases]

### Source Code

[![Source Code][source-badge]][source] [![Zip Archive][clone-badge]][clone]

[stable-releases]: https://github.com/cataclysmbnteam/Cataclysm-BN/releases/latest "Download stable executable"
[stable-releases-badge]: https://img.shields.io/github/v/release/cataclysmbnteam/Cataclysm-BN?style=for-the-badge&color=success&label=stable "Download experimental executable"
[all-releases]: https://github.com/cataclysmbnteam/Cataclysm-BN/releases?q=prerelease%3Atrue&expanded=true
[all-releases-badge]: https://img.shields.io/github/v/release/cataclysmbnteam/Cataclysm-BN?style=for-the-badge&color=important&label=Latest%20Release&include_prereleases&sort=date
[source]: https://github.com/cataclysmbnteam/Cataclysm-BN/archive/master.zip "The source can be downloaded as a .zip archive"
[source-badge]: https://img.shields.io/badge/Zip%20Archive-black?style=for-the-badge&logo=github
[clone]: https://github.com/cataclysmbnteam/Cataclysm-BN/ "clone from our GitHub repo"
[clone-badge]: https://img.shields.io/badge/Clone%20From%20Repo-black?style=for-the-badge&logo=github

## Building

- [with makefile](doc/src/content/docs/en/dev/guides/building/makefile.md): supports Linux, macOS,
  and BSD.
- [with MSYS2](doc/src/content/docs/en/dev/guides/building/msys.md)
- [with vcpkjg](doc/src/content/docs/en/dev/guides/building/vs_vcpkg.md)
- [with cmake](doc/src/content/docs/en/dev/guides/building/cmake.md)
- [which compilers we support](doc/src/content/docs/en/dev/reference/compiler_support.md)

Please read the [official docs](https://docs.cataclysmbn.org/en/dev/guides/building/cmake/) for
details.

## Contributing

> Cataclysm: Bright Nights developed under Creative Commons Attribution ShareAlike 3.0 license. The
> code and content of the game is free to use, modify, and redistribute for any purpose whatsoever.
> See http://creativecommons.org/licenses/by-sa/3.0/ for details. Some code distributed with the
> project is not part of the project and is released under different software licenses, the files
> covered by different software licenses have their own license notices.

Please check the [official docs](https://docs.cataclysmbn.org/en/contribute/contributing/) for
details.

## Documentation

Gameplay and developing documentation is available in the [doc](./doc/src/content/docs/) directory
in markdown format. You can also

- visit the [official docs](https://docs.cataclysmbn.org/en/) site
- [build and serve the documentation locally](./doc/src/content/docs/en/contribute/docs.md)

## Community

[![Official Docs](https://img.shields.io/badge/Docs-LightGray?style=for-the-badge&logo=astro)][docs]
[![Discussions](https://img.shields.io/badge/Discussions-black?style=for-the-badge&logo=github)][discussion]
[![Discord](https://img.shields.io/discord/830879262763909202?style=for-the-badge&logo=discord)][discord]
[![Discussions](https://img.shields.io/badge/CDDA%20Modding-green?style=for-the-badge&logo=discord)][modding]

[discussion]: https://github.com/cataclysmbnteam/Cataclysm-BN/discussions
[discord]: https://discord.gg/XW7XhXuZ89
[modding]: https://discord.gg/B5q4XCa "Unofficial DDA modding community discord has a BN channel"
[docs]: https://docs.cataclysmbn.org "Official BN documentation"

## Frequently Asked Questions

#### Is there a tutorial?

Yes, you can find the tutorial in the **Special** menu at the main menu (be aware that due to many
code changes the tutorial may not function). You can also access documentation in-game via the `?`
key.

#### How can I change the key bindings?

Press the `?` key, followed by the `1` key to see the full list of key commands. Press the `+` key
to add a key binding, select which action with the corresponding letter key `a-w`, and then the key
you wish to assign to that action.

#### How can I start a new world?

**World** on the main menu will generate a fresh world for you. Select **Create World**.

#### I've found a bug. What should I do?

[Bug report](https://github.com/cataclysmbnteam/Cataclysm-BN/issues/new?template=bug_report.yml) can
be submitted via debug menu.

Run `Submit a bug report on github` inside the game to submit an issue.

|           1. open Options (ESC) -> Debug Menu (a)           |                      2. open Info (i)                       |
| :---------------------------------------------------------: | :---------------------------------------------------------: |
| ![](doc/src/content/docs/en/contribute/img/readme-bug1.png) | ![](doc/src/content/docs/en/contribute/img/readme-bug2.png) |
|            3. Submit a bug report on github (U)             |              4. An link to issue is generated               |
| ![](doc/src/content/docs/en/contribute/img/readme-bug3.png) | ![](doc/src/content/docs/en/contribute/img/readme-bug4.png) |

It will open a bug report on browser with `Version and configuration` filled in.

#### I would like to make a suggestion. What should I do?

- For simple ideas: please visit
  [our Discussions page](https://github.com/cataclysmbnteam/Cataclysm-BN/discussions/categories/ideas).
  It could be a new feature, a port request, a mod idea, or anything else.
- Please submit an issue on
  [our GitHub page](https://github.com/cataclysmbnteam/Cataclysm-BN/issues/) using
  [feature request form](https://github.com/cataclysmbnteam/Cataclysm-BN/issues/new?template=feature_request.yml).
