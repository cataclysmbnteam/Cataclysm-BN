---
title: 3rd party mods translation
---

The game allows 3rd party mods to ship their own translation files for any language the game
supports.

Below are simplified steps that most of the modders will be able to follow to set up translation
workflow (this one is for Windows):

1. Install Poedit (https://poedit.net/download)
2. Install Python 3 (https://www.python.org/downloads/)
3. Open Windows CMD and type `pip install polib`
4. Copy `extract_json_strings.py`, `dedup_pot_file.py` and `extract_mod_strings.bat` from the
   [lang/](https://github.com/cataclysmbnteam/Cataclysm-BN/tree/upload/lang) folder of repository to
   the mod folder you wish to translate
5. After launching this `.bat` file there will be `extracted_strings.pot` in the mod's `lang`
   directory, which can be edited with Poedit (see instructions in `TRANSLATING_MODS.md`)

For further details, including instructions for MacOS and Linux, see
[TRANSLATING_MODS.md](https://github.com/cataclysmbnteam/Cataclysm-BN/blob/upload/doc/TRANSLATING_MODS.md)

PR that implements mod translations, for reference:
https://github.com/cataclysmbnteam/Cataclysm-BN/pull/505

Example of the mod translation: https://github.com/Kenan2000/Bright-Nights-Kenan-Mod-Pack/pull/36
