import { assertEquals } from "@std/assert"
import { GunsMigrateEnergy, ItemMigrateEnergy } from "./5259.migrate_energy.ts"
import { prettyParse } from "../utils.ts"

Deno.test("GunsMigrateEnergy migrates ups_charges -> power_draw", () => {
  assertEquals(prettyParse(GunsMigrateEnergy, { ups_charges: 10 }), { power_draw: "10 kJ" })
})

Deno.test("ItemMigrateEnergy migrates fields", () => {
  const given = {
    "id": "talking_doll",
    "type": "TOOL",
    "ammo": "battery",
    "charges_per_use": 1,
    "use_action": "DOLLCHAT",
    "magazines": [
      [
        "battery",
        [
          "light_minus_disposable_cell",
          "light_disposable_cell",
          "light_minus_battery_cell",
          "light_battery_cell",
          "light_plus_battery_cell",
          "light_atomic_battery_cell",
          "light_minus_atomic_battery_cell",
        ],
      ],
    ],
    "magazine_well": "250 ml",
  }
  const expected = {
    "id": "talking_doll",
    "type": "TOOL",
    "use_action": "DOLLCHAT",
    "battery_well": "250 ml",
    "power_draw": "1 kJ",
    "batteries": [
      "light_minus_disposable_cell",
      "light_disposable_cell",
      "light_minus_battery_cell",
      "light_battery_cell",
      "light_plus_battery_cell",
      "light_atomic_battery_cell",
      "light_minus_atomic_battery_cell",
    ],
  }
  assertEquals(prettyParse(ItemMigrateEnergy, given), expected)
})

Deno.test("ItemMigrateEnergy migrates TOOL_ARMOR", () => {
  const given = {
    "id": "exosuit_survivor",
    "type": "TOOL_ARMOR",
    "category": "armor",
    "max_charges": 500,
    "ammo": "battery",
    "use_action": {
      "type": "transform",
      "msg": "The %s engages.",
      "target": "exosuit_survivor_on",
      "active": true,
      "need_worn": true,
      "need_charges": 1,
    },
  }
  const expected = {
    "id": "exosuit_survivor",
    "type": "TOOL_ARMOR",
    "category": "armor",
    "max_power": "500 kJ",
    "use_action": {
      "type": "transform",
      "msg": "The %s engages.",
      "target": "exosuit_survivor_on",
      "active": true,
      "need_worn": true,
      "need_power": "1 kJ",
    },
  }
  assertEquals(prettyParse(ItemMigrateEnergy, given), expected)
})

Deno.test("ItemMigrateEnergy migrates plastic jack o lantern", () => {
  const given = {
    "id": "plastic_jack_o_lantern",
    "type": "TOOL",
    "initial_charges": 100,
    "max_charges": 100,
    "ammo": ["battery"],
    "use_action": {
      "transform_charges": 1,
      "type": "transform",
    },
  }
  const expected = {
    "id": "plastic_jack_o_lantern",
    "type": "TOOL",
    "use_action": {
      "type": "transform",
      "transform_power": "1 kJ",
    },
    "initial_power": "100 kJ",
    "max_power": "100 kJ",
  }
  assertEquals(prettyParse(ItemMigrateEnergy, given), expected)
})

Deno.test("ItemMigrateEnergy migrates test soldering iron", () => {
  const given = {
    "id": "test_soldering_iron",
    "type": "TOOL",
    "ammo": "battery",
    "charges_per_use": 1,
    "magazines": [
      [
        "battery",
        [
          "light_minus_battery_cell",
          "light_battery_cell",
          "light_plus_battery_cell",
          "light_atomic_battery_cell",
          "light_minus_atomic_battery_cell",
          "light_minus_disposable_cell",
          "light_disposable_cell",
        ],
      ],
    ],
    "magazine_well": 1,
  }
  const expected = {
    "id": "test_soldering_iron",
    "type": "TOOL",
    "power_draw": "1 kJ",
    "batteries": [
      "light_minus_battery_cell",
      "light_battery_cell",
      "light_plus_battery_cell",
      "light_atomic_battery_cell",
      "light_minus_atomic_battery_cell",
      "light_minus_disposable_cell",
      "light_disposable_cell",
    ],
    battery_well: "250 ml",
  }
  assertEquals(prettyParse(ItemMigrateEnergy, given), expected)
})
