---
title: Test JSON without launching game
---

## [`--check-mods`](../../dev/reference/cli_options.md#check-mods-mods)

```sh
./cataclysm-tiles --check-mods <mod_name>
```

Cataclysm: BN executable itself can be used to test mods without loading any saves. This is useful
for quickly checking if there's any errors in your JSON.

:::tip

Core BN content itself is also a mod (`bn`) and can be checked using same method.

:::

```sh
$ ./cataclysm-tiles --check-mods bn
Checking mod Bright Nights [bn]
Error loading data: Json error: data/json/recipes/weapon/bashing.json:23:16: invalid quantity string: unknown unit

    "skills_required": [ "survival", 2 ],
    "difficulty": 3,
    "time": "360
               ^
                 M",
    "autolearn": true,
    "qualities": [ { "id": "SAW_W", "level": 1 }, { "id": "CUT", "level": 1 } ],
```

## Caveats

:::caution

The game should still be load-tested, as many errors may not get caught or generate proper error
messages.

:::

```jsonc
{
  "type": "recipe",
  "result": "shillelagh",
  "category": "CC_WEAPON",
  "subcategory": "CSC_WEAPON_BASHING",
  "skill_used": "fabrication",
  "skills_required": ["survival", 2],
  "difficulty": 3,
  "time": "360 m",
  "autolearn": "afdsafds", // <- this should be a boolean
  "qualities": [{ "id": "SAW_W", "level": 1 }, { "id": "CUT", "level": 1 }],
  "tools": [[["char_smoker", 100]]],
  "components": [[["log", 1]], [["butter", 30], ["edible_lard", 4, "LIST"]]]
}
```

for example, the test failed without any error messages:

```sh
$ ./cataclysm-tiles --check-mods bn
Checking mod Bright Nights [bn]
$ echo $status
1 # <- non-zero exit code means error
```
