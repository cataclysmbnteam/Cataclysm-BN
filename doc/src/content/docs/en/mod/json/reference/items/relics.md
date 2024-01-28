---
title: Relics
---

# Relics

Relics are regular items with special relic data attached to them.

Relic data is defined in JSON in `relic_data` field of corresponding item type definition, and
whenever the item is spawned a copy of relic data is attached to the item instance.

Relic data object can contain the following fields:

```json
{
"name": "Boots of Haste",       // Overrides default item name
"moves": 100,                   // (optional) Activation move cost (default 100)
"charges_per_activation": 1,    // (optional) Charges per activation (default 1)
"active_effects": [ {}, ... ],  // (optional) Spells executed on activation (identical to `hit_you_effect`, see [MAGIC.md](MAGIC.md/#hit_you_effect))
"passive_effects": [ {}, ... ], // (optional) List of passive effects (enchantments), see [MAGIC.md](MAGIC.md/#enchantments)
"recharge_scheme": [ {}, ... ], // (optional) List of recharge methods, see below
}
```

## Relic recharge

Relics can recharge under certain conditions. Recharge method is defined as follows (all fields
optional):

```json
{
  "type": "time", // Defines what resource is consumed. Default: time
  "req": "none", // Defines under what conditions recharge works. Default: none (no special requirements)
  "field": "fd_blood", // Field type to be consumed with 'field' recharge type
  "trap": "tr_portal", // Trap type to be consumed with 'trap' recharge type
  "interval": "5 minutes", // Interval at which the recharge check is done. Default: 1 second
  "int_min": 1, // Min intensity of related 'type' effect. Default: 0
  "int_min": 5, // Max intensity of related 'type' effect. Default: 0
  "rate": 2, // Amount of charges restored when recharge operation succeeds. Default: 0
  "message": "Your body decays!" // Optional message to print on success
}
```

### Recharge type

| ID        | Description                                                                                                                                    |
| --------- | ---------------------------------------------------------------------------------------------------------------------------------------------- |
| `time`    | Needs no additional resources                                                                                                                  |
| `solar`   | Consumes sunlight (character must be in sunlight)                                                                                              |
| `pain`    | Causes pain to recharge. Intensity controlled by `int_min` and `int_max`                                                                       |
| `hp`      | Causes damage to all body parts. Intensity controlled by `int_min` and `int_max`                                                               |
| `fatigue` | Causes fatigue and drains stamina. Fatigue drain controlled by `int_min` and `int_max`, stamina drain rolled as `[ int_min*100, int_max*100 ]` |
| `field`   | Consumes adjacent field. Allowed field intensity controlled by `int_min` and `int_max`                                                         |
| `trap`    | Consumes adjacent trap.                                                                                                                        |

### Recharge requirements

| ID              | Description                                                               |
| --------------- | ------------------------------------------------------------------------- |
| `none`          | No additional requirements (always works)                                 |
| `equipped`      | Must be worn if armor, wielded if weapon.                                 |
| `close_to_skin` | Must be worn underneath all other clothing, or be wielded with bare hands |
| `sleep`         | Character must be asleep                                                  |
| `rad`           | Character or map tile must be irradiated                                  |
| `wet`           | Character must be wet, or it's raining                                    |
| `sky`           | Character must be above z=0                                               |

# Generation

The procedural generation of artifacts is defined in Json. The object looks like the following:

```json
{
  "type": "relic_procgen_data",
  "id": "cult",
  "passive_add_procgen_values": [
    {
      "weight": 100,
      "min_value": -1,
      "max_value": 1,
      "type": "STRENGTH",
      "increment": 1,
      "power_per_increment": 250
    }
  ],
  "passive_mult_procgen_values": [
    {
      "weight": 100,
      "min_value": -1.5,
      "max_value": 1.5,
      "type": "STRENGTH",
      "increment": 0.1,
      "power_per_increment": 250
    }
  ],
  "type_weights": [{ "weight": 100, "value": "passive_enchantment_add" }],
  "items": [{ "weight": 100, "item": "spoon" }]
}
```

## passive_add_procgen_values and passive_mult_procgen_values

As the names suggest, these are _passive_ benefits/penalties to having the artifact (ie. always
present without activating the artifact's abilities). **Add** values add or subtract from existing
scores, and **mult** values multiply them. These are entered as a list of possible 'abilities' the
artifact could get. It does not by default get all these abilities, rather when it spawns it selects
from the list provided.

- **weight:** the weight of this value in the list, to be chosen randomly
- **min_value:** the minimum possible value for this value type. for add must be an integer, for
  mult it can be a float
- **max_value:** the maximum possible value for this value type. for add must be an integer, for
  mult it can be a float
- **type:** the type of enchantment value. see MAGIC.md for detailed documentation on enchantment
  values
- **increment:** the increment that is used for the power multiplier
- **power_per_increment:** the power value per increment

## type_weights

This determines the relative weight of the 'add' and 'mult' types. When generated, an artifact first
decides if it is going to apply an 'add' or a 'mult' ability based on the type_weights of each. Then
it uses the weights of the entries under the selected type to pick an ability. This continues
cycling until the artifact reaches the defined power level. Possible values right now that are
functional are:

- passive_enchantment_add
- passive_enchantment_mult

This must be included in a dataset or it could cause a crash.

## items

This provides a list of possible items that this artifact can spawn as, if it appears randomly in a
hard-coded map extra.

## Power Level

An artifact's power level is a summation of its attributes. For example, each point of strength
addition in the above object, the artifact is a +250 power, so an artifact with +2 strength would
have a power level of 500. similarly, if an artifact had a strength multiplier of 0.8, it would have
a power level of -500.
