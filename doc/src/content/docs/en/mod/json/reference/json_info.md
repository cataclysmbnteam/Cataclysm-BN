---
title: JSON INFO
---

:::note

This document is being split into multiple pages.

:::

:::danger

Many of the JSON files are not documented yet or is outdated. Check relevent source files to be
sure.

:::

This document describes the contents of the json files used in Cataclysm: Dark days ahead. You are
probably reading this if you want to add or change content of Catacysm: Dark days ahead and need to
learn more about what to find where and what each file and property does.

## Navigating the JSON

A lot of the JSON involves cross-references to other JSON entities. To make it easier to navigate,
we provide a script `tools/json_tools/cddatags.py` that can build a `tags` file for you.

To run the script you'll need Python 3. On Windows you'll probably need to install that, and
associate `.py` files with Python. Then open a command prompt, navigate to your CDDA folder, and run
`tools\json_tools\cddatags.py`.

To use this feature your editor will need [ctags support](http://ctags.sourceforge.net/). When
that's working you should be able to easily jump to the definition of any entity. For example, by
positioning your cursor over an id and hitting the appropriate key combination.

- In Vim, this feature exists by default, and you can jump to a definition using
  [`^\]`](http://vimdoc.sourceforge.net/htmldoc/tagsrch.html#tagsrch.txt).
- In Notepad++ go to "Plugins" -> "Plugins Admin" and enable the "TagLEET" plugin. Then select any
  id and press Alt+Space to open the references window.

## The `type` Property

Entries are distinguished by their `type` property. This property is mandatory for all entries.
Setting this entry to 'armor' for example means the game will expect properties specific to armor in
that entry. Also ties in with [`copy-from`](./items/json_inheritance.md#copy-from), if you want to
[inherit properties of another object](./items/json_inheritance.md), it must be of the same tipe.

## Formatting

When editing JSON files make sure you apply the correct formatting as shown below.

### Time duration

A string containing one or more pairs of number and time duration unit. Number and unit, as well as
each pair, can be separated by an arbitrary amount of spaces. Available units:

- "hours", "hour", "h" - one hour
- "days", "day", "d" - one day
- "minutes", "minute", "m" - one minute
- "turns", "turn", "t" - one turn,

Examples:

- " +1 day -23 hours 50m " `(1*24*60 - 23*60 + 50 == 110 minutes)`
- "1 turn 1 minutes 9 turns" (1 minute and 10 seconds because 1 turn is 1 second)

### Other formatting

```json
"//" : "comment", // Preferred method of leaving comments inside json files.
```

Some json strings are extracted for translation, for example item names, descriptions, etc. The
exact extraction is handled in `lang/extract_json_strings.py`. Apart from the obvious way of writing
a string without translation context, the string can also have an optional translation context (and
sometimes a plural form), by writing it like:

```json
"name": { "ctxt": "foo", "str": "bar", "str_pl": "baz" }
```

or, if the plural form is the same as the singular form:

```json
"name": { "ctxt": "foo", "str_sp": "foo" }
```

You can also add comments for translators by adding a "//~" entry like below. The order of the
entries does not matter.

```json
"name": {
    "//~": "as in 'foobar'",
    "str": "bar"
}
```

[Currently, only some JSON values support this syntax](../../../i18n/reference/translation#supported-json-values).

## Description and content of each JSON file

This section describes each json file and their contents. Each json has their own unique properties
that are not shared with other Json files (for example 'chapters' property used in books does not
apply to armor). This will make sure properties are only described and used within the context of
the appropriate JSON file.

## `data/json/` JSONs

### Ascii_arts

| Identifier | Description                                                                                    |
| ---------- | ---------------------------------------------------------------------------------------------- |
| id         | Unique ID. Must be one continuous word, use underscores if necessary.                          |
| picture    | Array of string, each entry is a line of an ascii picture and must be at most 42 columns long. |

```json
{
  "type": "ascii_art",
  "id": "cashcard",
  "picture": [
    "",
    "",
    "",
    "       <color_white>╔═══════════════════╗",
    "       <color_white>║                   ║",
    "       <color_white>║</color> <color_yellow>╔═   ╔═╔═╗╔═║ ║</color>   <color_white>║",
    "       <color_white>║</color> <color_yellow>║═ ┼ ║ ║═║╚╗║═║</color>   <color_white>║",
    "       <color_white>║</color> <color_yellow>╚═   ╚═║ ║═╝║ ║</color>   <color_white>║",
    "       <color_white>║                   ║",
    "       <color_white>║   RIVTECH TRUST   ║",
    "       <color_white>║                   ║",
    "       <color_white>║                   ║",
    "       <color_white>║ 555 993 55221 066 ║",
    "       <color_white>╚═══════════════════╝"
  ]
}
```

### Body_parts

| Identifier        | Description                                                                                                                                                                                                                                                                             |
| ----------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| id                | (_mandatory_) Unique ID. Must be one continuous word, use underscores if necessary.                                                                                                                                                                                                     |
| name              | (_mandatory_) In-game name displayed.                                                                                                                                                                                                                                                   |
| accusative        | (_mandatory_) Accusative form for this bodypart.                                                                                                                                                                                                                                        |
| heading           | (_mandatory_) How it's displayed in headings.                                                                                                                                                                                                                                           |
| heading_multiple  | (_optional_) Plural form of heading. (default: heading value)                                                                                                                                                                                                                           |
| hp_bar_ui_text    | (_optional_) How it's displayed next to the hp bar in the panel. (default: empty string)                                                                                                                                                                                                |
| encumbrance_text  | (_optional_) Description of effect when encumbered. (default: empty string)                                                                                                                                                                                                             |
| main_part         | (_optional_) What is the main part this one is attached to. (default: self)                                                                                                                                                                                                             |
| base_hp           | (_optional_) The amount of hp this part has before any modification. (default: `60`)                                                                                                                                                                                                    |
| opposite_part     | (_optional_) What is the opposite part ot this one in case of a pair. (default: self)                                                                                                                                                                                                   |
| essential         | (_optional_) Whether the character dies if this part drops to `0` HP.                                                                                                                                                                                                                   |
| hit_size          | (_optional_) Float. Size of the body part when doing an unweighted selection. (default: `0.`)                                                                                                                                                                                           |
| hit_size_relative | (_optional_) Float. Hit sizes for attackers who are smaller, equal in size, and bigger. (default: `[ 0, 0, 0 ]`                                                                                                                                                                         |
| hit_difficulty    | (_optional_) Float. How hard is it to hit a given body part, assuming "owner" is hit. Higher number means good hits will veer towards this part, lower means this part is unlikely to be hit by inaccurate attacks. Formula is `chance *= pow(hit_roll, hit_difficulty)` (default: `0`) |
| side              | (_optional_) Which side this body part is on. Default both.                                                                                                                                                                                                                             |
| stylish_bonus     | (_optional_) Mood bonus associated with wearing fancy clothing on this part. (default: `0`)                                                                                                                                                                                             |
| hot_morale_mod    | (_optional_) Mood effect of being too hot on this part. (default: `0`)                                                                                                                                                                                                                  |
| cold_morale_mod   | (_optional_) Mood effect of being too cold on this part. (default: `0`)                                                                                                                                                                                                                 |
| squeamish_penalty | (_optional_) Mood effect of wearing filthy clothing on this part. (default: `0`)                                                                                                                                                                                                        |
| bionic_slots      | (_optional_) How many bionic slots does this part have.                                                                                                                                                                                                                                 |

```json
{
  "id": "torso",
  "type": "body_part",
  "name": "torso",
  "accusative": { "ctxt": "bodypart_accusative", "str": "torso" },
  "heading": "Torso",
  "heading_multiple": "Torso",
  "hp_bar_ui_text": "TORSO",
  "encumbrance_text": "Dodging and melee is hampered.",
  "main_part": "torso",
  "opposite_part": "torso",
  "hit_size": 45,
  "hit_size_relative": [20, 33.33, 36.57],
  "hit_difficulty": 1,
  "side": "both",
  "legacy_id": "TORSO",
  "stylish_bonus": 6,
  "hot_morale_mod": 2,
  "cold_morale_mod": 2,
  "squeamish_penalty": 6,
  "base_hp": 60,
  "bionic_slots": 80
}
```

### Bionics

| Identifier                                                                                           | Description                                                                                                                                                                     |
| ---------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| id                                                                                                   | (_mandatory_) Unique ID. Must be one continuous word, use underscores if necessary.                                                                                             |
| name                                                                                                 | (_mandatory_) In-game name displayed.                                                                                                                                           |
| description                                                                                          | (_mandatory_) In-game description.                                                                                                                                              |
| flags                                                                                                | (_optional_) A list of flags. See JSON_FLAGS.md for supported values.                                                                                                           |
| act_cost                                                                                             | (_optional_) How many kJ it costs to activate the bionic. Strings can be used "1 kJ"/"1000 J"/"1000000 mJ" (default: `0`)                                                       |
| deact_cost                                                                                           | (_optional_) How many kJ it costs to deactivate the bionic. Strings can be used "1 kJ"/"1000 J"/"1000000 mJ" (default: `0`)                                                     |
| react_cost                                                                                           | (_optional_) How many kJ it costs over time to keep this bionic active, does nothing without a non-zero "time". Strings can be used "1 kJ"/"1000 J"/"1000000 mJ" (default: `0`) |
| time                                                                                                 | (_optional_) How long, when activated, between drawing cost. If 0, it draws power once. (default: `0`)                                                                          |
| upgraded_bionic                                                                                      | (_optional_) Bionic that can be upgraded by installing this one.                                                                                                                |
| required_bionics                                                                                     | (_optional_) Bionics which are required to install this bionic, and which cannot be uninstalled if this bionic is installed                                                     |
| available_upgrades                                                                                   | (_optional_) Upgrades available for this bionic, i.e. the list of bionics                                                                                                       |
| having this one referenced by `upgraded_bionic`.                                                     |                                                                                                                                                                                 |
| and how much this bionic encumber them.                                                              |                                                                                                                                                                                 |
| carrying capacity in grams, can be negative. Strings can be used - "5000 g" or "5 kg" (default: `0`) |                                                                                                                                                                                 |
|                                                                                                      | weight_capacity_modifier                                                                                                                                                        |
| (default: `1`)                                                                                       |                                                                                                                                                                                 |
| when this bionic is installed (e.g. because it replaces the fault biological part).                  |                                                                                                                                                                                 |
| included_bionics                                                                                     | (_optional_) Additional bionics that are installed automatically when this bionic                                                                                               |
| is installed. This can be used to install several bionics from one CBM item, which is useful as each |                                                                                                                                                                                 |
| of those can be activated independently.                                                             |                                                                                                                                                                                 |
| with another. If true this bionic does not require a CBM item to be defined. (default: `false`)      |                                                                                                                                                                                 |
| env_protec                                                                                           | (_optional_) How much environmental protection does this bionic provide on the                                                                                                  |
| specified body parts.                                                                                |                                                                                                                                                                                 |
| provide on the specified body parts.                                                                 |                                                                                                                                                                                 |
| bionic provide on the specified body parts.                                                          |                                                                                                                                                                                 |
| protect does this bionic provide on the specified body parts.                                        |                                                                                                                                                                                 |
| A list of body parts occupied by this bionic, and the number of bionic slots it take on those parts. |                                                                                                                                                                                 |
|                                                                                                      | capacity                                                                                                                                                                        |
| kJ"/"1000 J"/"1000000 mJ" (default: `0`)                                                             |                                                                                                                                                                                 |
| bionic can use to produce bionic power.                                                              |                                                                                                                                                                                 |
| allows you to plug your power banks to an external power source (solar backpack, UPS, vehicle etc)   |                                                                                                                                                                                 |
| via a cable. (default: `false`)                                                                      |                                                                                                                                                                                 |
| store.                                                                                               |                                                                                                                                                                                 |
| `0`)                                                                                                 |                                                                                                                                                                                 |
| (default: `1`)                                                                                       |                                                                                                                                                                                 |
| converted into power. Useful for CBM using PERPETUAL fuel like `muscle`, `wind` or `sun_light`.      |                                                                                                                                                                                 |
| (default: `0`)                                                                                       |                                                                                                                                                                                 |
| power. (default: `false`)                                                                            |                                                                                                                                                                                 |
| diminishing fuel_efficiency. Float between 0.0 and 1.0. (default: `nullopt`)                         |                                                                                                                                                                                 |
| (_optional_) `emit_id` of the field emitted by this bionic when it produces energy. Emit_ids are     |                                                                                                                                                                                 |
| defined in `emit.json`.                                                                              |                                                                                                                                                                                 |
| designated as follow: "DEX", "INT", "STR", "PER".                                                    |                                                                                                                                                                                 |
| enchantments applied by this CBM (see MAGIC.md for instructions on enchantment. NB: enchantments are |                                                                                                                                                                                 |
| not necessarily magic.)                                                                              |                                                                                                                                                                                 |
| installing this CBM, and lose when you uninstall this CBM. Spell classes are automatically gained.   |                                                                                                                                                                                 |
| fake_item                                                                                            | (_optional_) ID of fake item used by this bionic. Mandatory for gun and weapon                                                                                                  |
| bionics.                                                                                             |                                                                                                                                                                                 |

```json
{
    "id"           : "bio_batteries",
    "name"         : "Battery System",
    "active"       : false,
    "act_cost"     : 0,
    "time"         : 1,
    "fuel_efficiency": 1,
    "stat_bonus": [ [ "INT", 2 ], [ "STR", 2 ] ],
    "fuel_options": [ "battery" ],
    "fuel_capacity": 500,
    "encumbrance"  : [ [ "torso", 10 ], [ "arm_l", 10 ], [ "arm_r", 10 ], [ "leg_l", 10 ], [ "leg_r", 10 ], [ "foot_l", 10 ], [ "foot_r", 10 ] ],
    "description"  : "You have a battery draining attachment, and thus can make use of the energy contained in normal, everyday batteries. Use 'E' to consume batteries.",
    "canceled_mutations": ["HYPEROPIC"],
    "included_bionics": ["bio_blindfold"]
},
{
    "id": "bio_purifier",
    "type": "bionic",
    "name": "Air Filtration System",
    "description": "Surgically implanted in your trachea is an advanced filtration system.  If toxins, or airborne diseases find their way into your windpipe, the filter will attempt to remove them.",
    "occupied_bodyparts": [ [ "torso", 4 ], [ "mouth", 2 ] ],
    "env_protec": [ [ "mouth", 7 ] ],
    "bash_protec": [ [ "leg_l", 3 ], [ "leg_r", 3 ] ],
    "cut_protec": [ [ "leg_l", 3 ], [ "leg_r", 3 ] ],
    "bullet_protec": [ [ "leg_l", 3 ], [ "leg_r", 3 ] ],
    "learned_spells": [ [ "mint_breath", 2 ] ],
    "flags": [ "BIONIC_NPC_USABLE" ]
}
```

Bionics effects are defined in the code and new effects cannot be created through JSON alone. When
adding a new bionic, if it's not included with another one, you must also add the corresponding CBM
item in `data/json/items/bionics.json`. Even for a faulty bionic.

### Dreams

| Identifier | Description                                                          |
| ---------- | -------------------------------------------------------------------- |
| messages   | List of potential dreams.                                            |
| category   | Mutation category needed to dream.                                   |
| strength   | Mutation category strength required (1 = 20-34, 2 = 35-49, 3 = 50+). |

```json
{
  "messages": [
    "You have a strange dream about birds.",
    "Your dreams give you a strange feathered feeling."
  ],
  "category": "MUTCAT_BIRD",
  "strength": 1
}
```

### Disease

| Identifier         | Description                                                                                              |
| ------------------ | -------------------------------------------------------------------------------------------------------- |
| id                 | Unique ID. Must be one continuous word, use underscores if necessary.                                    |
| min_duration       | The minimum duration the disease can last. Uses strings "x m", "x s","x d".                              |
| max_duration       | The maximum duration the disease can last.                                                               |
| min_intensity      | The minimum intensity of the effect applied by the disease                                               |
| max_intensity      | The maximum intensity of the effect.                                                                     |
| health_threshold   | The amount of health above which one is immune to the disease. Must be between -200 and 200. (optional ) |
| symptoms           | The effect applied by the disease.                                                                       |
| affected_bodyparts | The list of bodyparts on which the effect is applied. (optional, default to num_bp)                      |

```json
{
  "type": "disease_type",
  "id": "bad_food",
  "min_duration": "6 m",
  "max_duration": "1 h",
  "min_intensity": 1,
  "max_intensity": 1,
  "affected_bodyparts": ["TORSO"],
  "health_threshold": 100,
  "symptoms": "foodpoison"
}
```

### Item Groups

Item groups have been expanded, look at [the detailed docs](./items/item_spawn.md) to their new
description. The syntax listed here is still valid.

| Identifier | Description                                                                                                                                                        |
| ---------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| id         | Unique ID. Must be one continuous word, use underscores if necessary                                                                                               |
| items      | List of potential item ID's. Chance of an item spawning is x/T, where X is the value linked to the specific item and T is the total of all item values in a group. |
| groups     | ??                                                                                                                                                                 |

```json
{
  "id": "forest",
  "items": [
    ["rock", 40],
    ["stick", 95],
    ["mushroom", 4],
    ["mushroom_poison", 3],
    ["mushroom_magic", 1],
    ["blueberries", 3]
  ],
  "groups": []
}
```

### Item Category

When you sort your inventory by category, these are the categories that are displayed.

| Identifier     | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                      |
| -------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| id             | Unique ID. Must be one continuous word, use underscores if necessary                                                                                                                                                                                                                                                                                                                                                                                             |
| name           | The name of the category. This is what shows up in-game when you open the inventory.                                                                                                                                                                                                                                                                                                                                                                             |
| zone           | The corresponding loot_zone (see loot_zones.json)                                                                                                                                                                                                                                                                                                                                                                                                                |
| sort_rank      | Used to sort categories when displaying. Lower values are shown first                                                                                                                                                                                                                                                                                                                                                                                            |
| priority_zones | When set, items in this category will be sorted to the priority zone if the conditions are met. If the user does not have the priority zone in the zone manager, the items get sorted into zone set in the 'zone' property. It is a list of objects. Each object has 3 properties: ID: The id of a LOOT_ZONE (see LOOT_ZONES.json), filthy: boolean. setting this means filthy items of this category will be sorted to the priority zone, flags: array of flags |

```json
{
  "id": "armor",
  "name": "ARMOR",
  "zone": "LOOT_ARMOR",
  "sort_rank": -21,
  "priority_zones": [{ "id": "LOOT_FARMOR", "filthy": true, "flags": ["RAINPROOF"] }]
}
```

### Materials

| Identifier             | Description                                                                                                                                                                                                |
| ---------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `id`                   | Unique ID. Lowercase snake_case. Must be one continuous word, use underscores if necessary.                                                                                                                |
| `name`                 | In-game name displayed.                                                                                                                                                                                    |
| `bash_resist`          | How well a material resists bashing damage.                                                                                                                                                                |
| `cut_resist`           | How well a material resists cutting damage.                                                                                                                                                                |
| `bullet_resist`        | How well a material resists bullet damage.                                                                                                                                                                 |
| `acid_resist`          | Ability of a material to resist acid.                                                                                                                                                                      |
| `elec_resist`          | Ability of a material to resist electricity.                                                                                                                                                               |
| `fire_resist`          | Ability of a material to resist fire.                                                                                                                                                                      |
| `chip_resist`          | Returns resistance to being damaged by attacks against the item itself.                                                                                                                                    |
| `bash_dmg_verb`        | Verb used when material takes bashing damage.                                                                                                                                                              |
| `cut_dmg_verb`         | Verb used when material takes cutting damage.                                                                                                                                                              |
| `dmg_adj`              | Description added to damaged item in ascending severity.                                                                                                                                                   |
| `dmg_adj`              | Adjectives used to describe damage states of a material.                                                                                                                                                   |
| `density`              | Density of a material.                                                                                                                                                                                     |
| `vitamins`             | Vitamins in a material. Usually overridden by item specific values.                                                                                                                                        |
| `wind_resist`          | Percentage 0-100. How effective this material is at stopping wind from getting through. Higher values are better. If none of the materials an item is made of specify a value, a default of 99 is assumed. |
| `warmth_when_wet`      | Percentage of warmth retained when fully drenched. Default is 0.2.                                                                                                                                         |
| `specific_heat_liquid` | Specific heat of a material when not frozen (J/(g K)). Default 4.186.                                                                                                                                      |
| `specific_heat_solid`  | Specific heat of a material when frozen (J/(g K)). Default 2.108.                                                                                                                                          |
| `latent_heat`          | Latent heat of fusion for a material (J/g). Default 334.                                                                                                                                                   |
| `freeze_point`         | Freezing point of this material (F). Default 32 F ( 0 C ).                                                                                                                                                 |
| `edible`               | Optional boolean. Default is false.                                                                                                                                                                        |
| `rotting`              | Optional boolean. Default is false.                                                                                                                                                                        |
| `soft`                 | Optional boolean. Default is false.                                                                                                                                                                        |
| `reinforces`           | Optional boolean. Default is false.                                                                                                                                                                        |

There are six -resist parameters: acid, bash, chip, cut, elec, and fire. These are integer values;
the default is 0 and they can be negative to take more damage.

```json
{
  "type": "material",
  "id": "hflesh",
  "name": "Human Flesh",
  "density": 5,
  "specific_heat_liquid": 3.7,
  "specific_heat_solid": 2.15,
  "latent_heat": 260,
  "edible": true,
  "rotting": true,
  "bash_resist": 1,
  "cut_resist": 1,
  "bullet_resist": 1,
  "acid_resist": 1,
  "fire_resist": 1,
  "elec_resist": 1,
  "chip_resist": 2,
  "dmg_adj": ["bruised", "mutilated", "badly mutilated", "thoroughly mutilated"],
  "bash_dmg_verb": "bruised",
  "cut_dmg_verb": "sliced",
  "vitamins": [["calcium", 0.1], ["vitB", 1], ["iron", 1.3]],
  "burn_data": [
    { "fuel": 1, "smoke": 1, "burn": 1, "volume_per_turn": "2500_ml" },
    { "fuel": 2, "smoke": 3, "burn": 2, "volume_per_turn": "10000_ml" },
    { "fuel": 3, "smoke": 10, "burn": 3 }
  ]
}
```

### Monster Groups

#### Group definition

| Identifier  | Description                                                                                                                                                                                                                                                             |
| ----------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `name`      | Unique ID. Must be one continuous word, use underscores if necessary.                                                                                                                                                                                                   |
| `default`   | Default monster, automatically fills in any remaining spawn chances.                                                                                                                                                                                                    |
| `monsters`  | To choose a monster for spawning, the game creates 1000 entries and picks one. Each monster will have a number of entries equal to it's "freq" and the default monster will fill in the remaining. See the table below for how to build the single monster definitions. |
| `is_safe`   | (bool) Check to not trigger safe-mode warning.                                                                                                                                                                                                                          |
| `is_animal` | (bool) Check if that group has only normal animals.                                                                                                                                                                                                                     |

#### Monster definition

| Identifier        | Description                                                                                                                                                                                                                                                                                                                                                                                                                         |
| ----------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `monster`         | The monster's unique ID, eg. `"mon_zombie"`.                                                                                                                                                                                                                                                                                                                                                                                        |
| `freq`            | Chance of occurrence, x/1000.                                                                                                                                                                                                                                                                                                                                                                                                       |
| `cost_multiplier` | How many monsters each monster in this definition should count as, if spawning a limited number of monsters.                                                                                                                                                                                                                                                                                                                        |
| `pack_size`       | (_optional_) The minimum and maximum number of monsters in this group that should spawn together. (default: `[1,1]`)                                                                                                                                                                                                                                                                                                                |
| `conditions`      | Conditions limit when monsters spawn. Valid options: `SUMMER`, `WINTER`, `AUTUMN`, `SPRING`, `DAY`, `NIGHT`, `DUSK`, `DAWN`. Multiple Time-of-day conditions (`DAY`, `NIGHT`, `DUSK`, `DAWN`) will be combined together so that any of those conditions makes the spawn valid. Multiple Season conditions (`SUMMER`, `WINTER`, `AUTUMN`, `SPRING`) will be combined together so that any of those conditions makes the spawn valid. |
| `starts`          | (_optional_) This entry becomes active after this time. (Measured in hours)                                                                                                                                                                                                                                                                                                                                                         |
| `ends`            | (_optional_) This entry becomes inactive after this time. (Measured in hours)                                                                                                                                                                                                                                                                                                                                                       |

```json
{
  "name": "GROUP_ANT",
  "default": "mon_ant",
  "monsters": [
    { "monster": "mon_ant_larva", "freq": 40, "multiplier": 0 },
    { "monster": "mon_ant_soldier", "freq": 90, "multiplier": 5 },
    { "monster": "mon_ant_queen", "freq": 0, "multiplier": 0 },
    {
      "monster": "mon_thing",
      "freq": 100,
      "multiplier": 0,
      "pack_size": [3, 5],
      "conditions": ["DUSK", "DAWN", "SUMMER"]
    }
  ]
}
```

### Monster Factions

| Identifier     | Description                                                                                                                      |
| -------------- | -------------------------------------------------------------------------------------------------------------------------------- |
| `name`         | Unique ID. Must be one continuous word, use underscores when necessary.                                                          |
| `base_faction` | Optional base faction. Relations to other factions are inherited from it and relations of other factions to this one check this. |
| `by_mood`      | Be hostile towards this faction when angry, neutral otherwise. Default attitude to all other factions.                           |
| `neutral`      | Always be neutral towards this faction.                                                                                          |
| `friendly`     | Always be friendly towards this faction. By default a faction is friendly towards itself.                                        |
| `hate`         | Always be hostile towards this faction. Will change target to monsters of this faction if available.                             |

```json
{
  "name": "cult",
  "base_faction": "zombie",
  "by_mood": ["blob"],
  "neutral": ["nether"],
  "friendly": ["blob"],
  "hate": ["fungus"]
}
```

### Monsters

See MONSTERS.md

### Names

```json
{ "name" : "Aaliyah", "gender" : "female", "usage" : "given" }, // Name, gender, "given"/"family"/"city" (first/last/city name).
// NOTE: Please refrain from adding name PR's in order to maintain kickstarter exclusivity
```

### Profession item substitution

Defines item replacements that are applied to the starting items based upon the starting traits.
This allows for example to replace wool items with non-wool items when the characters starts with
the wool allergy trait.

If the JSON objects contains a "item" member, it defines a replacement for the given item, like
this:

```json
{
  "type": "profession_item_substitutions",
  "item": "sunglasses",
  "sub": [
    { "present": [ "HYPEROPIC" ], "new": [ "fitover_sunglasses" ] },
    { "present": [ "MYOPIC" ], "new": [ { "fitover_sunglasses", "ratio": 2 } ] }
  ]
}
```

This defines each item of type "sunglasses" shall be replaced with:

- an item "fitover_sunglasses" if the character has the "HYPEROPIC" trait,
- two items "fitover_sunglasses" if the character has the "MYOPIC" trait.

If the JSON objects contains a "trait" member, it defines a replacement for multiple items that
applies when the character has the given trait:

````json
{
  "type": "profession_item_substitutions",
  "trait": "WOOLALLERGY",
  "sub": [
    { "item": "blazer", "new": [ "jacket_leather_red" ] },
    { "item": "hat_hunting", "new": [ { "item": "hat_cotton", "ratio": 2 } ] }
  ]
}
```json
This defines characters with the WOOLALLERGY trait get some items replaced:
- "blazer" is converted into "jacket_leather_red",
- each "hat_hunting" is converted into *two* "hat_cotton" items.

### Professions

Professions are specified as JSON object with "type" member set to "profession":

```json
{
    "type": "profession",
    "id": "hunter",
    ...
}
````

The id member should be the unique id of the profession.

The following properties (mandatory, except if noted otherwise) are supported:

#### `description`

(string)

The in-game description.

#### `name`

(string or object with members "male" and "female")

The in-game name, either one gender-neutral string, or an object with gender specific names.
Example:

```json
"name": {
    "male": "Groom",
    "female": "Bride"
}
```

#### `points`

(integer)

Point cost of profession. Positive values cost points and negative values grant points.

#### `addictions`

(optional, array of addictions)

List of starting addictions. Each entry in the list should be an object with the following members:

- "type": the string id of the addiction (see JSON_FLAGS.md),
- "intensity": intensity (integer) of the addiction.

Example:

```json
"addictions": [
    { "type": "nicotine", "intensity": 10 }
]
```

Mods can modify this list (requires `"edit-mode": "modify"`, see example) via "add:addictions" and
"remove:addictions", removing requires only the addiction type. Example:

```json
{
  "type": "profession",
  "id": "hunter",
  "edit-mode": "modify",
  "remove:addictions": [
    "nicotine"
  ],
  "add:addictions": [
    { "type": "alcohol", "intensity": 10 }
  ]
}
```

#### `skills`

(optional, array of skill levels)

List of starting skills. Each entry in the list should be an object with the following members:

- "name": the string id of the skill (see skills.json),
- "level": level (integer) of the skill. This is added to the skill level that can be chosen in the
  character creation.

Example:

```json
"skills": [
    { "name": "archery", "level": 2 }
]
```

Mods can modify this list (requires `"edit-mode": "modify"`, see example) via "add:skills" and
"remove:skills", removing requires only the skill id. Example:

```json
{
  "type": "profession",
  "id": "hunter",
  "edit-mode": "modify",
  "remove:skills": [
    "archery"
  ],
  "add:skills": [
    { "name": "computer", "level": 2 }
  ]
}
```

#### `items`

(optional, object with optional members "both", "male" and "female")

Items the player starts with when selecting this profession. One can specify different items based
on the gender of the character. Each lists of items should be an array of items ids, or pairs of
item ids and snippet ids. Item ids may appear multiple times, in which case the item is created
multiple times. The syntax for each of the three lists is identical.

Example:

```json
"items": {
    "both": [
        "pants",
        "rock",
        "rock",
        ["tshirt_text", "allyourbase"],
        "socks"
    ],
    "male": [
        "briefs"
    ],
    "female": [
        "panties"
    ]
}
```

This gives the player pants, two rocks, a t-shirt with the snippet id "allyourbase" (giving it a
special description), socks and (depending on the gender) briefs or panties.

Mods can modify the lists of existing professions. This requires the "edit-mode" member with value
"modify" (see example). Adding items to the lists can be done with via "add:both" / "add:male" /
"add:female". It allows the same content (it allows adding items with snippet ids). Removing items
is done via "remove:both" / "remove:male" / "remove:female", which may only contain items ids.

Example for mods:

```json
{
  "type": "profession",
  "id": "hunter",
  "edit-mode": "modify",
  "items": {
    "remove:both": [
      "rock",
      "tshirt_text"
    ],
    "add:both": ["2x4"],
    "add:female": [
      ["tshirt_text", "allyourbase"]
    ]
  }
}
```

This mod removes one of the rocks (the other rock is still created), the t-shirt, adds a 2x4 item
and gives female characters a t-shirt with the special snippet id.

#### `pets`

(optional, array of string mtype_ids )

A list of strings, each is the same as a monster id player will start with these as tamed pets.

#### `vehicle`

(optional, string vproto_id )

A string, which is the same as a vehicle ( vproto_id ) player will start with this as a nearby
vehicle. ( it will find the nearest road and place it there, then mark it as "remembered" on the
overmap )

#### `flags`

(optional, array of strings)

A list of flags. TODO: document those flags here.

Mods can modify this via `add:flags` and `remove:flags`.

#### `cbms`

(optional, array of strings)

A list of CBM ids that are implanted in the character.

Mods can modify this via `add:CBMs` and `remove:CBMs`.

#### `traits`

(optional, array of strings)

A list of trait/mutation ids that are applied to the character.

Mods can modify this via `add:traits` and `remove:traits`.

### Recipes

Recipes represent both craft and uncraft (disassembly) recipes.

```json
"type": "recipe",            // Recipe type. Possible values: 'recipe' (craft recipe) and 'uncraft' (uncraft recipe).
"reversible": false,         // Generate an uncraft recipe that is a reverse of this craft recipe.
"result": "javelin",         // ID of resulting item. By default, also used as the ID of the recipe.
"id_suffix": "",             // Optional (default: empty string). Some suffix to make the ID of the recipe unique. The ID of the recipe is "<result><id_suffix>".
"category": "CC_WEAPON",     // Category of crafting recipe. CC_NONCRAFT used for disassembly recipes
"subcategory": "",           // Subcategory of crafting recipe. 
"override": false,           // Optional (default: false). If false and the ID of the recipe is already used by another recipe, loading of recipes fails. If true and a recipe with the ID is already defined, the existing recipe is replaced by the new recipe.
"obsolete": false,           // Optional (default: false). Marks this recipe as obsolete. Use this instead of outright deleting it to preserve save compatibility.
"delete_flags": [ "CANNIBALISM" ], // Optional (default: empty list). Flags specified here will be removed from the resultant item upon crafting. This will override flag inheritance, but *will not* delete flags that are part of the item type itself.
"contained": false,          // Optional (default: false). Spawn produced item in a container.
"container": "can_medium",   // Optional. Override container to spawn item in (by default uses item's default container)
"charges": 1,                // Optional (default: null). Override default charges for count-by-charges items (ammo, comestibles, etc.)
"result_mult": 1,            // Optional (default: 1). Produce multiple items (or stacks for count-by-charges items) from single craft
"skill_used": "fabrication", // Skill trained and used for success checks
"skills_required": [["survival", 1], ["throw", 2]], // Skills required to unlock recipe
"book_learn": [              // (optional) Array of books that this recipe can be learned from. Each entry contains the id of the book and the skill level at which it can be learned.
    [ "textbook_anarch", 7, "something" ], // The optional third entry defines a name for the recipe as it should appear in the books description (default is the name of resulting item of the recipe)
    [ "textbook_gaswarfare", 8, "" ] // If the name is empty, the recipe is hidden, it will not be shown in the description of the book.
],
"difficulty": 3,             // Difficulty of success check
"time": "5 m",               // Time to perform the recipe specified as time string (can use minutes, hours, etc.)
"time": 5000,                // Legacy time format, in moves. Won't be supported in the future. Here, 1000 ~= 10 turns ~= 10 seconds of game time
"autolearn": true,           // Automatically learned upon gaining required skills
"autolearn" : [              // Automatically learned upon gaining listed skills
    [ "survival", 2 ],
    [ "fabrication", 3 ]
],
"decomp_learn" : 4,          // Can be learned by disassembling an item of same type as result at this level of the skill_used
"decomp_learn" : [           // Can be learned by disassembling an item of same type as result at specified levels of skills
    [ "survival", 1 ],
    [ "fabrication", 2 ]
],
"never_learn": false,        // Optional (default: false). Prevents this recipe from being learned by the player. For debug or NPC purposes.
"batch_time_factors": [25, 15], // Optional factors for batch crafting time reduction. First number specifies maximum crafting time reduction as percentage, and the second number the minimal batch size to reach that number. In this example given batch size of 20 the last 6 crafts will take only 3750 time units.
"flags": [                   // A set of strings describing boolean features of the recipe
  "BLIND_EASY",
  "ANOTHERFLAG"
],
"construction_blueprint": "camp", // an optional string containing an update_mapgen_id.  Used by faction camps to upgrade their buildings. If non-empty, the function must exist.
"on_display": false,         // this is a hidden construction item, used by faction camps to calculate construction times but not available to the player
"using": [                   // Optional, string or array. Additional external requirements to use.
  "butter_resealable_containers"
],
"qualities": [               // Generic qualities of tools needed to craft
  {"id":"CUT","level":1,"amount":1}
],
"tools": [                   // Specific tools needed to craft
[
    [ "fire", -1 ]           // Charges consumed when tool is used, -1 means no charges are consumed
]],
"components": [              // Equivalent tools or components are surrounded by a single set of brackets
[
  [ "spear_wood", 1 ],       // Number of charges/items required
  [ "pointy_stick", 1 ]
],
[
  [ "rag", 1 ],
  [ "leather", 1 ],
  [ "fur", 1 ]
],
[
  [ "plant_fibre", 20, false ], // Optional flag for recoverability, default is true.
  [ "sinew", 20, false ],
  [ "thread", 20, false ],
  [ "duct_tape", 20 ]  // Certain items are flagged as unrecoverable at the item definition level.
]
]
```

#### Overlapping recipe component requirements

If recipes have requirements which overlap, this makes it more difficult for the game to calculate
whether it is possible to craft a recipe at all.

For example, the survivor telescope recipe has the following requirements (amongst others):

```
1 high-quality lens
AND
1 high-quality lens OR 1 small high-quality lens
```

These overlap because both list the high-quality lens.

A small amount of overlap (such as the above) can be handled, but if you have too many component
lists which overlap in too many ways, then you may see an error during recipe finalization that your
recipe is too complex. In this case, the game may not be able to corectly predict whether it can be
crafted.

To work around this issue, if you do not wish to simplify the recipe requirements, then you can
split your recipe into multiple steps. For example, if we wanted to simplify the above survivor
telescope recipe we could introduce an intermediate item "survivor eyepiece", which requires one of
either lens, and then the telescope would require a high-quality lens and an eyepiece. Overall, the
requirements are the same, but neither recipe has any overlap.

For more details, see
[this pull request](https://github.com/cataclysmbnteam/Cataclysm-BN/pull/36657) and the
[related issue](https://github.com/cataclysmbnteam/Cataclysm-BN/issues/32311).

### Constructions

```json
"id": "constr_pit_spiked",                                          // Identifier of the construction
"group": "spike_pit",                                               // Construction group, used to provide description and group related constructions in UI (e.g. different stages of some construction).
"category": "DIG",                                                  // Construction category
"required_skills": [ [ "survival", 1 ] ],                           // Skill levels required to undertake construction
"time": "30 m",                                                     // Time required to complete construction. Integers will be read as minutes or a time string can be used.
"components": [ [ [ "spear_wood", 4 ], [ "pointy_stick", 4 ] ] ],   // Items used in construction
"using": [ [ "welding_standard", 5 ] ],                             // (Optional) External requirements to use
"pre_note": "I am a dwarf and I'm digging a hole!",                 // (Optional) Annotation
"pre_flags": [ "DIGGABLE", "FLAT" ],                                // (Optional) Flags the terrain must have to be built on
"pre_terrain": "t_pit",                                             // (Optional) Required terrain to build on
"pre_furniture": "f_sandbag_half",                                  // (Optional) Required furniture to build on
"pre_special": "check_down_OK",                                     // (Optional) Hardcoded tile validity checks, refer to construction.cpp 
"post_terrain": "t_pit_spiked",                                     // (Optional) Terrain type after construction is complete
"post_furniture": "f_sandbag_wall",                                 // (Optional) Furniture type after construction is complete
"post_special": "done_dig_stair",                                   // (Optional) Hardcoded completion function, refer to construction.cpp
"post_flags": [ "keep_items" ],                                     // (Optional) Additional hardcoded effects, refer to construction.cpp. The only one available as of September 2022 is `keep_items`
"byproducts": [ { "item": "pebble", "charges": [ 3, 6 ] } ],        // (Optional) construction byproducts
"vehicle_start": false,                                             // (Optional, default false) Whether it will create a vehicle (for hardcode purposes)
"on_display": false,                                                // (Optional, default true) Whether it will show in player's UI
"dark_craftable": true,                                             // (Optional, default false) Whether it can be constructed in dark
```

### Construction groups

```json
"id": "build_wooden_door",            // Group identifier
"name": "Build Wooden Door",          // Description string displayed in the construction menu
```

### Construction sequences

Constuction sequences are needed for automatic calculation of basecamp blueprint requirements. Each
construction sequence describes steps needed to construct specified terrain or furniture on an empty
dirt tile. At any moment, only 1 construction sequence may be defined for any terrain or furniture.
For convenience, each non-blacklisted construction recipe that starts from an empty dirt tile
automatically generates a one-element-long construction sequence, but only as long as there are no
other such construction recipes that would produce the same sequence and there isn't an explicitly
defined sequence with same results.

```json
"id": "f_workbench",                // Sequence identifier
"blacklisted": false,               // (Optional) Whether this sequence is blacklisted
"post_furniture": "f_workbench",    // (Optional) Identifier of resulting furniture
"post_terrain": "t_rootcellar",     // (Optional) Identifier of resulting terrain
"elems": [                          // Array of construction identifiers.
  "constr_pit",                     //   First construction must start from an empty dirt tile,
  "constr_rootcellar"               //   and last construction must result in either post_furniture
]                                   //   or post_terrain, whichever one has been specified.
                                    //   Leave it empty to exclude the terrain/furniture from blueprint autocalc.
```

### Scent_types

| Identifier        | Description                                                                    |
| ----------------- | ------------------------------------------------------------------------------ |
| id                | Unique ID. Must be one continuous word, use underscores if necessary.          |
| receptive_species | Species able to track this scent. Must use valid ids defined in `species.json` |

```json
{
  "type": "scent_type",
  "id": "sc_flower",
  "receptive_species": ["MAMMAL", "INSECT", "MOLLUSK", "BIRD"]
}
```

### Scores and Achievements

Scores are defined in two or three steps based on _events_. To see what events exist and what data
they contain, read [`event.h`](../../../../../../../src/event.h).

Each event contains a certain set of fields. Each field has a string key and a `cata_variant` value.
The fields should provide all the relevant information about the event.

For example, consider the `gains_skill_level` event. You can see this specification for it in
`event.h`:

```json
template<>
struct event_spec<event_type::gains_skill_level> {
    static constexpr std::array<std::pair<const char *, cata_variant_type>, 3> fields = {{
            { "character", cata_variant_type::character_id },
            { "skill", cata_variant_type::skill_id },
            { "new_level", cata_variant_type::int_ },
        }
    };
};
```

From this, you can see that this event type has three fields:

- `character`, with the id of the character gaining the level.
- `skill`, with the id of the skill gained.
- `new_level`, with the integer level newly acquired in that skill.

Events are generated by the game when in-game circumstances dictate. These events can be transformed
and summarized in various ways. There are three concepts involved: event streams, event statistics,
and scores.

- Each `event_type` defined by the game generates an event stream.
- Further event streams can be defined in json by applying an `event_transformation` to an existing
  event stream.
- An `event_statistic` summarizes an event stream into a single value (usually a number, but other
  types of value are possible).
- A `score` uses such a statistic to define an in-game score which players can see.

#### `event_transformation`

An `event_transformation` can modify an event stream, producing another event stream.

The input stream to be transformed is specified either as an `"event_type"`, to use one of the
built-in event type streams, or an `"event_transformation"`, to use another json-defined transformed
event stream.

Any or all of the following alterations can be made to the event stream:

- Add new fields to each event based on event field transformations. The event field transformations
  can be found in
  [`event_field_transformation.cpp`](https://github.com/cataclysmbnteam/Cataclysm-BN/blob/main/src/event_field_transformations.cpp).
- Filter events based on the values they contain to produce a stream containing some subset of the
  input stream.
- Drop some fields which are not of interest in the output stream.

Here are examples of each modification:

```json
"id": "avatar_kills_with_species",
"type": "event_transformation",
"event_type": "character_kills_monster", // Transformation acts upon events of this type
"new_fields": { // A dictionary of new fields to add to the event
    // The key is the new field name; the value should be a dictionary of one element
    "species": {
        // The key specifies the event_field_transformation to apply; the value specifies
        // the input field whose value should be provided to that transformation.
        // So, in this case, we are adding a new field 'species' which will
        // contain the species of the victim of this kill event.
        "species_of_monster": "victim_type"
    }
}
```

```json
"id": "moves_on_horse",
"type": "event_transformation",
"event_type" : "avatar_moves", // An event type.  The transformation will act on events of this type
"value_constraints" : { // A dictionary of constraints
    // Each key is the field to which the constraint applies
    // The value specifies the constraint.
    // "equals" can be used to specify a constant string value the field must take.
    // "equals_statistic" specifies that the value must match the value of some statistic (see below)
    "mount" : { "equals": "mon_horse" }
}
// Since we are filtering to only those events where 'mount' is 'mon_horse', we
// might as well drop the 'mount' field, since it provides no useful information.
"drop_fields" : [ "mount" ]
```

#### `event_statistic`

As with `event_transformation`, an `event_statistic` requires an input event stream. That input
stream can be specified in the same was as for `event_transformation`, via one of the following two
entries:

```json
"event_type" : "avatar_moves" // Events of this built-in type
"event_transformation" : "moves_on_horse" // Events resulting from this json-defined transformation
```

Then it specifies a particular `stat_type` and potentially additional details as follows:

The number of events:

```json
"stat_type" : "count"
```

The sum of the numeric value in the specified field across all events:

```json
"stat_type" : "total"
"field" : "damage"
```

The maximum of the numeric value in the specified field across all events:

```json
"stat_type" : "maximum"
"field" : "damage"
```

The minimum of the numeric value in the specified field across all events:

```json
"stat_type" : "minimum"
"field" : "damage"
```

Assume there is only a single event to consider, and take the value of the given field for that
unique event:

```json
"stat_type": "unique_value",
"field": "avatar_id"
```

Regardless of `stat_type`, each `event_statistic` can also have:

```json
// Intended for use in describing scores and achievement requirements.
"description": "Number of things"
```

#### `score`

Scores simply associate a description to an event for formatting in tabulations of scores. The
`description` specifies a string which is expected to contain a `%s` format specifier where the
value of the statistic will be inserted.

Note that even though most statistics yield an integer, you should still use `%s`.

If the underlying statistic has a description, then the score description is optional. It defaults
to "<statistic description>: <value>".

```json
"id": "score_headshots",
"type": "score",
"description": "Headshots: %s",
"statistic": "avatar_num_headshots"
```

#### `achievement`

Achievements are goals for the player to aspire to, in the usual sense of the term as popularised in
other games.

An achievement is specified via requirements, each of which is a constraint on an `event_statistic`.
For example:

```json
{
  "id": "achievement_kill_zombie",
  "type": "achievement",
  // The achievement name and description are used for the UI.
  // Description is optional and can provide extra details if you wish.
  "name": "One down, billions to go\u2026",
  "description": "Kill a zombie",
  "requirements": [
    // Each requirement must specify the statistic being constrained, and the
    // constraint in terms of a comparison against some target value.
    { "event_statistic": "num_avatar_zombie_kills", "is": ">=", "target": 1 }
  ]
},
```

The `"is"` field must be `">="`, `"<="` or `"anything"`. When it is not `"anything"` the `"target"`
must be present, and must be an integer.

There are further optional fields:

```json
"hidden_by": [ "other_achievement_id" ]
```

Give a list of other achievement ids. This achievement will be hidden (i.e. not appear in the
achievements UI) until all of the achievements listed have been completed.

Use this to prevent spoilers or to reduce clutter in the list of achievements.

```json
"skill_requirements": [ { "skill": "archery", "is": ">=", "level": 5 } ]
```

This allows a skill level requirement (either an upper or lower bound) on when the achievement can
be claimed. The `"skill"` field uses the id of a skill.

Note that like `"time_constraint"` below achievements can only be captured when a statistic listed
in `"requirements"` changes.

```json
"kill_requirements": [ { "faction": "ZOMBIE", "is": ">=", "count": 1 }, { "monster": "mon_sludge_crawler", "is": ">=", "count": 1 } ],
```

This allows a kill requirement (either an upper or lower bound) on when the achievement can be
claimed. Can be defined with either `"faction"` or `"monster"` as a target, using species ids from
`species.json` or a specific monster id.

Only one of the `"monster"`/`"faction"` fields can be used per entry. If neither are used, any
monster will fulfill the requirements.

NPCs cannot currently be defined as a target.

Note that like `"time_constraint"` below achievements can only be captured when a statistic listed
in `"requirements"` changes.

```json
"time_constraint": { "since": "game_start", "is": "<=", "target": "1 minute" }
```

This allows putting a time limit (either a lower or upper bound) on when the achievement can be
claimed. The `"since"` field can be either `"game_start"` or `"cataclysm"`. The `"target"` describes
an amount of time since that reference point.

Note that achievements can only be captured when a statistic listed in their requirements changes.
So, if you want an achievement which would normally be triggered by reaching some time threshold
(such as "survived a certain amount of time") then you must place some requirement alongside it to
trigger it after that time has passed. Pick some statistic which is likely to change often, and add
an `"anything"` constraint on it. For example:

```json
{
  "id": "achievement_survive_one_day",
  "type": "achievement",
  "description": "The first day of the rest of their unlives",
  "time_constraint": { "since": "game_start", "is": ">=", "target": "1 day" },
  "requirements": [ { "event_statistic": "num_avatar_wake_ups", "is": "anything" } ]
},
```

This is a simple "survive a day" but is triggered by waking up, so it will be completed when you
wake up for the first time after 24 hours into the game.

### Skills

```json
"id" : "smg",  // Unique ID. Must be one continuous word, use underscores if necessary
"name" : "submachine guns",  // In-game name displayed
"description" : "Your skill with submachine guns and machine pistols. Halfway between a pistol and an assault rifle, these weapons fire and reload quickly, and may fire in bursts, but they are not very accurate.", // In-game description
"tags" : ["gun_type"]  // Special flags (default: none)
```

## `json/` JSONs

### Harvest

```json
{
    "id": "jabberwock",
    "type": "harvest",
    "message": "You messily hack apart the colossal mass of fused, rancid flesh, taking note of anything that stands out.",
    "entries": [
      { "drop": "meat_tainted", "type": "flesh", "mass_ratio": 0.33 },
      { "drop": "fat_tainted", "type": "flesh", "mass_ratio": 0.1 },
      { "drop": "jabberwock_heart", "base_num": [ 0, 1 ], "scale_num": [ 0.6, 0.9 ], "max": 3, "type": "flesh" }
    ],
},
{
  "id": "mammal_large_fur",
  "//": "drops large stomach",
  "type": "harvest",
  "entries": [
    { "drop": "meat", "type": "flesh", "mass_ratio": 0.32 },
    { "drop": "meat_scrap", "type": "flesh", "mass_ratio": 0.01 },
    { "drop": "lung", "type": "flesh", "mass_ratio": 0.0035 },
    { "drop": "liver", "type": "offal", "mass_ratio": 0.01 },
    { "drop": "brain", "type": "flesh", "mass_ratio": 0.005 },
    { "drop": "sweetbread", "type": "flesh", "mass_ratio": 0.002 },
    { "drop": "kidney", "type": "offal", "mass_ratio": 0.002 },
    { "drop": "stomach_large", "scale_num": [ 1, 1 ], "max": 1, "type": "offal" },
    { "drop": "bone", "type": "bone", "mass_ratio": 0.15 },
    { "drop": "sinew", "type": "bone", "mass_ratio": 0.00035 },
    { "drop": "fur", "type": "skin", "mass_ratio": 0.02 },
    { "drop": "fat", "type": "flesh", "mass_ratio": 0.07 }
  ]
},
{
  "id": "CBM_SCI",
  "type": "harvest",
  "entries": [
    {
      "drop": "bionics_sci",
      "type": "bionic_group",
      "flags": [ "FILTHY", "NO_STERILE", "NO_PACKED" ],
      "faults": [ "fault_bionic_salvaged" ]
    },
    { "drop": "meat_tainted", "type": "flesh", "mass_ratio": 0.25 },
    { "drop": "fat_tainted", "type": "flesh", "mass_ratio": 0.08 },
    { "drop": "bone_tainted", "type": "bone", "mass_ratio": 0.1 }
  ]
},
```

#### `id`

Unique id of the harvest definition.

#### `type`

Should always be `harvest` to mark the object as a harvest definition.

#### `message`

Optional message to be printed when a creature using the harvest definition is butchered. May be
omitted from definition.

#### `entries`

Array of dictionaries defining possible items produced on butchering and their likelihood of being
produced. `drop` value should be the `id` string of the item to be produced.

`type` value should be a string with the associated body part the item comes from. Acceptable values
are as follows: `flesh`: the "meat" of the creature. `offal`: the "organs" of the creature. these
are removed when field dressing. `skin`: the "skin" of the creature. this is what is ruined while
quartering. `bone`: the "bones" of the creature. you will get some amount of these from field
dressing, and the rest of them from butchering the carcass. `bionic`: an item gained by dissecting
the creature. not restricted to CBMs. `bionic_group`: an item group that will give an item by
dissecting a creature. not restricted to groups containing CBMs.

`flags` value should be an array of strings. It's the flags that will be added to te items of that
entry upon harvesting.

`faults` value should be an array of `fault_id` strings. It's the faults that will be added to te
items of that entry upon harvesting.

For every `type` other then `bionic` and `bionic_group` following entries scale the results:
`base_num` value should be an array with two elements in which the first defines the minimum number
of the corresponding item produced and the second defines the maximum number. `scale_num` value
should be an array with two elements, increasing the minimum and maximum drop numbers respectively
by element value * survival skill. `max` upper limit after `bas_num` and `scale_num` are calculated
using `mass_ratio` value is a multiplier of how much of the monster's weight comprises the
associated item. to conserve mass, keep between 0 and 1 combined with all drops. This overrides
`base_num`, `scale_num` and `max`

For `type`s: `bionic` and `bionic_group` following enrties can scale the results: `max` this value
(in contrary to `max` for other `type`s) corresponds to maximum butchery roll that will be passed to
check_butcher_cbm() in activity_handlers.cpp; view check_butcher_cbm() to see corresponding
distribution chances for roll values passed to that function

### Weapon Category

Used to classify weapons (guns or melee) into groups, mainly for use in martial arts.

```json
{
  "type": "weapon_category",
  "id": "WEAP_CAT",
  "name": "Weapon Category"
}
```

`"name"` is a translatable string used for UI display in martial arts UI, while ID is used for JSON
entries.

### Furniture

```json
{
  "type": "furniture",
  "id": "f_toilet",
  "name": "toilet",
  "symbol": "&",
  "looks_like": "chair",
  "color": "white",
  "move_cost_mod": 2,
  "light_emitted": 5,
  "required_str": 18,
  "flags": ["TRANSPARENT", "BASHABLE", "FLAMMABLE_HARD"],
  "crafting_pseudo_item": "anvil",
  "examine_action": "toilet",
  "close": "f_foo_closed",
  "open": "f_foo_open",
  "lockpick_result": "f_safe_open",
  "lockpick_message": "With a click, you unlock the safe.",
  "provides_liquids": "beer",
  "bash": "TODO",
  "deconstruct": "TODO",
  "max_volume": "1000 L",
  "examine_action": "workbench",
  "workbench": { "multiplier": 1.1, "mass": 10000, "volume": "50L" },
  "boltcut": {
    "result": "f_safe_open",
    "duration": "1 seconds",
    "message": "The safe opens.",
    "sound": "Gachunk!",
    "byproducts": [{ "item": "scrap", "count": 3 }]
  },
  "hacksaw": {
    "result": "f_safe_open",
    "duration": "12 seconds",
    "message": "The safe is hacksawed open!",
    "sound": "Gachunk!",
    "byproducts": [{ "item": "scrap", "count": 13 }]
  }
}
```

#### `type`

Fixed string, must be `furniture` to identify the JSON object as such.

`"id", "name", "symbol", "looks_like", "color", "bgcolor", "max_volume", "open", "close", "bash", "deconstruct", "examine_action", "flgs`

Same as for terrain, see below in the chapter "Common to furniture and terrain".

#### `move_cost_mod`

Movement cost modifier (`-10` = impassable, `0` = no change). This is added to the movecost of the
underlying terrain.

#### `lockpick_result`

(Optional) When the furniture is successfully lockpicked, this is the furniture it will turn into.

#### `lockpick_message`

(Optional) When the furniture is successfully lockpicked, this is the message that will be printed
to the player. When it is missing, a generic `"The lock opens…"` message will be printed instead.

#### `oxytorch`

(Optional) Data for using with an oxytorch.

```cpp
oxytorch: {
    "result": "furniture_id", // (optional) furniture it will become when done, defaults to f_null
    "duration": "1 seconds", // ( optional ) time required for oxytorching, default is 1 second
    "message": "You quickly cut the metal", // ( optional ) message that will be displayed when finished
    "byproducts": [ // ( optional ) list of items that will be spawned when finished
        {
            "item": "item_id",
            "count": 100 // exact amount
        },
        {
            "item": "item_id",
            "count": [ 10, 100 ] // random number in range ( inclusive )
        }
    ]
}
```

#### `light_emitted`

How much light the furniture produces. 10 will light the tile it's on brightly, 15 will light that
tile and the tiles around it brightly, as well as slightly lighting the tiles two tiles away from
the source. For examples: An overhead light is 120, a utility light, 240, and a console, 10.

#### `boltcut`

(Optional) Data for using with an bolt cutter.

```cpp
"boltcut": {
    "result": "furniture_id", // (optional) furniture it will become when done, defaults to f_null
    "duration": "1 seconds", // ( optional ) time required for bolt cutting, default is 1 second
    "message": "You finish cutting the metal.", // ( optional ) message that will be displayed when finished
    "sound": "Gachunk!", // ( optional ) description of the sound when finished
    "byproducts": [ // ( optional ) list of items that will be spawned when finished
        {
            "item": "item_id",
            "count": 100 // exact amount
        },
        {
            "item": "item_id",
            "count": [ 10, 100 ] // random number in range ( inclusive )
        }
    ]
}
```

#### `hacksaw`

(Optional) Data for using with an hacksaw.

```cpp
"hacksaw": {
    "result": "furniture_id", // (optional) furniture it will become when done, defaults to f_null
    "duration": "1 seconds", // ( optional ) time required for hacksawing, default is 1 second
    "message": "You finish cutting the metal.", // ( optional ) message that will be displayed when finished
    "byproducts": [ // ( optional ) list of items that will be spawned when finished
        {
            "item": "item_id",
            "count": 100 // exact amount
        },
        {
            "item": "item_id",
            "count": [ 10, 100 ] // random number in range ( inclusive )
        }
    ]
}
```

#### `required_str`

Strength required to move the furniture around. Negative values indicate an unmovable furniture.

#### `crafting_pseudo_item`

(Optional) Id of an item (tool) that will be available for crafting when this furniture is range
(the furniture acts as an item of that type). Can be made into an array. Example:
`"crafting_pseudo_item": [ "fake_gridwelder", "fake_gridsolderingiron" ],`

#### `workbench`

(Optional) Can craft here. Must specify a speed multiplier, allowed mass, and allowed volume.
Mass/volume over these limits incur a speed penalty. Must be paired with a `"workbench"`
`examine_action` to function.

#### `plant_data`

(Optional) This is a plant. Must specify a plant transform, and a base depending on context. You can
also add a harvest or growth multiplier if it has the `GROWTH_HARVEST` flag.

#### `surgery_skill_multiplier`

(Optional) Surgery skill multiplier (float) applied by this furniture to survivor standing next to
it for the purpose of surgery.

#### `provides_liquids`

(Optional) Dispenses infinite amounts of specified liquid item when interacted. Must be used with
`"examine_action": "liquid_source"` to work.

### Terrain

```json
{
  "type": "terrain",
  "id": "t_spiked_pit",
  "name": "spiked pit",
  "symbol": "0",
  "looks_like": "pit",
  "color": "ltred",
  "move_cost": 10,
  "light_emitted": 10,
  "trap": "spike_pit",
  "max_volume": "1000 L",
  "flags": ["TRANSPARENT", "DIGGABLE"],
  "digging_result": "digging_sand_50L",
  "connects_to": "WALL",
  "close": "t_foo_closed",
  "open": "t_foo_open",
  "lockpick_result": "t_door_unlocked",
  "lockpick_message": "With a click, you unlock the door.",
  "bash": "TODO",
  "deconstruct": "TODO",
  "harvestable": "blueberries",
  "transforms_into": "t_tree_harvested",
  "harvest_season": "WINTER",
  "roof": "t_roof",
  "examine_action": "pit",
  "boltcut": {
    "result": "t_door_unlocked",
    "duration": "1 seconds",
    "message": "The door opens.",
    "sound": "Gachunk!",
    "byproducts": [{ "item": "scrap", "2x4": 3 }]
  },
  "hacksaw": {
    "result": "t_door_unlocked",
    "duration": "12 seconds",
    "message": "The door is hacksawed open!",
    "sound": "Gachunk!",
    "byproducts": [{ "item": "scrap", "2x4": 13 }]
  }
}
```

#### `type`

Fixed string, must be "terrain" to identify the JSON object as such.

`"id", "name", "symbol", "looks_like", "color", "bgcolor", "max_volume", "open", "close", "bash", "deconstruct", "examine_action", "flgs`

Same as for furniture, see below in the chapter "Common to furniture and terrain".

#### `move_cost`

Move cost to move through. A value of 0 means it's impassable (e.g. wall). You should not use
negative values. The positive value is multiple of 50 move points, e.g. value 2 means the player
uses `2 * 50 = 100` move points when moving across the terrain.

#### `light_emitted`

How much light the terrain emits. 10 will light the tile it's on brightly, 15 will light that tile
and the tiles around it brightly, as well as slightly lighting the tiles two tiles away from the
source. For examples: An overhead light is 120, a utility light, 240, and a console, 10.

#### `digging_result`

(Optional) String defining the ID of what itemgroup this terrain will produce when a pit is dug
here.

Only relevant for terrain with the `DIGGABLE` flag. If not specificed, default is itemgroup
`digging_soil_loam_50L`. Note as well that this group will be called 4 times by default, 8 times if
the terrain has the `DIGGABLE_CAN_DEEPEN` flag.

#### `lockpick_result`

(Optional) When the terrain is successfully lockpicked, this is the terrain it will turn into.

#### `lockpick_message`

(Optional) When the terrain is successfully lockpicked, this is the message that will be printed to
the player. When it is missing, a generic `"The lock opens…"` message will be printed instead.

#### `oxytorch`

(Optional) Data for using with an oxytorch.

```cpp
oxytorch: {
    "result": "terrain_id", // terrain it will become when done
    "duration": "1 seconds", // ( optional ) time required for oxytorching, default is 1 second
    "message": "You quickly cut the bars", // ( optional ) message that will be displayed when finished
    "byproducts": [ // ( optional ) list of items that will be spawned when finished
        {
            "item": "item_id",
            "count": 100 // exact amount
        },
        {
            "item": "item_id",
            "count": [ 10, 100 ] // random number in range ( inclusive )
        }
    ]
}
```

#### `trap`

(Optional) Id of the build-in trap of that terrain.

For example the terrain `t_pit` has the built-in trap `tr_pit`. Every tile in the game that has the
terrain `t_pit` also has, therefore, an implicit trap `tr_pit` on it. The two are inseparable (the
player can not deactivate the built-in trap, and changing the terrain will also deactivate the
built-in trap).

A built-in trap prevents adding any other trap explicitly (by the player and through mapgen).

#### `harvestable`

(Optional) If defined, the terrain is harvestable. This entry defines the item type of the harvested
fruits (or similar). To make this work, you also have to set one of the `harvest_*` `examine_action`
functions.

#### `boltcut`

(Optional) Data for using with an bolt cutter.

```cpp
"boltcut": {
    "result": "terrain_id", // terrain it will become when done
    "duration": "1 seconds", // ( optional ) time required for bolt cutting, default is 1 second
    "message": "You finish cutting the metal.", // ( optional ) message that will be displayed when finished
    "sound": "Gachunk!", // ( optional ) description of the sound when finished
    "byproducts": [ // ( optional ) list of items that will be spawned when finished
        {
            "item": "item_id",
            "count": 100 // exact amount
        },
        {
            "item": "item_id",
            "count": [ 10, 100 ] // random number in range ( inclusive )
        }
    ]
}
```

#### `hacksaw`

(Optional) Data for using with an hacksaw.

```cpp
"hacksaw": {
    "result": "terrain_id", // terrain it will become when done
    "duration": "1 seconds", // ( optional ) time required for hacksawing, default is 1 second
    "message": "You finish cutting the metal.", // ( optional ) message that will be displayed when finished
    "byproducts": [ // ( optional ) list of items that will be spawned when finished
        {
            "item": "item_id",
            "count": 100 // exact amount
        },
        {
            "item": "item_id",
            "count": [ 10, 100 ] // random number in range ( inclusive )
        }
    ]
}
```

#### `transforms_into`

(Optional) Used for various transformation of the terrain. If defined, it must be a valid terrain
id. Used for example:

- When harvesting fruits (to change into the harvested form of the terrain).
- In combination with the `HARVESTED` flag and `harvest_season` to change the harvested terrain back
  into a terrain with fruits.

#### `harvest_season`

(Optional) On of "SUMMER", "AUTUMN", "WINTER", "SPRING", used in combination with the "HARVESTED"
flag to revert the terrain back into a terrain that can be harvested.

#### `roof`

(Optional) The terrain of the terrain on top of this (the roof).

### Common To Furniture And Terrain

Some values can/must be set for terrain and furniture. They have the same meaning in each case.

#### `id`

Id of the object, this should be unique among all object of that type (all terrain or all furniture
types). By convention (but technically not needed), the id should have the "f_" prefix for furniture
and the "t_" prefix for terrain. This is not translated. It must not be changed later as that would
break save compatibility.

#### `name`

Displayed name of the object. This will be translated.

#### `flags`

(Optional) Various additional flags, see "doc/JSON_FLAGS.md".

#### `connects_to`

(Optional) The group of terrains to which this terrain connects. This affects tile rotation and
connections, and the ASCII symbol drawn by terrain with the flag "AUTO_WALL_SYMBOL".

Current values:

- `CHAINFENCE`
- `RAILING`
- `WALL`
- `WATER`
- `WOODFENCE`
- `GUTTER`

Example: `-` , `|` , `X` and `Y` are terrain which share the same `connects_to` value. `O` does not
have it. `X` and `Y` also have the `AUTO_WALL_SYMBOL` flag. `X` will be drawn as a T-intersection
(connected to west, south and east), `Y` will be drawn as a horizontal line (going from west to
east, no connection to south).

```
-X-    -Y-
 |      O
```

#### `symbol`

ASCII symbol of the object as it appears in the game. The symbol string must be exactly one
character long. This can also be an array of 4 strings, which define the symbol during the different
seasons. The first entry defines the symbol during spring. If it's not an array, the same symbol is
used all year round.

#### `comfort`

How comfortable this terrain/furniture is. Impact ability to fall asleep on it. uncomfortable =
-999, neutral = 0, slightly_comfortable = 3, comfortable = 5, very_comfortable = 10

#### `floor_bedding_warmth`

Bonus warmth offered by this terrain/furniture when used to sleep.

#### `bonus_fire_warmth_feet`

Increase warmth received on feet from nearby fire (default = 300)

#### `looks_like`

id of a similar item that this item looks like. The tileset loader will try to load the tile for
that item if this item doesn't have a tile. looks_like entries are implicitly chained, so if
'throne' has looks_like 'big_chair' and 'big_chair' has looks_like 'chair', a throne will be
displayed using the chair tile if tiles for throne and big_chair do not exist. If a tileset can't
find a tile for any item in the looks_like chain, it will default to the ascii symbol.

#### `color` or `bgcolor`

Color of the object as it appears in the game. "color" defines the foreground color (no background
color), "bgcolor" defines a solid background color. As with the "symbol" value, this can be an array
with 4 entries, each entry being the color during the different seasons.

> **NOTE**: You must use ONLY ONE of "color" or "bgcolor"

#### `max_volume`

(Optional) Maximal volume that can be used to store items here. Volume in ml and L can be used - "50
ml" or "2 L"

#### `examine_action`

(Optional) The json function that is called when the object is examined. See "src/iexamine.h".

#### `close" And "open`

(Optional) The value should be a terrain id (inside a terrain entry) or a furniture id (in a
furniture entry). If either is defined, the player can open / close the object. Opening / closing
will change the object at the affected tile to the given one. For example one could have object
"safe_c", which "open"s to "safe_o" and "safe_o" in turn "close"s to "safe_c". Here "safe_c" and
"safe_o" are two different terrain (or furniture) types that have different properties.

#### `bash`

(Optional) Defines whether the object can be bashed and if so, what happens. See "map_bash_info".

#### `deconstruct`

(Optional) Defines whether the object can be deconstructed and if so, what the results shall be. See
"map_deconstruct_info".

#### `pry`

(Optional) Defines whether the object can be pried open and if so, what happens. See
"prying_result".

#### `map_bash_info`

Defines the various things that happen when the player or something else bashes terrain or
furniture.

```json
{
  "str_min": 80,
  "str_max": 180,
  "str_min_blocked": 15,
  "str_max_blocked": 100,
  "str_min_supported": 15,
  "str_max_supported": 100,
  "sound": "crunch!",
  "sound_vol": 2,
  "sound_fail": "whack!",
  "sound_fail_vol": 2,
  "ter_set": "t_dirt",
  "furn_set": "f_rubble",
  "explosive": 1,
  "collapse_radius": 2,
  "destroy_only": true,
  "bash_below": true,
  "tent_centers": ["f_groundsheet", "f_fema_groundsheet", "f_skin_groundsheet"],
  "items": "bashed_item_result_group"
}
```

#### `str_min`, `str_max`, `str_min_blocked`, `str_max_blocked`, `str_min_supported`, `str_max_supported`

TODO

#### `sound`, `sound_fail`, `sound_vol`, `sound_fail_vol`

(Optional) Sound and volume of the sound that appears upon destroying the bashed object or upon
unsuccessfully bashing it (failing). The sound strings are translated (and displayed to the player).

#### `furn_set`, `ter_set`

The terrain / furniture that will be set when the original is destroyed. This is mandatory for bash
entries in terrain, but optional for entries in furniture (it defaults to no furniture).

#### `explosive`

(Optional) If greater than 0, destroying the object causes an explosion with this strength (see
`game::explosion`).

#### `destroy_only`

TODO

#### `bash_below`

TODO

#### `tent_centers`, `collapse_radius`

(Optional) For furniture that is part of tents, this defines the id of the center part, which will
be destroyed as well when other parts of the tent get bashed. The center is searched for in the
given "collapse_radius" radius, it should match the size of the tent.

#### `items`

(Optional) An item group (inline) or an id of an item group, see doc/ITEM_SPAWN.md. The default
subtype is "collection". Upon successful bashing, items from that group will be spawned.

#### `map_deconstruct_info`

```json
{
  "furn_set": "f_safe",
  "ter_set": "t_dirt",
  "items": "deconstructed_item_result_group"
}
```

#### `furn_set`, `ter_set`

The terrain / furniture that will be set after the original has been deconstructed. "furn_set" is
optional (it defaults to no furniture), "ter_set" is only used upon "deconstruct" entries in terrain
and is mandatory there.

#### `items`

(Optional) An item group (inline) or an id of an item group, see doc/ITEM_SPAWN.md. The default
subtype is "collection". Upon deconstruction the object, items from that group will be spawned.

#### `prying_result`

```json
{
  "success_message": "You pry open the door.",
  "fail_message": "You pry, but cannot pry open the door.",
  "break_message": "You damage the door!",
  "pry_quality": 2,
  "pry_bonus_mult": 3,
  "noise": 12,
  "break_noise": 10,
  "sound": "crunch!",
  "break_sound": "crack!",
  "breakable": true,
  "difficulty": 8,
  "new_ter_type": "t_door_o",
  "new_furn_type": "f_crate_o",
  "break_ter_type": "t_door_b",
  "break_furn_type": "f_null",
  "break_items": [
    { "item": "2x4", "prob": 25 },
    { "item": "wood_panel", "prob": 10 },
    { "item": "splinter", "count": [1, 2] },
    { "item": "nail", "charges": [0, 2] }
  ]
}
```

#### `new_ter_type`, `new_furn_type`

The terrain / furniture that will be set after the original has been pried open. "furn_set" is
optional (it defaults to no furniture), "ter_set" is only used upon "pry" entries in terrain and is
mandatory there.

#### `success_message`, `fail_message`, `break_message`

Messages displayed on successfully prying open the terrain / furniture, on failure, or should the
terrain / furniture break into something else from a failed pry attempt. `break_message` is only
required if `breakable` is set to true and `break_ter_type` is defined.

#### `pry_quality`, `pry_bonus_mult`, `difficulty`

This determines the minimum prying quality needed to attempt to pry open the terrain / furniture,
and the chance of successfully prying it open. From iuse.cpp:

```cpp
int diff = pry->difficulty;
diff -= ( ( pry_level - pry->pry_quality ) * pry->pry_bonus_mult );
```

```cpp
if( dice( 4, diff ) < dice( 4, p->str_cur ) ) {
    p->add_msg_if_player( m_good, pry->success_message );
```

`difficulty` is compared to the character's current strength during the prying attempt. If the
available prying quality of your tool is above the required `pry_quality`, effective difficulty is
reduced at a rate determined by `pry_bonus_mult`. `pry_bonus_mult` is optional, and defaults to 1
(meaning for every level of prying quality your tool has beyond the minimum, `diff` will be reduced
by 1).

#### `noise`, `break_noise`, 'sound', 'break_sound'

If `noise` is specified, successfully prying the terrain / furniture open will play `sound` at the
specified volume. If `breakable` is true and `break_noise` is specified, breaking the terrain /
furniture on a failed prying attempt will play `break_noise` at the specified volume.

If `noise` or `break_noise` are not specified then prying or breaking the terrain / furniture in
question will simply be silent and make no sound, making them optional. `sound` and `break_sound`
are also optional, with default messages of "crunch!" and "crack!" respectively.

#### `breakable`, `break_ter_type`, `break_furn_type`

If `breakable` is set to true, then failed pry attempts have a chance of breaking the terrain /
furniture instead. For terrain, `break_ter_type` is mandatory if `breakable` is set to true,
`break_furn_type` is optional and defaults to null.

#### `break_items`

(Optional) An item group (inline) or an id of an item group, see doc/ITEM_SPAWN.md. The default
subtype is "collection". If `breakable` is set to true, breaking the object from a failed pry
attempt will spawn items from that group.

### `plant_data`

```json
{
  "transform": "f_planter_harvest",
  "base": "f_planter",
  "growth_multiplier": 1.2,
  "harvest_multiplier": 0.8
}
```

#### `transform`

What the `PLANT` furniture turn into when it grows a stage, or what a `PLANTABLE` furniture turns
into when it is planted on.

#### `base`

What the 'base' furniture of the `PLANT` furniture is - what it would be if there was not a plant
growing there. Used when monsters 'eat' the plant to preserve what furniture it is.

#### `growth_multiplier`

A flat multiplier on the growth speed on the plant. For numbers greater than one, it will take
longer to grow, and for numbers less than one it will take less time to grow.

#### `harvest_multiplier`

A flat multiplier on the harvest count of the plant. For numbers greater than one, the plant will
give more produce from harvest, for numbers less than one it will give less produce from harvest.

### trap

```json
{
  "type": "trap",                // Defines this as a trap
  "id": "tr_caltrops_glass",     // Unique identifier
  "name": "glass caltrops",      // Displayed name
  "color": "dark_gray",          // Color of symbol displayed if tiles are disabled or if no sprite is defined for it
  "symbol": "_",                 // Symbol display if tiles are disabled or if no sprite is defined
  "looks_like": "tr_caltrops",   // hint to tilesets if this part has no tile, use the looks_like tile
  "visibility": 6,               // Difficulty of spotting this trap if not placed by the player, checked against perception. 0 means the trap is never concealed from the player
  "avoidance": 6,                // How likely the trap is to be set off when stepped into. 0 means the trap will never be set off if stepped onto
  "difficulty": 0,               // How hard the trap is to disarm, higher is harder.
  "action": "caltrops_glass",    // What the trap does when triggered, see trapfunc.cpp or below for more info
  "spell_data": { "id": "spell_trap_can_alarm_trigger" },   // `id` of spell cast by a trap with the `spell` trap action
  "map_regen": "microlab_shifting_hall",    // Used by traps with the `map_regen` action, defines what `mapgen_update` entry is used for the area it's applied to.
  "remove_on_trigger": true,     // Does the trap remove itself after being set off? Omit if the trap is intended to stick around and trigger repeatedly, not recommended for traps generated by terrain
  "trigger_items": [ "tripwire", "shotgun_d", { "item": "shot_hull", "quantity": 2, "charges": 1 } ],   // What items spawn when the trap is triggered, times `quantity` and of `charges` amount (used if item has a default stack size like ammo). Note that the `beartrap`, `snare_light`, `snare_heavy`, `pit_spikes`, `pit_glass`, and `crossbow` still have some items that are spawned by hardcoded behavior for various reasons.
  "drops": [ "tripwire", "shotgun_d", { "item": "shot_00", "quantity": 2, "charges": 1 } ],   // That items drop when the trap is succesfully disarmed. Optional use of `quantity` or `charges` follows same conventions as with `trigger_items`
  "vehicle_data": {              // Block of behaviors that occur when a vehicle's wheel rolls over this trap, separate from normal behaviors of traps being set off by players or monsters
    "do_explosion": true,        // If true, create an explosion instead of simply applying damage value to the exact vehicle tile that triggered the trap
    "damage": 1000,              // Direct damage applied to the vehicle tile that triggered it, or base damage at epicenter, depending on if `do_explosion` is true
    "shrapnel": 8,               // Fragmentation damage if set to explode
    "sound_volume": 10,          // Volume of sound, if specified, that plays when triggering
    "sound": "Boom!",            // Sound description as posted in message log, in the form of "You hear %s"
    "sound_type": "explosion",   // Possible types are: background, weather, music, movement, speech, activity, destructive_activity, alarm, combat, alert, order
    "sound_variant": "default"   // Sound variant used
  },
  "benign": true,                // This means it's not a trap meant to be set off but something harmless like a rollmat or funnel, so won't ask the player if they're really sure about stepping into it, and monsters with the `PATH_AVOID_DANGER_2` monflag won't care about avoiding these. Using this with an `action` that can actually cause harm is a Bad Idea
  "funnel_radius": 200           // Used for funnels, when it's raining this trap will collect water at a rate based on this value, automatically filling empty container items placed on the same tile as it
  "floor_bedding_warmth": -1000, // Used for rollmats and the like, bonus warmth offered by this terrain/furniture when used to sleep.
},
```

#### action

The following actions are available as defined in trapfunc.cpp:

- `none` - Does nothing, used for funnels and other things where no trap behavior is necessary.
- `bubble` - Makes a pop sound from stepping on bubble wrap, and that's all.
- `glass` - Deals minor cutting damage to whatever steps on it, crunchy glass noises.
- `cot` - Perfectly safe and cozy if you're a player or NPC, but monsters will stumble over it and
  lose a turn.
- `beartrap` - Deals damage and keeps the victim locked to that tile until they break free. If so,
  the item itself is dropped later on once they free themselves.
- `board` - Deals some minor damage to whatever steps on it, and subtracts some moves from them.
- `caltrops` - Deals damage to whatever steps on it, and subtracts some moves from them.
- `caltrops_glass` - Same basic effect as `caltrops_glass` but with more glass crunching noises.
- `tripwire` - Deals minor damage if it successfully trips the victim, knocking them back and
  substracting moves from them.
- `crossbow` - Deals piercing damage with a chance of missing, the ammunition being dropped
  afterward is still hardcoded into this due to having a random chance to spawn.
- `shotgun` - Deals heavy ballistic damage to whoever sets it off. If the trap's ID is specifically
  `tr_shotgun_2`, the victim is hit twice instead of only once.
- `blade` - Lashes out and strikes whoever sets it off with direct bashing and cutting damage.
- `snare_light` - Deals damage and keeps the victim locked to that tile until they break free,
  spawning the relevant items when the effect wears off.
- `snare_heavy` - Comparable to `snare_light` but higher damage, and different item spawns when the
  target frees themselves.
- `landmine` - Explosion with moderate damage and heavy fragmentation.
- `boobytrap` - Same effect as `landmine` but different message printed on trigger.
- `telepad` - Teleports the victim to a random space within 8 tiles, with risk of teleporting into a
  wall, and inflicts teleglow on them.
- `goo` - Players and NPCs have a chance of taking damage and are slowed down by slime, non-slime
  monsters take a speed debuff and non-robot monsters get turned into slimes.
- `dissector` - Deals cutting damage to victims, creatures of species `ROBOT` are immune.
- `pit` - Victim takes falling damage and is held in place until they climb back out.
- `pit_spikes` - As with `pit` but additional damage from spikes. Random chance of the spikes
  breaking, spawning additional items and converting underlying terrain to `t_pit`.
- `pit_glass` - Similiar to `pit_spikes`, except when it decides to break it spawns glass shards
  instead of wooden spears.
- `lava` - High levels of heat damage dealt to whatever sets it off.
- `portal` - Currently aliases to `telepad` and triggers the exact same effects.
- `sinkhole` - If triggered, converts the underlying terrain to `t_pit` and triggers the effect of
  falling into a pit. Players can attempt to prevent this if they have a grappling hook, bullwhip,
  or long rope in their inventory, or if they have the Web Diver mutation.
- `ledge` - Fall down a level to whatever is below, triggering potential falling damage.
- `temple_flood` - Used in strange temples to trigger the hardcoded event that floors that converts
  floor in the area to deep water. Only the player can set this off, not NPCs or monsters.
- `temple_toggle` - Used in strange temples, toggles red/green/blue puzzle tiles between floor and
  wall forms. Only the player can set this off, not NPCs or monsters.
- `glow` - Random chance of irradiating and flashbanging players and NPCs, monsters have a chance of
  suffering acid damage and a speed debuff instead.
- `hum` - Makes a humming noise of a randomized volume, ranging from barely audiable to deafening.
- `shadow` - Summons shadow monsters nearby. Only the player can set this off, not NPCs or monsters.
- `map_regen` - Applies the mapgen update specified in the trap's `map_regen` to the map tile the
  trap is in, for altering the map. If used, `remove_on_trigger` and `trigger_items` aren't
  processed as the mapgen update could just as easily remove the trap, change it to something else,
  or do any number of things that would otherwise disrupt the trap cleanup function.
- `drain` - Deals a tiny amount of damage to the target, ignores armor and immunities.
- `spell` - Casts the spell specified by the trap's `spell_data`, centered on whatever set it off.
  Note that the spell used generally requires a `min_aoe` defined to work successfully, and not all
  spell effects can be expected to work properly with this.
- `snake` - Similar to `shadow` trap effect, summons shadow snakes nearby. Main difference is NPCs
  and monsters are capable of setting it off.

### clothing_mod

```json
"type": "clothing_mod",
"id": "leather_padded",   // Unique ID.
"flag": "leather_padded", // flag to add to clothing.
"item": "leather",        // item to consume.
"implement_prompt": "Pad with leather",      // prompt to show when implement mod.
"destroy_prompt": "Destroy leather padding", // prompt to show when destroy mod.
"restricted": true,       // (optional) If true, clothing must list this mod's flag in "valid_mods" list to use it. Defaults to false.
"mod_value": [            // List of mod effect.
    {
        "type": "bash",   // "bash", "cut", "bullet", "fire", "acid", "warmth", "storage", and "encumbrance" is available.
        "value": 1,       // value of effect.
        "round_up": false // (optional) round up value of effect. defaults to false.
        "proportion": [   // (optional) value of effect propotions to clothing's parameter.
            "thickness",  //            Add value for every layer of "material_thickness" the item has.
            "volume",     //            Add value for every liter of baseline volume the item has.
            "coverage"    //            Reduce value by percentage of average coverage the item has.
        ]
    }
]
```

## Obsoletion and migration

For maps, you remove the item from all the places it can spawn, remove the mapgen entries, and add
the overmap terrain id into `data/json/obsoletion/migration_oter_ids.json`, to migrate oter_id
`underground_sub_station` and `sewer_sub_station` into their rotatable versions, note that if mapgen
has already generated this area this will only alter the tile shown on the overmap:

```json
{
  "type": "oter_id_migration",
  "//": "obsoleted in 0.4",
  "old_directions": false,
  "new_directions": false,
  "oter_ids": {
    "underground_sub_station": "underground_sub_station_north",
    "sewer_sub_station": "sewer_sub_station_north"
  }
}
```

If `old_directions` option is enabled each entry will create four migrations: for `old_north`,
`old_west`, `old_south`, and `old_east`. What they will be migrated to depends of value of
`new_directions` option. If it is set to `true` then terrains will be migrated to same directions:
`old_north` to `new_north`, `old_east` to `new_east` and such. If `new_directions` is set to
`false`, then all four terrains will be migrated to one plain `new`. For both of those cases you
only need to specify plain `old` and `new` names in `oter_ids` map, without any suffixes.
