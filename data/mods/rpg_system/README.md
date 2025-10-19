# RPG System

An RPG progression mod for Cataclysm: Bright Nights that adds a LitRPG-style experience system.

## Features

- **Experience & Leveling**: Gain XP from killing monsters, level up to 40
- **Classes**: Choose from 4 base classes (Warrior, Mage, Rogue, Scout) and 8 prestige classes at level 10
- **Traits**: Unlock powerful traits
- **Stat Points**: Assign free stat points

## Balance

Worth ~25 character creation points. Worth much less early game, more late game. Not designed to be used with "Stats through Kills" mod.

## Screenshots

![System menu](https://cdn.imgchest.com/files/70905114f237.png) ![Class menu](https://cdn.imgchest.com/files/de1caa6782c7.png) ![Trait menu](https://cdn.imgchest.com/files/ae723f297ad8.png)

## Dev notes

When you add a class or trait, it must be added both to `traits.json` and to `rpg_mutations.lua`.

Each base class should give a total of 1 stat point per level, and each prestige class should give a total of 1.5 stat points per level. In addition to stat points, classes will give unique bonuses.

A guideline for trait level requirements is 5 if the highest required stat is 12, 10 if the highest required stat is 16, and no level requirement otherwise.

### TODOs

Have more interesting and dynamic stats and traits. They are all currently implemented either using `mod_` functions in lua, or using static mutation JSON bonuses. We could definitely add some more BombasticPerk style traits. Also, probably much of this is unbalanced.

Anyone who is interested should feel free to make changes!