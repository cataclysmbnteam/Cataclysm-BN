---
title: Mod Tileset
---

MOD tileset defines additional sprite sheets. It is specified as JSON object with `type` member set
to `mod_tileset`.

Example:

```json
[
  {
    "type": "mod_tileset",
    "compatibility": ["MshockXottoplus"],
    "tiles-new": [
      {
        "file": "test_tile.png",
        "tiles": [
          {
            "id": "player_female",
            "fg": 1,
            "bg": 0
          },
          {
            "id": "player_male",
            "fg": 2,
            "bg": 0
          }
        ]
      }
    ]
  }
]
```

## `compatibility`

(string)

The internal ID of the compatible tilesets. MOD tileset is only applied when base tileset's ID
exists in this field.

## `tiles-new`

Setting of sprite sheets. Same as `tiles-new` field in `tile_config`. Sprite files are loaded from
the same folder json file exists.
